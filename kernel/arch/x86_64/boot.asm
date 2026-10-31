; ChiquiOS - BIOS x86_64 Bootloader
;
; ESTADO: EN DESARROLLO
;
; Este archivo define los requisitos del cargador.
; Todavía no es un sector de arranque funcional.
;
; Secuencia necesaria:
;
; 1. BIOS inicia en modo real de 16 bits.
; 2. Cargar el cargador de segunda etapa.
; 3. Consultar el mapa de memoria mediante E820.
; 4. Preparar GDT.
; 5. Entrar en modo protegido de 32 bits.
; 6. Habilitar PAE.
; 7. Preparar tablas de páginas.
; 8. Activar EFER.LME.
; 9. Activar paginación.
; 10. Saltar a modo long de 64 bits.
; 11. Preparar una pila alineada.
; 12. Entregar boot_info en RDI.
; 13. Saltar a _start.
;
; IMPORTANTE:
; Este archivo no debe conectarse al Makefile
; hasta implementar y probar toda la secuencia.