// LAB 6: Your driver code here

#include <inc/error.h>
#include <inc/string.h>

#include <kern/e1000.h>
#include <kern/pci.h>
#include <kern/pmap.h>

// VA del BAR 0
volatile uint8_t *e1000_bar0;

// Array de Transmit Descriptors
static struct tx_desc tx_descriptors[TX_MAX_DESC];

// Array de Receive Descriptors
static struct rx_desc rx_descriptors[RX_MAX_DESC];

// Array de Transmit Packets Buffers
static uint8_t tx_buffers[TX_MAX_DESC][TRANSMIT_BUF_SIZE];

// Array de Receive Packets Buffers
static uint8_t rx_buffers[RX_MAX_DESC][RECEIVE_BUF_SIZE];


/*--------------------*/
/* Funciones privadas */
/*--------------------*/

// Obtiene el valor del registro e1000_bar0[offset]
static uint32_t
e1000_getreg(uint32_t offset) {
  return *(volatile uint32_t *) (e1000_bar0 + offset);
}

// Escribe el valor en el registro e1000_bar0[offset]
static void
e1000_setreg(uint32_t offset, uint32_t value)
{
  *(volatile uint32_t *) (e1000_bar0 + offset) = value;
}

// Inicializa los transmit descriptors
void
tx_descriptors_init(void) {
	size_t i;
	for (i = 0; i < TX_MAX_DESC; i ++) {
		tx_descriptors[i].buffer_addr = PADDR(&tx_buffers[i]);
		tx_descriptors[i].status |= E1000_TXD_STAT_DD;
	}
}

// Inicializa los receive descriptors
void
rx_descriptors_init(void) {
	size_t i;
	for (i = 0; i < RX_MAX_DESC; i ++) {
		rx_descriptors[i].buffer_addr = PADDR(&rx_buffers[i]);
	}
}

// Inicializa los registros BAR del transmit array
void
tx_registers_init(void) {
	// Inicializo los registros Transmit Descriptor Base Address (TDBAL y TDBAH)
	uint64_t array_addr = PADDR(tx_descriptors);
	e1000_setreg(E1000_TDBAL, array_addr);
	e1000_setreg(E1000_TDBAH, 0);

	// Inicializo el registro Transmit Descriptor Length (TDLEN)
	e1000_setreg(E1000_TDLEN, sizeof(tx_descriptors));

	// Inicializo los registros Transmit Descriptor Head y Tail (TDH y TDT)
	e1000_setreg(E1000_TDH, 0);
	e1000_setreg(E1000_TDT, 0);

	// Inicializo el registro Transmit Control (TCTL)
	uint32_t tctl_flags = E1000_TCTL_EN | E1000_TCTL_PSP | E1000_TCTL_CT | E1000_TCTL_COLD;
	e1000_setreg(E1000_TCTL, tctl_flags);

	// Inicializo el registro Transmit Inter Packet Gap (TIPG)
	uint32_t tipg_flags = E1000_TIPG_IPGT | E1000_TIPG_IPGR1 | E1000_TIPG_IPGR2;
	e1000_setreg(E1000_TIPG, tipg_flags);
}

// Inicializa los registros BAR del receive array
void
rx_registers_init(void) {
	// Inicializo los registros Receive Address (RAL y RAH) apuntando a la MAC_ADDRESS
	e1000_setreg(E1000_RAL0, MAC_ADDR_LOW);
	e1000_setreg(E1000_RAH0, MAC_ADDR_HIGH | E1000_ADDR_VALID);

	// Inicializo los registros Receive Descriptor Base Address (RDBAL y RDBAH)
	uint64_t array_addr = PADDR(rx_descriptors);
	e1000_setreg(E1000_RDBAL, array_addr);
	e1000_setreg(E1000_RDBAH, 0);

	// Inicializo el registro Receive Descriptor Length (RDLEN)
	e1000_setreg(E1000_RDLEN, sizeof(rx_descriptors));

	// Inicializo los registros Receive Descriptor Head y Tail (RDH y RDT)
	// RDH apunta al primer descriptor valido (rx_descriptors[0])
	e1000_setreg(E1000_RDH, PADDR(&rx_descriptors[0]));
	// RDH apunta al ultimo descriptor valido + 1 (rx_descriptors[RX_MAX_DESC])
	e1000_setreg(E1000_RDT, PADDR(&rx_descriptors[RX_MAX_DESC]));

	// Inicializo el registro Receive Control (RCTL)
	uint32_t rctl_flags = 	E1000_RCTL_EN | E1000_RCTL_LPE_NO | E1000_RCTL_LBM_NO |
			 				E1000_RCTL_BAM | E1000_RCTL_SZ_2048 | E1000_RCTL_SECRC;
	e1000_setreg(E1000_RCTL, rctl_flags);
}

