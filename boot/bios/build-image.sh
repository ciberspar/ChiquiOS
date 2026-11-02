#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
BOOT_DIR="$ROOT/boot/bios"
OUT_DIR="$ROOT/build/cbl/bios"

mkdir -p "$OUT_DIR"

echo "[CBL] Compiling Stage 1..."
nasm -f bin "$BOOT_DIR/stage1.asm" \
    -o "$OUT_DIR/stage1.bin"

echo "[CBL] Compiling Stage 2..."
nasm -f bin "$BOOT_DIR/stage2.asm" \
    -o "$OUT_DIR/stage2.bin"

SIZE=$(wc -c < "$OUT_DIR/stage2.bin")

if [ "$SIZE" -gt 4096 ]; then
    echo "[CBL] ERROR: Stage 2 exceeds 4096 bytes."
    exit 1
fi

echo "[CBL] Preparing Stage 2..."
truncate -s 4096 "$OUT_DIR/stage2.bin"

echo "[CBL] Creating boot image..."
cat "$OUT_DIR/stage1.bin" \
    "$OUT_DIR/stage2.bin" \
    > "$OUT_DIR/cbl-bios.img"

echo "[CBL] Checking image..."

if [ "$(wc -c < "$OUT_DIR/stage1.bin")" -ne 512 ]; then
    echo "[CBL] ERROR: Stage 1 must be exactly 512 bytes."
    exit 1
fi

if [ "$(wc -c < "$OUT_DIR/cbl-bios.img")" -ne 4608 ]; then
    echo "[CBL] ERROR: Invalid image size."
    exit 1
fi

echo "[CBL] BIOS image created successfully!"
echo "$OUT_DIR/cbl-bios.img"