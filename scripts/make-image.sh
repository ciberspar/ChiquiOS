
#!/usr/bin/env bash
set -Eeuo pipefail

ARCH="${1:-amd64}"
EDITION="${2:-desktop}"
ROOTFS="$PWD/work/$ARCH-$EDITION/rootfs"
ISO_ROOT="$PWD/work/$ARCH-$EDITION/iso"
OUTPUT="$PWD/output/chiquiOS-$ARCH-$EDITION.iso"

case "$ARCH" in
  i386|amd64) ;;
  arm64)
    echo "ARM64 requiere un generador EFI ARM64 específico."
    echo "No se generará una ISO x86 para ARM64."
    exit 2
    ;;
  *)
    echo "Uso: $0 {i386|amd64} {cli|desktop}"
    exit 2
    ;;
esac

if [[ ! -d "$ROOTFS" ]]; then
  echo "No existe el rootfs: $ROOTFS"
  exit 1
fi

for cmd in mksquashfs grub-mkrescue xorriso; do
  command -v "$cmd" >/dev/null || {
    echo "Falta instalar: $cmd"
    exit 1
  }
done

KERNEL="$(find "$ROOTFS/boot" -maxdepth 1 -type f -name 'vmlinuz-*' | sort -V | tail -n 1)"
INITRD="$(find "$ROOTFS/boot" -maxdepth 1 -type f -name 'initrd.img-*' | sort -V | tail -n 1)"

if [[ -z "$KERNEL" || -z "$INITRD" ]]; then
  echo "No se encuentra el kernel o initramfs en $ROOTFS/boot."
  exit 1
fi

rm -rf "$ISO_ROOT"
mkdir -p "$ISO_ROOT/live" "$ISO_ROOT/boot/grub" output

mksquashfs "$ROOTFS" "$ISO_ROOT/live/filesystem.squashfs" \
  -comp xz -b 1M -noappend

cp "$KERNEL" "$ISO_ROOT/live/vmlinuz"
cp "$INITRD" "$ISO_ROOT/live/initrd.img"

cat > "$ISO_ROOT/boot/grub/grub.cfg" <<'EOF'
set timeout=5
set default=0

menuentry "ChiquiOS Live" {
    search --no-floppy --file --set=root /live/filesystem.squashfs
    linux /live/vmlinuz boot=live components quiet
    initrd /live/initrd.img
}

menuentry "ChiquiOS Live (modo seguro)" {
    search --no-floppy --file --set=root /live/filesystem.squashfs
    linux /live/vmlinuz boot=live components nomodeset
    initrd /live/initrd.img
}
EOF

grub-mkrescue -o "$OUTPUT" "$ISO_ROOT"

echo "ISO generada: $OUTPUT"
file "$OUTPUT"
