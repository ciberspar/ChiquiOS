ChiquiOS — Diseño del Microkernel

1. Objetivo

ChiquiOS utilizará una arquitectura de microkernel real para mejorar la estabilidad, seguridad y recuperación ante fallos.

El núcleo mantendrá únicamente las funciones esenciales. Los controladores y servicios se ejecutarán de forma independiente siempre que sea técnicamente posible.

2. Arquitectura

El microkernel será responsable de:

* Gestión básica de memoria.
* Planificación de procesos e hilos.
* Comunicación entre procesos (IPC).
* Interrupciones y excepciones.
* Aislamiento de procesos.
* Operaciones esenciales del sistema.

Los servicios externos incluirán:

* PCI.
* USB.
* Almacenamiento.
* Red.
* Audio.
* Gráficos.
* Sistema de archivos.
* Administración de servicios.

3. Recuperación automática

Cuando falle un componente que no sea el microkernel:

1. Detectar el fallo.
2. Aislar el proceso afectado.
3. Registrar el error.
4. Intentar reiniciar el servicio en caliente.
5. Comprobar que funciona correctamente.
6. Si vuelve a fallar, limitar los intentos e informar al usuario.

El resto del sistema debe continuar funcionando cuando sea seguro.

4. Fallo del microkernel

Si falla el microkernel:

1. Intentar mostrar un diagnóstico de emergencia.
2. Registrar información del fallo si todavía es posible.
3. Solicitar el reinicio completo del equipo.
4. Utilizar un mecanismo independiente de recuperación cuando el hardware lo permita.

No se garantizará la recuperación si el hardware o el propio mecanismo de reinicio también fallan.

5. Supervisor de servicios

El supervisor funcionará fuera del microkernel y tendrá estas responsabilidades:

* Vigilar los servicios.
* Detectar bloqueos.
* Reiniciar servicios.
* Controlar los intentos de recuperación.
* Registrar errores.
* Informar de servicios que no pudieron recuperarse.

6. Arquitecturas

Objetivos:

* x86 de 32 bits (i386).
* x86_64.

Arranque previsto:

* BIOS.
* UEFI.

7. Organización del código

* kernel/core: funciones esenciales del microkernel.
* kernel/arch: código específico de cada arquitectura.
* servers: servicios del sistema.
* drivers: controladores independientes.
* system/recovery: supervisor y recuperación.
* tests: pruebas de estabilidad y fallos.

8. Etapas de desarrollo

1. Preparar la arquitectura.
2. Implementar interrupciones y excepciones.
3. Implementar memoria y aislamiento.
4. Implementar planificación y multitarea.
5. Implementar IPC.
6. Crear procesos independientes.
7. Implementar supervisor y watchdog.
8. Migrar controladores a servicios independientes.
9. Probar recuperación automática.
10. Integrar Bacus-Pkg y el escritorio.

9. Criterios de calidad

La recuperación en caliente solo se considerará implementada cuando las pruebas demuestren que un servicio puede fallar, reiniciarse y volver a funcionar sin detener los demás servicios.

Este documento define objetivos de arquitectura. No implica que estas funciones ya estén implementadas.