#!/bin/sh
export DEBIAN_FRONTEND=noninteractive

echo "⚙️ Configurando usuarios y entorno chiquiOS..."

# Crear usuario por defecto 'baco' con contraseña 'baco'
useradd -m -s /bin/bash baco
echo "baco:baco" | chpasswd
usermod -aG sudo,audio,video baco

# Configurar permisos para el script de bienvenida en el bashrc
echo "/usr/local/bin/welcome.sh" >> /home/baco/.bashrc

echo "✅ Configuración de usuario completada."
