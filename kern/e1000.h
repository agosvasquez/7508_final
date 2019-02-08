#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H

#include <kern/pci.h>


/*------------*/
/* Constantes */
/*------------*/

#define E1000_STATUS   		0x00008		/* Device Status - RO */
#define E1000_TXD_CMD_DEXT	0x20000000	/* Descriptor extension (0 = legacy) */
#define NTXDESC				56			/* Cantidad maxima de transmit descriptors en el array */

/*-------------*/
/* Estructuras */
/*-------------*/

// Transmit Descriptor - Legacy Mode
// Seccion 3.3.3 - Pag. 36
// e1000_hw.h
struct e1000_tx_desc {
    uint64_t buffer_addr;       /* Address of the descriptor's data buffer */
    union {
        uint32_t data;
        struct {
            uint16_t length;    /* Data buffer length */
            uint8_t cso;        /* Checksum offset */
            uint8_t cmd;        /* Descriptor control */
        } flags;
    } lower;
    union {
        uint32_t data;
        struct {
            uint8_t status;     /* Descriptor status */
            uint8_t css;        /* Checksum start */
            uint16_t special;
        } fields;
    } upper;
};

// Receive Descriptor
// ...
// e1000_hw.h
struct e1000_rx_desc {
    uint64_t buffer_addr; /* Address of the descriptor's data buffer */
    uint16_t length;     /* Length of data DMAed into data buffer */
    uint16_t csum;       /* Packet checksum */
    uint8_t status;      /* Descriptor status */
    uint8_t errors;      /* Descriptor Errors */
    uint16_t special;
};


/*-----------*/
/* Funciones */
/*-----------*/
int e1000_attach(struct pci_func *pcif);

#endif  // JOS_KERN_E1000_H
