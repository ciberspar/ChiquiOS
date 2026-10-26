
#ifndef CHIQUIOS_USB_H
#define CHIQUIOS_USB_H

#include "drivers.h"

typedef struct {
    u8 bus;
    u8 slot;
    u8 function;
    u8 class_code;
    u8 subclass;
    u8 prog_if;
    u8 connected;
} usb_controller_t;

void usb_initialize(void);
void usb_scan_pci(void);
void usb_list_devices(void);
void usb_print_summary(void);

#endif
