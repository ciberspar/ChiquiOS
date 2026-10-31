ChiquiOS — Memory Management Design

1. Objective

Build a robust, efficient and predictable memory management system inspired by modern operating systems such as macOS.

No arbitrary fixed RAM limit will be imposed by the kernel.

2. Memory architecture

Physical Memory Manager (PMM)

* Track available and reserved physical memory.
* Allocate and release physical pages.
* Respect firmware memory maps.
* Detect invalid memory regions.

Virtual Memory Manager (VMM)

* Manage virtual address spaces.
* Isolate processes.
* Support memory protection.
* Prevent unauthorized access between processes.

Dynamic Allocator

* Allocate memory dynamically.
* Minimize fragmentation.
* Detect invalid frees and allocation errors.
* Support different allocation sizes.

Memory Monitor

* Track memory usage.
* Detect leaks where possible.
* Monitor memory pressure.
* Record allocation failures.

3. Memory protection

* Kernel memory must be protected.
* User processes must have isolated address spaces.
* Read-only memory must remain protected.
* Non-executable memory must be supported where available.

4. Memory pressure

When RAM becomes scarce:

1. Reclaim unused caches.
2. Release reclaimable resources.
3. Notify affected services.
4. Use swap only if implemented and configured.
5. Report unrecoverable allocation failures safely.

5. Architecture support

The memory manager must support:

* i386
* x86_64

Architecture-specific page tables and address-space operations must remain separate from shared memory-management logic.

6. Development status

This document describes design goals. These features must be implemented and tested before being considered available.