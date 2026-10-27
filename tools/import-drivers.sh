
#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DEST="$ROOT/kernel/drivers/drivers-reales"

mkdir -p "$DEST"

command -v git >/dev/null 2>&1 || {
    echo "Error: Git no está instalado."
    exit 1
}

clone_sparse() {
    local name="$1"
    local url="$2"
    shift 2
    local target="$DEST/$name"

    if [ -d "$target/.git" ]; then
        echo "Actualizando $name..."
        git -C "$target" pull --ff-only
    else
        echo "Descargando estructura de $name..."
        git clone --depth 1 --filter=blob:none --sparse \
            "$url" "$target"
    fi

    git -C "$target" sparse-checkout set "$@"
    echo "Listo: $name"
}

clone_sparse linux \
    https://github.com/torvalds/linux.git \
    drivers

clone_sparse freebsd \
    https://github.com/freebsd/freebsd-src.git \
    sys/dev sys/net

clone_sparse netbsd \
    https://github.com/NetBSD/src.git \
    sys/dev sys/net

clone_sparse haiku \
    https://github.com/haiku/haiku.git \
    src/add-ons/kernel/drivers

clone_sparse redox \
    https://github.com/redox-os/redox.git \
    drivers

echo
echo "Importación terminada."
echo "Ruta: kernel/drivers/drivers-reales/"
echo "Aviso: las fuentes aún no están adaptadas al kernel de ChiquiOS."
