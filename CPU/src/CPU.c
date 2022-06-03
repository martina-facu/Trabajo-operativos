#include <stdio.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/config.h>
#include <conexion.h>
#include <pcb.h>
#include <paquete.h>

Pcb* obtener_pcb(int socket_serv, int cliente){
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->buffer = malloc(sizeof(t_buffer));
	t_buffer* buffer = paquete->buffer;

	//recibimos el codigo del tipo de mensaje que nos llega
	recv(cliente, &(paquete->codigo_operacion), sizeof(uint8_t), 0); // TODO : ver si podemos agregar alguna funcionalidad a que se envie el codigo de la operacion o sino sacarlo

	//recibo el tamaño del paquete
	recv(cliente, &(buffer->size), sizeof(uint32_t), 0);

	//recibo el buffer con las instrucciones
	buffer->stream = malloc(buffer->size);
	recv(cliente, buffer->stream, buffer->size, 0);

	Pcb* pcb = pcb_deserializar(buffer);


	return pcb;
}

int main(void) {
	t_config* config = config_create("cpu.config");

//	Dispatch
	char* puerto_dispatch = config_get_string_value(config,"PUERTO_ESCUCHA_DISPATCH");
	printf("\n puerto escucha %s",puerto_dispatch);

	int socket_dispatch = iniciar_servidor("127.0.0.1", puerto_dispatch);
	printf("\n socket %d",socket_dispatch);

	int cliente1 = esperar_cliente(socket_dispatch);
	printf("\n cliente:  %d",cliente1);

	uint8_t mensaje = 0;
	recv(cliente1, &mensaje, sizeof(uint8_t), 0);
	printf("Mensaje recibido dispatch: %d", mensaje);

	uint8_t handshake = 4;

	send(cliente1, &handshake, sizeof(uint8_t), 0);


//	Interrupt
	char* puerto_interrupt = config_get_string_value(config,"PUERTO_ESCUCHA_INTERRUPT");
	printf("\n puerto interrupt %s",puerto_dispatch);

	int socket_interrupt = iniciar_servidor("127.0.0.1", puerto_interrupt);
	int cliente2 = esperar_cliente(socket_interrupt);
	uint8_t mensaje1 = 0;

	recv(cliente2, &mensaje1, sizeof(uint8_t), 0);
	printf("\n Mensaje recibido interrupt: %d", mensaje1);
	uint8_t handshake1 = 5;
	send(cliente2, &handshake1, sizeof(uint8_t), 0);

	//Recibir pcb de kernel

	t_paquete *paquete_pcb;
	recv(cliente1, paquete_pcb, sizeof(uint8_t), 0);
	uint8_t pcb_recibido = 1;
	Pcb* pcb = obtener_pcb(socket_dispatch, cliente1);
	pcb_mostrar(pcb);
//	send(cliente1, &pcb_recibido, sizeof(uint8_t), 0);


	close(socket_dispatch);
	close(socket_interrupt);
	return EXIT_SUCCESS;
}
