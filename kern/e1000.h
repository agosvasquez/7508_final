#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H

#include <kern/pci.h>


/*------------*/
/* Constantes */
/*------------*/

/* E1000 MAC Address */
/* (Lab 6 - Ej. 10) */
#define MAC_ADDRESS         52:54:00:12:34:56

/* Device Status - RO */
/* (e1000_hw.h) */
#define E1000_STATUS        0x00008

/* Transmit Descriptor Registers Set */
/* (e1000_hw.h) */
#define E1000_TCTL     		0x00400  	/* TX Control - RW */
#define E1000_TIPG     		0x00410  	/* TX Inter-packet gap -RW */
#define E1000_TDBAL    		0x03800  	/* TX Descriptor Base Address Low - RW */
#define E1000_TDBAH    		0x03804  	/* TX Descriptor Base Address High - RW */
#define E1000_TDLEN    		0x03808  	/* TX Descriptor Length - RW */
#define E1000_TDH      		0x03810  	/* TX Descriptor Head - RW */
#define E1000_TDT      		0x03818  	/* TX Descripotr Tail - RW */

/* Receive Descriptor Registers Set */
/* (e1000_hw.h) */
#define E1000_RCTL          0x00100     /* RX Control - RW */
#define E1000_RDBAL         0x02800     /* RX Descriptor Base Address Low - RW */
#define E1000_RDBAH         0x02804     /* RX Descriptor Base Address High - RW */
#define E1000_RDLEN         0x02808     /* RX Descriptor Length - RW */
#define E1000_RDH           0x02810     /* RX Descriptor Head - RW */
#define E1000_RDT           0x02818     /* RX Descriptor Tail - RW */
/* (Tabla 13.2 - Pag. 220) */
#define E1000_RAL0          0x05400     /* Receive Address Register Low 0 */
#define E1000_RAH0          0x05404     /* Receive Address Register High 0 */
/* (Tabla 13.91 - Pag. 330) */
#define E1000_ADDR_VALID    0x80000000  /* Address Valid (bit 31) para RAH */ 

/* Transmit Descriptor bit definitions */
/* (e1000_hw.h) */
#define E1000_TXD_CMD_EOP   0x01000000  /* End of Packet */
#define E1000_TXD_CMD_RS    0x08000000  /* Report Status */
#define E1000_TXD_STAT_DD   0x00000001 	/* Descriptor Done */

/* Receive Descriptor bit definitions */
/* (e1000_hw.h) */
#define E1000_RXD_STAT_DD   0x01        /* Descriptor Done */
#define E1000_RXD_STAT_EOP  0x02        /* End of Packet */

/* Transmit Control (TCTL) */
/* (e1000_hw.h) */
#define E1000_TCTL_EN     	0x00000002	/* enable tx */
#define E1000_TCTL_PSP    	0x00000008	/* pad short packets */
/* (Seccion 14.5 - Pag. 378) */
#define E1000_TCTL_CT     	0x00000010	/* collision threshold (10h) */
#define E1000_TCTL_COLD   	0x00000040  /* collision distance (40h) */

/* Receive Control (RCTL) */
/* (e1000_hw.h) */
#define E1000_RCTL_EN             0x00000002    /* enable */
#define E1000_RCTL_LPE_NO         0x00000000    /* long packet disabled */
#define E1000_RCTL_LBM_NO         0x00000000    /* no loopback mode */
#define E1000_RCTL_RDMTS_EIGTH    0x00000200    /* rx desc min threshold size */
#define E1000_RCTL_BAM            0x00008000    /* broadcast enable */
#define E1000_RCTL_SZ_2048        0x00000000    /* rx buffer size 2048 */
#define E1000_RCTL_SECRC          0x04000000    /* Strip Ethernet CRC */

/* Transmit Inter Packet Gap (TIPG) */
/* (Seccion 14.3.34 - Pag. 313) */
#define E1000_TIPG_IPGT     10
#define E1000_TIPG_IPGR1    4
#define E1000_TIPG_IPGR2    6

/* Variables globales */
#define TX_MAX_DESC			56			/* Cantidad maxima de transmit descriptors en el tx_array */
#define RX_MAX_DESC         128         /* Cantidad maxima de receive descriptors en el rx_array */
#define MAX_PACKET_SIZE		1518		/* Tamaño maximo en bytes un Ethernet packet */     
#define RECEIVE_BUF_SIZE    2048        /* Viene dado por E1000_RCTL_SZ_2048 */

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
