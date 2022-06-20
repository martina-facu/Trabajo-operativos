#ifndef UTILSKERNEL
#define UTILSKERNEL

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
#include <logs.h>
#include <conexion.h>
#include <protocolo.h>
#include <pcb.h>
#include <instrucciones.h>
#include "./planificadores/estados.h"

/*
 * Listado de variables globales
*/

	t_log* logger;


/*
 * Prototipo de funciones
*/

	t_list* deserializar_mensaje(int socket_serv, int cliente, uint32_t* tamano_proceso);

	void avisar_proceso_finalizado(int cliente);

	Pcb* crear_pcb(t_list* instrucciones, uint32_t tamano_proceso, t_config* config, uint32_t id_proceso);

	int levantar_conexion_memoria(t_config* config);

	int levantar_conexion_dispacher(t_config* config);

	int levantar_conexion_interrupt(t_config* config);

#endif
