#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/config.h>
#include "../../Shared/utils/instrucciones.h"
#include "../../Shared/utils/protocolo.h"

t_list* obtener_instrucciones_deserializadas(int* socket_serv, int* cliente){
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->buffer = malloc(sizeof(t_buffer));
	t_buffer* buffer = paquete->buffer;

	t_config* config = config_create("kernel.config");
	socket_serv = iniciar_servidor("127.0.0.1",config);

	cliente = esperar_cliente(socket_serv);

	//recibimos el codigo del tipo de mensaje que nos llega
	recv(cliente, &(paquete->codigo_operacion), sizeof(uint8_t), 0); // TODO : ver si podemos agregar alguna funcionalidad a que se envie el codigo de la operacion o sino sacarlo

	//recibo el tamaño del paquete
	recv(cliente, &(buffer->size), sizeof(uint32_t), 0);

	//recibo el buffer con las instrucciones
	buffer->stream = malloc(buffer->size);
	recv(cliente, buffer->stream, buffer->size, 0);

	t_list* instrucciones = list_create();
	deserializar_instrucciones(buffer,instrucciones);

	return instrucciones;
}

//void avisar_proceso_finalizado(int* cliente){
//	uint8_t recibido = 1;
//	send(cliente,&recibido,sizeof(uint8_t),0);
//}

int main(){
	int socket_serv;
	int cliente;


	t_list* instrucciones = obtener_instrucciones_deserializadas(&socket_serv,&cliente);

	mostrar_instrucciones(instrucciones);

//	avisar_proceso_finalizado(cliente);

	uint8_t recibido = 1;
	send(&cliente,&recibido,sizeof(uint8_t),0);

	close(socket_serv);

	return 0;
}
