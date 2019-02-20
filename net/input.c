#include "ns.h"

#define SPIN_TIME	50

extern union Nsipc nsipcbuf;

void
input(envid_t ns_envid)
{
	binaryname = "ns_input";

	// LAB 6: Your code here:
	// 	- read a packet from the device driver
	//	- send it to the network server
	// Hint: When you IPC a page to the network server, it will be
	// reading from it for a while, so don't immediately receive
	// another packet in to the same physical page.
	
	int i, r;
	
	// Loop infinito
	while (1) {
		// Leo un paquete del driver
		nsipcbuf.pkt.jp_data[0] = 'a';
		if ((r = sys_network_recv(nsipcbuf.pkt.jp_data, sizeof(nsipcbuf.pkt.jp_data))) < 0) {
			sys_yield();
			continue;
		}

		// Asigno a jp_len la cantidad de bytes recibidos
		nsipcbuf.pkt.jp_len = r;

		// Envio el paquete al network server
		ipc_send(ns_envid, NSREQ_INPUT, &nsipcbuf.pkt, PTE_P | PTE_W | PTE_U);

		// Spin para esperar que el network server lea correctamente el paquete actual
		// antes de leer el siguiente en la misma pagina fisica
		for (i = 0; i < SPIN_TIME; i++) {
			sys_yield();
		}
	}
}
