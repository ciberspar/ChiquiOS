
#!/bin/sh
set -eu

HOST="$1"

mkdir -p "$HOST/etc"
mkdir -p "$HOST/usr/local/bin"

cat > "$HOST/etc/hostname" <<'EOF'
chiquios
EOF

cat > "$HOST/etc/motd" <<'EOF'
Bienvenido a ChiquiOS
Protegido por Baco, la tortuga.
Gestor de paquetes: Bacus-Pkg
EOF

if [ -d "${BACUS_SOURCE:-/bacuspkg}" ]; then
    mkdir -p "$HOST/usr/local/share/bacus-pkg"
    cp -R "${BACUS_SOURCE:-/bacuspkg}/." \
        "$HOST/usr/local/share/bacus-pkg/"
fi
