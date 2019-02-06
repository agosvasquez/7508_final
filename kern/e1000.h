#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H

#include <kern/pci.h>

#define E1000_STATUS   0x00008  /* Device Status - RO */

int e1000_attach(struct pci_func *pcif);

static uint32_t e1000_getreg(uint32_t offset);

#endif  // JOS_KERN_E1000_H
