
#ifndef CHIQUIOS_DRIVERS_H
#define CHIQUIOS_DRIVERS_H

#include "../core/types.h"
#include "../core/boot_info.h"

/* CPU */
void cpu_detect(void);

/* Pantalla VGA */
void vga_initialize(void);
void vga_clear(void);
void vga_write(const char *text);
void vga_put_char(char ch);

/* Teclado PS/2 */
void keyboard_initialize(void);
char keyboard_read_char(void);

/* Teclado USB */
void keyboard_usb_initialize(void);
void keyboard_usb_set_ready(u8 ready);
void keyboard_usb_push_char(char c);
char keyboard_usb_read_char(void);

/* PCI */
void pci_scan_all(void);
void pci_scan_usb(void);

/* USB */
void usb_initialize(void);
void usb_scan_pci(void);
void usb_list_devices(void);
void usb_print_summary(void);

/* Almacenamiento */
void storage_initialize(void);
void storage_list_devices(void);

/* Ethernet */
void ethernet_initialize(void);
void ethernet_list_devices(void);

/* Wi-Fi */
void wifi_initialize(void);
void wifi_list_devices(void);

/* Internet */
void internet_initialize(void);
void internet_list_devices(void);

/* Controlador Intel E1000 */
void e1000_probe(void);

/* Shell */
void shell_run(void);

/* Kernel */
void kernel_main(const chiquios_boot_info_t *boot_info);

#endif
