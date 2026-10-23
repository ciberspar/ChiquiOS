
#!/usr/bin/env bash
set -Eeuo pipefail

ARCH="${1:-amd64}"
EDITION="${2:-desktop}"
SUITE="bookworm"
MIRROR="https://deb.debian.org/debian"

case "$ARCH" in
  i386|amd64|arm64) ;;
  *) echo "Arquitectura no válida: $ARCH"; exit 2 ;;
esac

case "$EDITION" in
  cli|desktop) ;;
  *) echo "Edición no válida: $EDITION"; exit 2 ;;
esac

if [[ $EUID -ne 0 ]]; then
  echo "Ejecuta este script con sudo."
  exit 1
fi

for cmd in debootstrap chroot; do
  command -v "$cmd" >/dev/null || {
    echo "Falta instalar: $cmd"
    exit 1
  }
done

ROOTFS="$PWD/work/$ARCH-$EDITION/rootfs"
mkdir -p work output

if [[ -e "$ROOTFS" ]]; then
  echo "Ya existe $ROOTFS. Elimínalo si quieres reconstruirlo."
  exit 1
fi

mkdir -p "$ROOTFS"

cleanup() {
  for dir in run sys proc dev; do
    if mountpoint -q "$ROOTFS/$dir"; then
      umount -lf "$ROOTFS/$dir" || true
    fi
  done
}
trap cleanup EXIT

if [[ "$ARCH" == "$(dpkg --print-architecture)" ]]; then
  debootstrap --arch="$ARCH" "$SUITE" "$ROOTFS" "$MIRROR"
else
  debootstrap --foreign --arch="$ARCH" "$SUITE" "$ROOTFS" "$MIRROR"

  case "$ARCH" in
    arm64) QEMU="qemu-aarch64-static" ;;
    i386)  QEMU="qemu-i386-static" ;;
  esac

  if command -v "$QEMU" >/dev/null; then
    install -D "$(command -v "$QEMU")" "$ROOTFS/usr/bin/$QEMU"
  else
    echo "Falta $QEMU. Instala qemu-user-static."
    exit 1
  fi

  chroot "$ROOTFS" /debootstrap/debootstrap --second-stage
fi

cat > "$ROOTFS/etc/apt/sources.list" <<EOF
deb $MIRROR bookworm main contrib non-free non-free-firmware
deb $MIRROR bookworm-updates main contrib non-free non-free-firmware
deb http://security.debian.org/debian-security bookworm-security main contrib non-free non-free-firmware
EOF

cat > "$ROOTFS/etc/os-release" <<'EOF'
PRETTY_NAME="ChiquiOS Live"
NAME="ChiquiOS"
ID=chiquios
ID_LIKE=debian
VERSION_ID="12"
VERSION="12 (Bookworm)"
EOF

install -D /dev/null "$ROOTFS/etc/hostname"
echo "chiquios" > "$ROOTFS/etc/hostname"

for dir in dev proc sys run; do
  mkdir -p "$ROOTFS/$dir"
done

mount --bind /dev "$ROOTFS/dev"
mount -t proc proc "$ROOTFS/proc"
mount -t sysfs sysfs "$ROOTFS/sys"
mount --bind /run "$ROOTFS/run"

install_list() {
  local file="$1"
  [[ -f "$file" ]] || {
    echo "No existe la lista: $file"
    exit 1
  }

  grep -Ev '^[[:space:]]*(#|$)' "$file"
}

case "$ARCH" in
  amd64) KERNEL="linux-image-amd64" ;;
  i386)  KERNEL="linux-image-686-pae" ;;
  arm64) KERNEL="linux-image-arm64" ;;
esac

{
  echo "$KERNEL"
  echo "live-boot"
  echo "initramfs-tools"
  install_list config/packages/base.txt
  if [[ "$EDITION" == "desktop" ]]; then
    install_list config/packages/desktop.txt
    install_list config/packages/browsers.txt
  fi
} | sort -u > "work/$ARCH-$EDITION/packages.txt"

cp "work/$ARCH-$EDITION/packages.txt" "$ROOTFS/tmp/packages.txt"

chroot "$ROOTFS" /bin/bash -euc '
  export DEBIAN_FRONTEND=noninteractive
  apt-get update
  xargs -r apt-get install -y --no-install-recommends < /tmp/packages.txt
  apt-get clean
  rm -rf /var/lib/apt/lists/*
  rm -f /tmp/packages.txt
  update-initramfs -u -k all
'

mkdir -p output
echo "Root filesystem preparado: $ROOTFS"
