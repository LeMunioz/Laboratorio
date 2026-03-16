# Raspberry Pi como Servidor Web (acceso mundial, gratis)
# ANGEL EDUARDO MUÑOZ PEREZ
# FABIAN EMMANUEL CANDIA VILLA
# CUALTOS UDG_ ING COMPUTACION 2026A
## Arquitectura del proyecto

```
[Sensor DHT22]
      │  GPIO
[Raspberry Pi 3]
      │  Flask :5000
      │
[cloudflared]  ←→  [Cloudflare Tunnel]  ←→  Internet
                          │
                   https://tu-nombre.trycloudflare.com
                          │
                    Cualquier navegador
                    en el mundo 
```

**¿Por qué Cloudflare Tunnel?**
- 100% gratis
- HTTPS automático (certificado incluido)
- No necesitas IP fija ni abrir puertos en el router
- Funciona aunque el Pi esté detrás de un NAT/CGNAT
- Un solo comando para activarlo

---

## 1. Estructura de archivos

```
/home/pi/temperatura/
  app.py               ← Servidor Flask + lector de sensor
  templates/
    index.html       ← Dashboard web (sirve Flask)
  temperaturas.db      ← Base de datos SQLite (auto-generado)
  venv/                ← Entorno virtual Python
```

---

## 2. Preparar el Pi

### 2.1 Actualizar sistema
```bash
sudo apt update && sudo apt upgrade -y
```

### 2.2 Habilitar interfaces
```bash
sudo raspi-config

sudo reboot
```

---

## 3. Instalar dependencias Python

```bash
# Crear proyecto
mkdir -p ~/temperatura/templates
cd ~/temperatura
python3 -m venv venv
source venv/bin/activate

# Dependencias
pip install flask
pip install adafruit-circuitpython-dht   # Para DHT22/DHT11
sudo apt install -y libgpiod2
```

---

## 4. Copiar los archivos



---

## 5. Probar el servidor localmente

```bash
cd ~/temperatura
source venv/bin/activate
python3 app.py
```

Deberías ver:
```
==========================================================
  🌡️  Servidor de Temperatura — Raspberry Pi
  Sensor  : DHT22   Pin GPIO: 4
  Puerto  : 5000    BD: temperaturas.db
==========================================================
  🌡️  Hilo de sensor iniciado (DHT22, GPIO4)
 * Running on http://0.0.0.0:5000
```

Abre en tu red local: **http://\<IP_DEL_PI\>:5000**

---

## 6. Exponer al internet con Cloudflare Tunnel

### 6.1 Instalar cloudflared
```bash

# OS 32-bit 
wget https://github.com/cloudflare/cloudflared/releases/latest/download/cloudflared-linux-arm.deb
sudo dpkg -i cloudflared-linux-arm.deb
```

### 6.2 Lanzar el túnel (modo rápido, sin cuenta)
```bash
# En otra terminal, con Flask ya corriendo:
cloudflared tunnel --url http://localhost:5000
```

Se deberia ver como asi
```
+--------------------------------------------------------------------------------------------+
|  Your quick Tunnel has been created! Visit it at (it may take some time to be reachable):  |
|  https://example-random-words.trycloudflare.com                                            |
+--------------------------------------------------------------------------------------------+
```

Esa URL funciona desde **cualquier dispositivo en el mundo**, con HTTPS.

> !!! La URL cambia cada vez que reinicias `cloudflared` en modo rápido.
> Para URL fija permanente, crea una cuenta gratis en cloudflare.com


### 6.3 (Opcional) URL fija con cuenta gratuita
```bash
# Autenticarse (abre el navegador)
cloudflared tunnel login

# Crear túnel con nombre
cloudflared tunnel create mi-temperatura

# Configurar
mkdir -p ~/.cloudflared
cat > ~/.cloudflared/config.yml << EOF
tunnel: mi-temperatura
credentials-file: /home/pi/.cloudflared/<ID-DEL-TUNEL>.json
ingress:
  - hostname: temperatura.tudominio.com
    service: http://localhost:5000
  - service: http_status:404
EOF

# Ejecutar
cloudflared tunnel run mi-temperatura
```

---

## 7. Ejecutar todo automáticamente al inicio

### 7.1 Servicio systemd para Flask
```bash
sudo nano /etc/systemd/system/temperatura.service
```

Contenido:
```ini
[Unit]
Description=Monitor de Temperatura Flask
After=network.target

[Service]
User=pi
WorkingDirectory=/home/pi/temperatura
ExecStart=/home/pi/temperatura/venv/bin/python3 app.py
Restart=always
RestartSec=5

[Install]
WantedBy=multi-user.target
```

```bash
sudo systemctl daemon-reload
sudo systemctl enable temperatura
sudo systemctl start temperatura
```

### 7.2 Servicio systemd para Cloudflare Tunnel
```bash
sudo cloudflared service install
sudo systemctl enable cloudflared
sudo systemctl start cloudflared
```

---

## 8. Comandos útiles

```bash
# Ver estado del servidor
sudo systemctl status temperatura

# Ver logs en vivo
journalctl -u temperatura -f

# Reiniciar
sudo systemctl restart temperatura

# Verificar que el servidor responde
curl http://localhost:5000/api/ultima

# Ver base de datos SQLite
sqlite3 ~/temperatura/temperaturas.db "SELECT * FROM lecturas ORDER BY id DESC LIMIT 10;"
```

---

## 9. Endpoints de la API

| Endpoint         | Descripción                                |
|------------------|--------------------------------------------|
| `GET /`          | Dashboard web (página HTML)                |
| `GET /api/ultima`| Última lectura en JSON                     |
| `GET /api/historia` | Lecturas de la última hora + estadísticas |
| `GET /api/eventos` | Stream SSE para actualizaciones en vivo   |

---

## 10. Resumen

| Componente     | Tecnología          | Costo |
|----------------|---------------------|-------|
| Servidor web   | Flask (Python)      | Gratis|
| Base de datos  | SQLite              | Gratis|
| Exposición web | Cloudflare Tunnel   | Gratis|
| HTTPS/SSL      | Cloudflare (auto)   | Gratis|
| Actualizaciones| Server-Sent Events  | —     |
