/*
 * CPU.h
 *
 *  Created on: 20 jun. 2022
 *      Author: utnso
 */

#ifndef CPU_H_
#define CPU_H_


/*
 * Listado de librerias incluidas
 */

	#include <stdio.h>
	#include <stdarg.h>
	#include <string.h>
	#include <stdlib.h>
	#include <commons/log.h>
	#include <commons/config.h>
	#include <commons/collections/list.h>
	#include <commons/string.h>
	#include <logs.h>
	#include <conexion.h>
	#include <pthread.h>
	#include "utils.h"

/*
 * Listado de estructuras
 */

	/*
	 * Archivo de configuracion
	 *
	 */


//	typedef struct
//	{
//
//		int entradasTLB;
//		char* algoritmoReemplazoTLB;
//		int retardoNoOp;
//		char* IPMemoria;
//		char* IPCPU;
//		char* puertoMemoria;
//		char* puertoDispatch;
//		char* puertoInterrupt;
//	}t_config_cpu;

/*
 * Listado de variables globales
*/

	t_config* config;
	t_config_cpu* configuracion;

	int socket_dispatch = 0;
	int socket_interrupt = 0;

	int fdmax = -1;
	int fdmin = 201669;
	int cantidad_clientes_dispatch = 0;
	int cantidad_clientes_interrupt = 0;
	int cliente_dispatch;
	int cliente_interrupt;
	bool interrupcion = false;

	pthread_attr_t attr;
	pthread_t threadId;
	int resThread;

	int idAnteriorPCB = -1;

/*
 * Prototipo de funciones
*/
	t_config_cpu* crearConfigCPU();
	t_config_cpu* cargarConfiguracion(char* configPath);
	void aceptoServerInterrupt(int socketAnalizar);
	void aceptoServerDispatch(int socketAnalizar);
	void * atencionInterrupt(void * socketInterrupt);
	void compararLimitesConNuevoDescriptor(int nuevoSocket);
	int levantarServerDispatch(void);
	int levantarServerInterrupt(void);
	void procesarPCB(void);
	void reciboPCBdesdeKernel(int acceptedConecctionDispatch);

#endif /* CPU_H_ */
