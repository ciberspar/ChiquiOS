
#!/bin/sh
set -eu

# ChiquiOS - Driver source importer
# No modifica kernel/drivers: guarda las fuentes aparte.

ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
DEST="$ROOT/kernel/drivers-real"

mkdir -p "$DEST"

clone_or_update() {
    name="$1"
    url="$2"
    target="$DEST/$name"

    if [ -d "$target/.git" ]; then
        echo "Actualizando $name..."
        git -C "$target" pull --ff-only
    else
        echo "Descargando fuentes de $name..."
        git clone --depth 1 "$url" "$target"
    fi
}

command -v git >/dev/null 2>&1 || {
    echo "Error: instala Git para importar las fuentes."
    exit 1
}

clone_or_update linux \
    https://github.com/torvalds/linux.git

clone_or_update netbsd \
    https://github.com/NetBSD/src.git

clone_or_update freebsd \
    https://github.com/freebsd/freebsd-src.git

clone_or_update haiku \
    https://github.com/haiku/haiku.git

clone_or_update redox \
    https://github.com/redox-os/redox.git

echo
echo "Fuentes descargadas."
echo "Ubicación: $DEST"
echo "No se han integrado ni compilado los controladores."
