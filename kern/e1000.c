#include <kern/e1000.h>
#include <kern/pci.h>
#include <kern/pmap.h>

// LAB 6: Your driver code here

// VA del BAR 0
volatile uint8_t *e1000_bar0;

int
e1000_attach(struct pci_func *pcif) {
	// Habilito el E1000 device
	pci_func_enable(pcif);

	// Creo un mapeo virtual para el BAR 0 (Base Address Register)
	uint32_t reg_base0 = pcif->reg_base[0];
	uint32_t reg_size0 = pcif->reg_size[0];
	e1000_bar0 = mmio_map_region(reg_base0, reg_size0);

	// Compruebo que el status es el correcto (0x80080783)
	//uint32_t status = e1000_getreg(E1000_STATUS);
	//cprintf("0x%x\n", status);

	return 0;
}


static uint32_t
e1000_getreg(uint32_t offset)
{
  return *(volatile uint32_t *) (e1000_bar0 + offset);
}