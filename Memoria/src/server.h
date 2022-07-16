/*
 * memoria.h
 *
 *  Created on: 22 jun. 2022
 *      Author: utnso
 */

#ifndef SERVER_H_
#define SERVER_H_

#include "memoria.h"
#include "mensajes.h"

/*
DESCOMENTAR SI ROMPE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include <errno.h>
#include <conexion.h>
#include <logs.h>
#include <sys/socket.h>
#include <pcb.h>
*/


/*
 * Listado de variables globales
*/
	fd_set  read_fd_set;
	fd_set  master_fd_set;
	int memorySocketServer;
	struct timeval 	tiempoSelect;

	int fdmax = -1;
	int fdmin = 201669;
	int memorySocketServer;


	int connectionsKernel = 0;
	int connectionsCPU = 0;

/*
 * Prototipos de funciones
*/
	//SERVIDOR
	int levantar_server(char* ipServer, char* portServer, t_log* logger);
	void aceptoYEvaluoConexion(int socketAnalizar);
	int validoYAceptoConexionKernel(int temporalAcceptedConnection);
	int validoYAceptoConexionCPU(int temporalAcceptedConnection);
	int manejo_mensajes_kernel(int socket_cliente);
	int manejo_mensajes_cpu(int socket_cliente);
	pcb_t* recibir_paquete_pcb_kernel();
	t_paquete* recibir_mensaje_cpu(int socket_cliente, t_log* logger);
	void mandar_lecto_escritura(uint32_t direccion, uint32_t* valor, uint8_t operacion, int conexion);


#endif /* SERVER_H_ */
