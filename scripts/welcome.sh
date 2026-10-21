#!/bin/sh
# ==========================================
# chiquiOS Welcome Script (v1.0)
# Basado en Debian + Bacusp
# ==========================================

clear
echo "  ______________________________________"
echo " /                                      \\"
echo "|     🍓 ¡Bienvenido a chiquiOS! 🐢     |"
echo " \\______________________________________/"
echo ""
echo "  [SO]: chiquiOS Core (Debian Bookworm)"
echo "  [Gestor]: bacusp (POSIX Package Manager)"
echo "  [Mascota oficial]: Baco 🐢"
echo ""

# Mostrar uso de memoria y espacio rápido
echo "--- Estado del Sistema ---"
free -h | awk 'NR==1 || NR==2 {print "  " $0}'
echo ""
df -h / | awk 'NR==1 || NR==2 {print "  " $0}'
echo ""
echo "Escribe 'bacusp --help' para abrir la tienda o gestionar paquetes."
echo ""