// Inicializa la cola de transmision
void
e1000_init_transmit_queue(void) {
	// Inicializo cada transmit descriptor con el array tx_buffers
	tx_descriptors_init();

	// Inicializo los registros
	tx_registers_init();
}

// Inicializa la cola de recepcion
void
e1000_init_receive_queue(void) {
	// Inicializo cada receive descriptor con el array rx_buffers
	rx_descriptors_init();

	// Inicializo los registros
	rx_registers_init();	
}

// Transmite un paquete
int
e1000_send_packet(const void *buf, size_t len) {

	int r = 0;

	// Obtengo el indice en la queue dado por el tail register
	uint32_t idx = e1000_getreg(E1000_TDT);

	// Si el DD Bit esta en 1, puedo reciclar el descriptor y usarlo para transmitir el paquete
	bool is_dd_set = (tx_descriptors[idx].status & E1000_TXD_STAT_DD);

	if (is_dd_set) {
		// Seteo el RS y el EOP bit del Command Field en 1
		// Los desplazo 24 bits ya que ahi empieza el command field del descriptor
		uint32_t cmd_flags = (E1000_TXD_CMD_RS >> 24) | (E1000_TXD_CMD_EOP >> 24);
		tx_descriptors[idx].cmd |= cmd_flags;
		
		// Seteo el DD Bit del Status en 0, para indicar que esta en uso
		tx_descriptors[idx].status &= ~E1000_TXD_STAT_DD;

		// Seteo la longitud del paquete
		tx_descriptors[idx].length = len;
		
		// Para transmitir un paquete, lo agrego al tail (TDT) de la cola de transmision
		// Esto equivale a copiar el paquete en el siguiente buffer
		memcpy(tx_buffers[idx], buf, len);
		
		// Actualizo el registro TDT
		idx = (idx + 1) % TX_MAX_DESC;
  		e1000_setreg(E1000_TDT, idx);
	} else {
		// Devuelvo un codigo de error para que el caller de esta funcion
		// sepa que el paquete no se pudo enviar
		r = -E_FULL_TX_QUEUE;
	}

	return r;
}

// Recibe un paquete
int
e1000_receive_packet(void *buf, size_t bufsize) {
	
	int r = 0;

	// Obtengo el indice en la queue dado por el tail register
	uint32_t idx = e1000_getreg(E1000_RDT);

	// Si el DD Bit esta en 1, puedo reciclar el descriptor y usarlo para recibir el paquete
	bool is_dd_set = (rx_descriptors[idx].status & E1000_RXD_STAT_DD);
	
	if (is_dd_set) {
		// Seteo el EOP bit del Status en 1
		rx_descriptors[idx].status |= E1000_RXD_STAT_EOP;

		// Seteo el DD Bit del Status en 0, para indicar que esta en uso
		rx_descriptors[idx].status &= ~E1000_RXD_STAT_DD;

		// Seteo la longitud del paquete
		rx_descriptors[idx].length = bufsize;
		
		// Para recibir un paquete, lo agrego al tail (TDT) de la cola de recepcion
		// Esto equivale a copiar el paquete en el siguiente buffer
		memcpy(rx_buffers[idx], buf, bufsize);
		
		// Actualizo el registro RDT
		idx = (idx + 1) % RX_MAX_DESC;
  		e1000_setreg(E1000_RDT, idx);
	} else {
		// Devuelvo un codigo de error para que el caller de esta funcion
		// sepa que no se recibio ningun paquete
		r = -E_EMPTY_RX_QUEUE;
	}
	
	return r;
}

/*--------------------*/
/* Funciones publicas */
/*--------------------*/

// Inicializa el E1000
int
e1000_attach(struct pci_func *pcif) {
	// Habilito el E1000 device
	pci_func_enable(pcif);

	// Creo un mapeo virtual para el BAR 0 (Base Address Register)
	uint32_t reg_base0 = pcif->reg_base[0];
	uint32_t reg_size0 = pcif->reg_size[0];
	e1000_bar0 = mmio_map_region(reg_base0, reg_size0);

	// Compruebo que el status es el correcto (0x80080783)
	/*
	uint32_t status = e1000_getreg(E1000_STATUS);
	cprintf("0x%x\n", status);
	*/

	// Inicializo la cola de transmision
	e1000_init_transmit_queue();

	// Compruebo que el paquete se transmite correctamente
	/*
	e1000_send_packet("Hola", 4);
	e1000_send_packet("Mundo", 5);
	e1000_send_packet("Como", 4);
	e1000_send_packet("Estan?", 6);
	*/

	// Inicializo la cola de recepcion
	e1000_init_receive_queue();

	// Compruebo que el paquete se recibe correctamente
	/*
	e1000_receive_packet("Hola", 4);
	e1000_receive_packet("Mundo", 5);
	e1000_receive_packet("Como", 4);
	e1000_receive_packet("Estan?", 6);
	*/
	
	return 0;
}
