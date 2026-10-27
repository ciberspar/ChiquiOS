
# ChiquiOS — Controladores reales

Esta carpeta contiene fuentes externas para estudiar y
adaptar controladores al kernel propio de ChiquiOS.

## Fuentes

- `linux/`: controladores del kernel Linux.
- `freebsd/`: controladores y código de red de FreeBSD.
- `netbsd/`: controladores y código de red de NetBSD.
- `haiku/`: controladores de Haiku.
- `redox/`: fuentes del proyecto Redox OS.

## Categorías futuras

Los controladores adaptados de ChiquiOS se organizarán
por categoría, por ejemplo:

- `network/`: Ethernet y Wi-Fi.
- `usb/`: controladores USB.
- `storage/`: discos y almacenamiento.
- `graphics/`: vídeo y gráficos.
- `input/`: teclado, ratón y otros dispositivos.

## Importante

Descargar código fuente no significa que el controlador
funcione en ChiquiOS.

Antes de integrarlo:

1. Identificar el hardware compatible.
2. Revisar las dependencias del controlador.
3. Verificar la licencia de cada archivo.
4. Adaptar el código a las interfaces del kernel propio.
5. Compilar y probar antes de habilitarlo.

Las fuentes originales deben mantenerse separadas
del código adaptado de ChiquiOS.
