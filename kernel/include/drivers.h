
#ifndef CHIQUIOS_DRIVERS_H
#define CHIQUIOS_DRIVERS_H

/* Tipos básicos */
typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

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

/* Interfaz de teclado USB */
void keyboard_usb_initialize(void);
void keyboard_usb_set_ready(u8 ready);
void keyboard_usb_push_char(char c);
char keyboard_usb_read_char(void);

/* PCI */
void pci_scan_all(void);
void pci_scan_usb(void);

/* Subsistema USB */
void usb_initialize(void);
void usb_scan_pci(void);
void usb_list_devices(void);
void usb_print_summary(void);

/* Almacenamiento: interfaz para futuros controladores */
void storage_initialize(void);
void storage_list_devices(void);

/* Red: interfaces para futuros controladores */
void ethernet_initialize(void);
void ethernet_list_devices(void);
void wifi_initialize(void);
void wifi_list_devices(void);

/* Shell y kernel */
void shell_run(void);
void kernel_main(void);

/* internet */ 
void internet_initialize(void);
void internet_list_devices(void);

#endif
