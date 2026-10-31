ChiquiOS — Kernel Size Policy

1. No fixed kernel size

ChiquiOS must not use an arbitrary fixed maximum kernel size.

The kernel image must grow according to its actual compiled contents and required functionality.

2. Build system

The build system must:

* Generate the kernel image using its actual compiled size.
* Avoid artificial padding to a fixed size.
* Avoid fixed kernel-size checks.
* Support separate i386 and x86_64 builds.
* Report the final kernel image size.

3. Bootloader

The bootloader must:

* Read the actual kernel image size.
* Load the complete kernel image.
* Check available memory before loading.
* Avoid overwriting reserved memory.
* Detect incomplete or failed reads.

4. Memory management

The kernel must manage memory dynamically.

Memory allocation must respect the physical memory map and reserved regions.

5. Modularity

The microkernel should remain compact.

Drivers, filesystems and other independent services should run outside the microkernel whenever possible.

6. Development rule

The existing BIOS boot process must remain functional during migration.

The fixed 16 KiB loading mechanism must be replaced before claiming support for larger kernels.

This document defines the intended behavior. It does not mean variable-size loading is already implemented.