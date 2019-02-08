#include <kern/e1000.h>
#include <kern/pci.h>
#include <kern/pmap.h>

// LAB 6: Your driver code here

// VA del BAR 0
volatile uint8_t *e1000_bar0;

// Array de Transmit Descriptors
struct e1000_tx_desc *tx_descriptors = NULL;


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
	uint32_t status = e1000_getreg(E1000_STATUS);
	cprintf("0x%x\n", status);

	// Inicializo la transmit queue
	e1000_init_transmit_queue();


	// Inicializo la receive queue
	//e1000_init_receive_queue();

	return 0;
}

void
e1000_init_transmit_queue(void) {
	// 1) 	Alocar una porción de memoria para el array de transmit descriptors y
	// 		para los buffers packets apuntados por los descriptors.
	// 		Dos alternativas:
	//			a) Alocar paginas dinamicamente
	//			b) Declarar variables globales
	//		Importante:
	//			- Los buffers deben ser contiguos en memoria fisica
	//			- Alinear la memoria a 16 bytes
	//			- El array consta de un multiplo de 8 descriptors
	//			- El array tiene como maximo 64 descriptors
	//			- Cargar address y length de los transmit descriptors

	for (i = NTXDESC; i > 0; i--) {
		// Armo la lista enlazada de envs libres de modo tal que
		// en la primera llamada a env_init --> env_free_list = envs[0]
		//envs[i - 1].env_status = ENV_FREE;
		//envs[i - 1].env_id = 0;
		//envs[i - 1].env_link = env_free_list;
		//env_free_list = &envs[i - 1];
	}
}


// Obtiene el valor del registro e1000_bar0[offset]
// con aritmetica de punteros
static uint32_t
e1000_getreg(uint32_t offset) {
  return *(volatile uint32_t *) (e1000_bar0 + offset);
}