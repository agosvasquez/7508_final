#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H

#include <kern/pci.h>


/*------------*/
/* Constantes */
/*------------*/

/* Registers Set */
/* (e1000_hw.h) */
#define E1000_STATUS   		0x00008		/* Device Status - RO */
#define E1000_TCTL     		0x00400  	/* TX Control - RW */
#define E1000_TIPG     		0x00410  	/* TX Inter-packet gap -RW */
#define E1000_TDBAL    		0x03800  	/* TX Descriptor Base Address Low - RW */
#define E1000_TDBAH    		0x03804  	/* TX Descriptor Base Address High - RW */
#define E1000_TDLEN    		0x03808  	/* TX Descriptor Length - RW */
#define E1000_TDH      		0x03810  	/* TX Descriptor Head - RW */
#define E1000_TDT      		0x03818  	/* TX Descripotr Tail - RW */

/* Transmit Descriptor bit definitions */
/* (e1000_hw.h) */
#define E1000_TXD_CMD_EOP    0x01000000 /* End of Packet */
#define E1000_TXD_CMD_RS     0x08000000 /* Report Status */
#define E1000_TXD_CMD_DEXT	0x20000000	/* Descriptor extension (0 = legacy) */
#define E1000_TXD_STAT_DD   0x00000001 	/* Descriptor Done */

/* Transmit Control */
/* (e1000_hw.h) */
#define E1000_TCTL_RST    	0x00000001	/* software reset */
#define E1000_TCTL_EN     	0x00000002	/* enable tx */
#define E1000_TCTL_PSP    	0x00000008	/* pad short packets */
/* (Seccion 13.4.33 - Pag. 311) */
#define E1000_TCTL_CT     	4    		/* collision threshold */
#define E1000_TCTL_COLD   	12    		/* collision distance */

/* Variables globales */
#define TX_MAX_DESC			56			/* Cantidad maxima de transmit descriptors en el tx_ring */
#define MAX_PACKET_SIZE		1518		/* Tamaño maximo en bytes un Ethernet packet */     


/*-------------*/
/* Estructuras */
/*-------------*/

/* Transmit Descriptor - Legacy Mode */
/* (Seccion 3.3.3 - Pag. 36) */
struct tx_desc {
    uint64_t buffer_addr;	/* Address of the descriptor's data buffer */
    uint16_t length;    	/* Data buffer length */
    uint8_t cso;        	/* Checksum offset */
    uint8_t cmd;        	/* Descriptor control */
    uint8_t status;     	/* Descriptor status */
    uint8_t css;        	/* Checksum start */
    uint16_t special;
};

/* Receive Descriptor */
/* e1000_hw.h */
struct rx_desc {
    uint64_t buffer_addr; 	/* Address of the descriptor's data buffer */
    uint16_t length;     	/* Length of data DMAed into data buffer */
    uint16_t csum;       	/* Packet checksum */
    uint8_t status;      	/* Descriptor status */
    uint8_t errors;      	/* Descriptor Errors */
    uint16_t special;
};


/*-----------*/
/* Funciones */
/*-----------*/

int e1000_attach(struct pci_func *pcif);
int e1000_send_packet(const void *buffer, size_t len);
int e1000_receive_packet(void *buffer, size_t bufsize);

#endif  // JOS_KERN_E1000_H
