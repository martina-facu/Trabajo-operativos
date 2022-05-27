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
#include "../../Shared/conexion.c"


t_list* obtener_instrucciones_deserializadas(int socket_serv, int cliente){
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

	t_list* instrucciones = list_create();
	deserializar_instrucciones(buffer,instrucciones);

	return instrucciones;
}

void avisar_proceso_finalizado(int cliente){
	uint8_t recibido = 1;
	send(cliente,&recibido,sizeof(uint8_t),0);
}

int main(){
	t_config* config = config_create("kernel.config");

//	Conexion con memoria
	char * ip_memoria = malloc(sizeof(char)*30);
	strcpy(ip_memoria,config_get_string_value(config,"IP_MEMORIA"));
	printf("\nIp de la memoria: %s",ip_memoria);

	char* puerto_memoria= config_get_string_value(config,"PUERTO_MEMORIA");
	int conexion_memoria= crear_conexion(ip_memoria,puerto_memoria);

	uint8_t handshake_memoria = 3;
	send(conexion_memoria,&handshake_memoria,sizeof(uint8_t),0);

	uint8_t respuesta_memoria = 0;
	recv(conexion_memoria,&respuesta_memoria, sizeof(uint8_t), 0);
	printf("\nMensaje recibido de la memoria: %d", respuesta_memoria);

//	Conexion con la cpu
	char * ip_cpu = malloc(sizeof(char)*30);
	strcpy(ip_cpu,config_get_string_value(config,"IP_CPU"));
	printf("\nIp cpu: %s",ip_cpu);

//	Dispatch
	char* puerto_dispatch= config_get_string_value(config,"PUERTO_CPU_DISPATCH");
	printf("\nPuerto dispatch: %s",puerto_dispatch);

	int cpu_dispatch= crear_conexion(ip_cpu,puerto_dispatch);
	printf("\nConexion dispatch: %d",cpu_dispatch);

	uint8_t handshake = 2;

	send(cpu_dispatch,&handshake,sizeof(uint8_t),0);

	uint8_t respuesta = 0;
	recv(cpu_dispatch,&respuesta, sizeof(uint8_t), 0);
	printf("\nMensaje recibido dispatch: %d", respuesta);

//	Interrupt
	char* puerto_interrupt= config_get_string_value(config,"PUERTO_CPU_INTERRUPT");
	int cpu_interrupt= crear_conexion(ip_cpu,puerto_interrupt);
	uint8_t handshake1 = 3;
	send(cpu_interrupt,&handshake1,sizeof(uint8_t),0);

	uint8_t respuesta1 = 0;
	recv(cpu_interrupt,&respuesta1, sizeof(uint8_t), 0);
	printf("\nMensaje recibido interrupt: %d", respuesta1);

//	Servidor para la consola
	char* puerto_escucha = config_get_string_value(config,"PUERTO_ESCUCHA");

	int socket_serv = iniciar_servidor("127.0.0.1",puerto_escucha);
	int cliente = esperar_cliente(socket_serv);

	t_list* instrucciones = obtener_instrucciones_deserializadas(socket_serv,cliente);
	mostrar_instrucciones(instrucciones);

	avisar_proceso_finalizado(cliente);

	close(cpu_dispatch);
	close(cpu_interrupt);
	close(socket_serv);
	close(conexion_memoria);

	return 0;
}
