#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/config.h>
#include <conexion.h>
#include <protocolo.h>
#include <pcb.h>
#include <instrucciones.h>
#include "./planificadores/estados.h"
#include "utils.h"

int main() {
	t_config* config = config_create("kernel.config");

//	Conexiones como cliente
	int conexion_memoria = levantar_conexion_memoria(config);
	int cpu_dispatch = levantar_conexion_dispacher(config);
	int cpu_interrupt = levantar_conexion_interrupt(config);

//	Servidor para la consola
	char* puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
	int socket_serv = iniciar_servidor("127.0.0.1", puerto_escucha);
	int cliente = esperar_cliente(socket_serv);

	uint32_t* tamano_proceso = malloc(sizeof(uint32_t));
	t_list* instrucciones = deserializar_mensaje(socket_serv, cliente,
			tamano_proceso);
	printf("\n\nEL TAMAÑO DEL PROCESO ES: %d", *tamano_proceso);
	printf("\n");

// crear PCB
	Pcb* pcb = crear_pcb(instrucciones, *tamano_proceso, config, socket_serv);
	pcb_mostrar(pcb);

//	serializar PCB

	uint32_t* tamanio_mensaje = malloc(sizeof(uint32_t));
	void* a_enviar = pcb_serializar(pcb,tamanio_mensaje,0);

//	Enviar PCB
	send(cpu_dispatch, a_enviar, *tamanio_mensaje, 0);

//	RECIBO PCB CPU
//	//recibimos el codigo del tipo de mensaje que nos llega
//	recv(cpu_dispatch, &(paquete->codigo_operacion), sizeof(uint8_t), 0);
//
//	//recibo el tamaño del paquete
//	recv(cpu_dispatch, &(buffer->size), sizeof(uint32_t), 0);
//
//	//recibo el buffer con el pcb
//	buffer->stream = malloc(buffer->size);
//	recv(cpu_dispatch, buffer->stream, buffer->size, 0);
//
//	pcb = pcb_deserializar(buffer);
//	pcb_mostrar(pcb);


	avisar_proceso_finalizado(cliente);

//	Cerrar conexiones
	close(socket_serv);
	close(cpu_dispatch);
	close(cpu_interrupt);
	close(conexion_memoria);

	return 0;
}
