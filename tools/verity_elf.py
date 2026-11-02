
#!/usr/bin/env python3

import pathlib
import struct
import subprocess
import sys

EXPECTED = {
    "i386": {
        "class": 1,
        "machine": 3,
    },
    "x86_64": {
        "class": 2,
        "machine": 62,
    },
}

def fail(message):
    print(f"ERROR: {message}")
    sys.exit(1)

def main():
    if len(sys.argv) != 3:
        fail("Uso: verify_elf.py ARCHIVO i386|x86_64")

    path = pathlib.Path(sys.argv[1])
    architecture = sys.argv[2]

    if architecture not in EXPECTED:
        fail("Arquitectura desconocida")

    if not path.is_file():
        fail(f"No existe {path}")

    data = path.read_bytes()

    if len(data) < 64 or data[:4] != b"\x7fELF":
        fail("Archivo ELF inválido")

    elf_class = data[4]
    endian = data[5]

    if endian not in (1, 2):
        fail("Endian inválido")

    byte_order = "<" if endian == 1 else ">"

    machine = struct.unpack_from(
        byte_order + "H", data, 18
    )[0]

    expected = EXPECTED[architecture]

    if elf_class != expected["class"]:
        fail("Clase ELF incorrecta")

    if machine != expected["machine"]:
        fail("Arquitectura ELF incorrecta")

    elf_type = struct.unpack_from(
        byte_order + "H", data, 16
    )[0]

    if elf_type != 2:
        fail("El ELF no es ejecutable")

    entry_format = "Q" if elf_class == 2 else "I"

    entry = struct.unpack_from(
        byte_order + entry_format, data, 24
    )[0]

    if entry == 0:
        fail("Dirección de entrada inválida")

    result = subprocess.run(
        ["nm", "-g", str(path)],
        capture_output=True,
        text=True
    )

    if result.returncode != 0:
        fail("No se pudo inspeccionar la tabla de símbolos")

    for symbol in (
        "_start",
        "kernel_main",
        "chiquios_cpp_module_name"
    ):
        if symbol not in result.stdout:
            fail(f"Falta el símbolo: {symbol}")

    print(f"OK: {path}")
    print(f"Arquitectura: {architecture}")
    print(f"Tamaño: {len(data)} bytes")
    print(f"Entry point: 0x{entry:X}")
    print("Símbolos esenciales: OK")

if __name__ == "__main__":
    main()
