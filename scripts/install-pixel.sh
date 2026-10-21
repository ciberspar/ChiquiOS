#!/bin/sh
# Configura el entorno gráfico PIXEL sobre Debian Bookworm
export DEBIAN_FRONTEND=noninteractive

echo "🍓 Instalando entorno PIXEL / LXDE..."
apt-get update
apt-get install -y --no-install-recommends \
    xorg \
    lxde-core \
    pcmanfm \
    lxpanel \
    openbox \
    lightdm \
    netsurf-gtk \
    dialog \
    whiptail \
    sudo \
    curl \
    ca-certificates

echo "✅ Entorno PIXEL configurado exitosamente."
