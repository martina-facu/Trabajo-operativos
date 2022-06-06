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
	int cpu_interrupt = levantar_conexion_interrupt (config);

//	Servidor para la consola
	char* puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
	int socket_serv = iniciar_servidor("127.0.0.1", puerto_escucha);
	int cliente = esperar_cliente(socket_serv);

	uint32_t* tamano_proceso = malloc(sizeof(uint32_t));
	t_list* instrucciones = deserializar_mensaje(socket_serv, cliente,tamano_proceso);
	printf("\n\nEL TAMAÑO DEL PROCESO ES: %d", *tamano_proceso);
	printf("\n");

// crear PCB
	Pcb* pcb = crear_pcb(instrucciones,*tamano_proceso,config,socket_serv);
	pcb_mostrar(pcb);

//	serializar PCB
	void* stream_pcb = pcb_armar_stream(pcb);
	uint32_t tamano_pcb = pcb_calcular_espacio(pcb);

	t_buffer* buffer = armar_buffer(tamano_pcb, stream_pcb);
	t_paquete* paquete = empaquetar_buffer(buffer);

	void* a_enviar = malloc(paquete->size);
	a_enviar = serializar_paquete(paquete, a_enviar);

//	Enviar PCB
	send(cpu_dispatch, a_enviar, paquete->size, 0);

	avisar_proceso_finalizado(cliente);
	close(socket_serv);
	close(cpu_dispatch);
	close(cpu_interrupt);
	close(conexion_memoria);

	return 0;
}
