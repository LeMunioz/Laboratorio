#!/usr/bin/env python3
"""
=============================================================
  Monitor de Temperatura — Servidor Flask en Raspberry Pi
  Raspberry Pi 3  ·  DHT22 / DHT11 / DS18B20
  Sirve la página web + API REST + eventos en tiempo real (SSE)
  ANGEL EDUARDO MUÑOZ PEREZ
=============================================================
"""

import time
import os
import json
import queue
import sqlite3
import threading
from datetime import datetime
from flask import Flask, Response, jsonify, render_template, stream_with_context

# ─────────────────────────────────────────────────────────
#  CONFIGURACIÓN  [CREOQ QUE LO ENCHUFMAOS MAL EN CUALTOS XDXD]
# ─────────────────────────────────────────────────────────
GPIO_PIN         = 4          #<--------
TIPO_SENSOR      = "DHT22"    
INTERVALO_SEG    = 300        # 5 minutos entre lecturas
SQLITE_ARCHIVO   = "temperaturas.db"
FLASK_PORT       = 5000
FLASK_HOST       = "0.0.0.0"  # Escuchar en todas las interfaces
# ─────────────────────────────────────────────────────────

app = Flask(__name__)

# Cola para Server-Sent Events (SSE) — push instantáneo al browser
_sse_queues: list[queue.Queue] = []
_sse_lock = threading.Lock()


# ══════════════════════════════════════
#  BASE DE DATOS
# ══════════════════════════════════════

def init_db():
    with sqlite3.connect(SQLITE_ARCHIVO) as conn:
        conn.execute("""
            CREATE TABLE IF NOT EXISTS lecturas (
                id         INTEGER PRIMARY KEY AUTOINCREMENT,
                timestamp  TEXT    NOT NULL,
                temp_c     REAL    NOT NULL,
                humedad    REAL
            )
        """)
        conn.commit()


def guardar_lectura(timestamp: str, temp_c: float, humedad: float | None):
    with sqlite3.connect(SQLITE_ARCHIVO) as conn:
        conn.execute(
            "INSERT INTO lecturas (timestamp, temp_c, humedad) VALUES (?, ?, ?)",
            (timestamp, temp_c, humedad)
        )
        conn.commit()


def obtener_ultima() -> dict | None:
    with sqlite3.connect(SQLITE_ARCHIVO) as conn:
        row = conn.execute(
            "SELECT timestamp, temp_c, humedad FROM lecturas ORDER BY id DESC LIMIT 1"
        ).fetchone()
    if row:
        return {"timestamp": row[0], "temp_c": row[1], "humedad": row[2]}
    return None


def obtener_ultima_hora() -> list[dict]:
    with sqlite3.connect(SQLITE_ARCHIVO) as conn:
        rows = conn.execute("""
            SELECT timestamp, temp_c, humedad
            FROM lecturas
            WHERE timestamp >= datetime('now', '-1 hour', 'localtime')
               OR timestamp >= datetime('now', '-1 hour')
            ORDER BY id ASC
            LIMIT 60
        """).fetchall()
    return [{"timestamp": r[0], "temp_c": r[1], "humedad": r[2]} for r in rows]


# ══════════════════════════════════════
#  SENSOR
# ══════════════════════════════════════

def leer_dht() -> tuple[float | None, float | None]:
    try:
        import adafruit_dht, board
        _pins = {4: board.D4, 17: board.D17, 22: board.D22, 27: board.D27}
        pin = _pins.get(GPIO_PIN, board.D4)
        cls = adafruit_dht.DHT22 if TIPO_SENSOR == "DHT22" else adafruit_dht.DHT11
        dht = cls(pin, use_pulseio=False)
        for _ in range(5):
            try:
                t = dht.temperature
                h = dht.humidity
                if t is not None:
                    return round(t, 2), round(h, 2) if h else None
            except RuntimeError:
                time.sleep(2)
        return None, None
    except ImportError:
        # Modo simulación
        import random
        return round(20 + random.uniform(0, 12), 2), round(45 + random.uniform(0, 30), 2)


