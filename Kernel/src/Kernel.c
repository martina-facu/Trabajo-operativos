#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
//#include <netdb.h>
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
//	int conexion_memoria = levantar_conexion_memoria(config);
//	int cpu_dispatch = levantar_conexion_dispacher(config);
//	int cpu_interrupt = levantar_conexion_interrupt (config);

//	Servidor para la consola
	char* puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
	int socket_serv = iniciar_servidor("127.0.0.1", puerto_escucha);
	int cliente = esperar_cliente(socket_serv);

	uint32_t* tamano_proceso = malloc(sizeof(uint32_t));
	t_list* instrucciones = deserializar_mensaje(socket_serv, cliente,tamano_proceso);
	printf("\n\nEL TAMAÑO DEL PROCESO ES: %d", *tamano_proceso);
	printf("\n");

// crear PCB, serializar y enviar a CPU
	Pcb* pcb = crear_pcb(instrucciones,*tamano_proceso,config,socket_serv);
	pcb_mostrar(pcb);

	void* stream = malloc(pcb_calcular_espacio(pcb));
	pcb_serializar(pcb, stream);
	t_buffer* buffer = crear_buffer(stream, pcb_calcular_espacio(pcb));
	t_paquete* paquete = pcb_empaquetar(buffer);
	void* a_enviar = serializar_paquete(paquete,stream); //aca falla
	send(5, a_enviar, sizeof(t_paquete), 0);

	avisar_proceso_finalizado(cliente);
	close(socket_serv);
//	close(cpu_dispatch);
//	close(cpu_interrupt);
//	close(conexion_memoria);

	return 0;
}
