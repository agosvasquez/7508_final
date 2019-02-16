#include "ns.h"

extern union Nsipc nsipcbuf;

void
output(envid_t ns_envid)
{
	binaryname = "ns_output";

	// LAB 6: Your code here:
	// 	- read a packet from the network server
	//	- send the packet to the device driver
	/*
	int r;
	
	// Loop
	while (...) {
		// Los NSREQ_OUTPUT IPC's se envian con la funcion low_level_output()
		// Definida en net/lwip/jos/jif/jif.c
		// ...
		
		// Cada IPC incluye una pagina tipo union Nsipc con el packet en su struct jif_pkt 'pkt'
		// ...
		
		// Aceptar mensajes NSREQ_OUTPUT IPC del core network server
		// Enviar packets acompañando dichos mensajes al network device driver

		// Acepto un packet del network server
		// TODO: ver lib/nsipc.c
		nsipcbuf.accept ...
		struct jif_pkt packet = nsipcbuf.pkt;
		// ...

		// Envio el packet al device driver
		r = sys_network_send(nsipcbuf.pkt.jp_data, nsipcbuf.pkt.jp_len);
		
		// Contemplar el caso que la transmit queue este llena
		if (r < 0) {
			// ...
		}
	}
	*/
}
