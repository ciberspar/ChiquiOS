ChiquiOS — System Architecture

1. Objective

ChiquiOS is an independent operating system designed around a modular microkernel architecture.

The project aims to support:

* i386 (32-bit)
* x86_64 (64-bit)
* BIOS boot
* UEFI boot
* Desktop and command-line environments

These are development targets, not claims of current support.

2. Architecture

The system will be divided into independent components.

Microkernel

The microkernel will handle only essential low-level responsibilities:

* CPU and interrupt management
* Scheduling
* Memory management
* Inter-process communication (IPC)
* Process isolation

User-space servers

System services will run outside the microkernel whenever possible:

* PCI
* USB
* Storage
* Filesystems
* Networking
* Audio
* Graphics

Recovery system

The recovery system will:

* Detect service failures.
* Record diagnostic information.
* Restart recoverable services.
* Limit repeated restart attempts.
* Keep the rest of the system running when possible.

If the microkernel itself fails, the system will attempt to display an emergency diagnostic and restart.

Recovery cannot be guaranteed if essential hardware or kernel state is damaged.

3. Project structure

boot/
kernel/
  core/
  arch/
    i386/
    x86_64/
  include/
servers/
drivers/
system/
  recovery/
desktop/
bacus-pkg/
tests/

Existing files will be migrated gradually. The current build must not be broken by creating this structure.

4. Development phases

1. Document and organize the architecture.
2. Separate CPU architectures and boot methods.
3. Implement GDT, IDT and exception handling.
4. Implement physical and virtual memory management.
5. Implement processes, threads and scheduling.
6. Implement IPC.
7. Move system services outside the kernel.
8. Implement service monitoring and recovery.
9. Migrate drivers into isolated services.
10. Add automated tests.
11. Integrate Bacus-Pkg and the desktop environment.

5. Current implementation status

The existing codebase is an early i386 kernel.

The following are not yet implemented as complete microkernel features:

* Process isolation
* IPC
* Independent user-space drivers
* Real service restart
* x86_64 support
* UEFI boot

6. Development rule

All development must preserve the existing working system whenever possible.

Changes must be tested incrementally on the pruebas branch before being considered stable.