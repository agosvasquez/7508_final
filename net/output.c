#include "ns.h"

extern union Nsipc nsipcbuf;

void
output(envid_t ns_envid)
{
	binaryname = "ns_output";

	// LAB 6: Your code here:
	// 	- read a packet from the network server
	//	- send the packet to the device driver
	
	int r, type, perm;
	envid_t sender; 
	
	// Loop infinito
	while (1) {
		// Leo un paquete del network server y lo guardo en nsipcbuf.pkt
		type = ipc_recv(&sender, &nsipcbuf.pkt, &perm);

		// Compruebo que el request y el envid son los correctos
		if (type == NSREQ_OUTPUT && sender == ns_envid) {
			// Envio el paquete al driver
			if ((r = sys_network_send(nsipcbuf.pkt.jp_data, nsipcbuf.pkt.jp_len)) < 0) {
				continue;
			}
		}
	}
}