def leer_ds18b20() -> tuple[float | None, None]:
    base = "/sys/bus/w1/devices/"
    try:
        devs = [d for d in os.listdir(base) if d.startswith("28-")]
        if not devs:
            return None, None
        with open(os.path.join(base, devs[0], "w1_slave")) as f:
            lines = f.readlines()
        if "YES" not in lines[0]:
            return None, None
        return round(int(lines[1].split("t=")[1]) / 1000.0, 2), None
    except Exception as e:
        print(f"  X DS18B20: {e}")
        return None, None


def leer_sensor() -> tuple[float | None, float | None]:
    if TIPO_SENSOR == "DS18B20":
        return leer_ds18b20()
    return leer_dht()


# ══════════════════════════════════════
#  HILO DE LECTURA
# ══════════════════════════════════════

def _broadcast_sse(data: dict):
    msg = f"data: {json.dumps(data)}\n\n"
    with _sse_lock:
        dead = []
        for q in _sse_queues:
            try:
                q.put_nowait(msg)
            except queue.Full:
                dead.append(q)
        for q in dead:
            _sse_queues.remove(q)


def loop_sensor():
    print(f"    Hilo de sensor iniciado ({TIPO_SENSOR}, GPIO{GPIO_PIN})")
    ciclo = 0
    while True:
        ciclo += 1
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        temp_c, humedad = leer_sensor()

        if temp_c is None:
            print(f"  !!!  [{timestamp}] Sin lectura, reintentando en 10s")
            time.sleep(10)
            continue

        print(f"  (:  [{timestamp}] Ciclo #{ciclo}  {temp_c}°C  H:{humedad}%")
        guardar_lectura(timestamp, temp_c, humedad)

        # Empujar al browser vía SSE
        _broadcast_sse({
            "timestamp": timestamp,
            "temp_c": temp_c,
            "humedad": humedad,
        })

        time.sleep(INTERVALO_SEG)


# ══════════════════════════════════════
#  RUTAS FLASK
# ══════════════════════════════════════

@app.route("/")
def index():
    return render_template("index.html")


@app.route("/api/ultima")
def api_ultima():
    data = obtener_ultima()
    if data:
        return jsonify({"ok": True, "data": data})
    return jsonify({"ok": False, "data": None}), 404


@app.route("/api/historia")
def api_historia():
    rows = obtener_ultima_hora()
    if not rows:
        return jsonify({"ok": True, "data": [], "stats": {}})

    valores = [r["temp_c"] for r in rows]
    stats = {
        "promedio": round(sum(valores) / len(valores), 2),
        "maxima":   round(max(valores), 2),
        "minima":   round(min(valores), 2),
        "count":    len(valores),
    }
    return jsonify({"ok": True, "data": rows, "stats": stats})


@app.route("/api/eventos")
def api_eventos():
    """Server-Sent Events — push en tiempo real al browser."""
    q: queue.Queue = queue.Queue(maxsize=10)
    with _sse_lock:
        _sse_queues.append(q)

    # Enviar ultimo valor inmediatamente al conectar
    ultima = obtener_ultima()
    initial = f"data: {json.dumps(ultima or {})}\n\n" if ultima else ""

    @stream_with_context
    def generate():
        if initial:
            yield initial
        while True:
            try:
                msg = q.get(timeout=30)
                yield msg
            except queue.Empty:
                yield ": ping\n\n"   # heartbeat para mantener conexión
            except GeneratorExit:
                break
        with _sse_lock:
            try:
                _sse_queues.remove(q)
            except ValueError:
                pass

    return Response(
        generate(),
        mimetype="text/event-stream",
        headers={
            "Cache-Control":    "no-cache",
            "X-Accel-Buffering": "no",
        },
    )


# ══════════════════════════════════════
#  MAIN
# ══════════════════════════════════════

if __name__ == "__main__":
    print("=" * 58)
    print("  🌡️  Servidor de Temperatura — Raspberry Pi")
    print(f"  Sensor  : {TIPO_SENSOR}   Pin GPIO: {GPIO_PIN}")
    print(f"  Puerto  : {FLASK_PORT}    BD: {SQLITE_ARCHIVO}")
    print("=" * 58)

    init_db()

    # Iniciar hilo de lectura de sensor (daemon → muere con el proceso)
    t = threading.Thread(target=loop_sensor, daemon=True)
    t.start()

    # Iniciar servidor Flask
    app.run(host=FLASK_HOST, port=FLASK_PORT, threaded=True, debug=False)
