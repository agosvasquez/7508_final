:octocat: TP5: Network Driver :octocat:
========================
 
-------------
:clubs: Questions

1. ¿How did you structure your transmit implementation? In particular, what do you do if the transmit ring is full?

Mi proceso de transmisión de paquetes consta de los siguientes pasos:
- Leo un paquete del network server
- Compruebo que el request y el envid son los correctos
- Envío el paquete al driver

Si se envía un paquete al driver y la cola de transmisión está llena, se devuelve el error E_FULL_TX_QUEUE
y no se hace nada con el paquete en cuestión: simplemente se pasa al siguiente.

En particular, la función e1000_send_packet():
- Obtiene el índice en la cola de transmisión a partir del registro TDT
- Chequea el DD bit para ver si puede o no reciclar el descriptor actual y usarlo para transmitir el paquete
- Si puede reciclarlo:
	-> Setea varios flags y la longitud del paquete
	-> Copia el paquete en el siguiente buffer (equivale a agregarlo a TDT)
	-> Actualiza TDT
- Si no:
	-> Devuelve E_FULL_TX_QUEUE para informar al caller que no se pudo enviar el paquete


2. How did you structure your receive implementation? In particular, what do you do if the receive queue is empty
   and a user environment requests the next incoming packet?
   
Mi proceso de recepción de paquetes consta de los siguientes pasos:
- Leo un paquete del driver
- Me guardo la cantidad de bytes recibidos
- Envío el paquete al network server
- Spin por un tiempo para que el network server lea correctamente el paquete actual antes de leer el siguiente
  en la misma pagina fisica
  
Si se solicita un paquete al driver y la cola de recepción está vacía, se devuelve el error E_EMPTY_RX_QUEUE,
se desaloja el proceso actual en favor de otro (sys_yield) y se pasa al siguiente paquete.

En particular, la función e1000_receive_packet():
- Obtiene el índice en la cola de recepción a partir del registro RDT
- Actualiza RDT
- Chequea el DD bit para ver si puede o no reciclar el descriptor actual y usarlo para recibir el paquete
- Si puede reciclarlo:
	-> Setea varios flags y la longitud del paquete
	-> Copia el siguiente buffer en el paquete (equivale a agregarlo a RDT)
	-> Devuelve la cantidad de bytes recibidos
- Si no:
	-> Devuelve E_FULL_RX_QUEUE para informar al caller que no se recibió ningún paquete


3. What does the web page served by JOS's web server say?

La página web de JOS dice "This file came from JOS. Cheesy web page!".


4. How long approximately did it take you to do this lab?

Completar este lab me tomó aproximadamente tres (3) semanas.

