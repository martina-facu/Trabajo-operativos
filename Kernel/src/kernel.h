#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/log.h>
#include <pthread.h>
#include <semaphore.h>
#include<netdb.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include "pcb.h"
#include "listas.h"
#include "semaforos.h"
#include "planificador_mediano_plazo.h"
#include "planificador_largo_plazo.h"
#include "var_glob.h"
#include "sjf.h"
#include "fifo.h"
#include "conexiones.h"
#include <conexion.h>

#ifndef KERNEL_H_
#define KERNEL_H_

	typedef struct
	{

		int grado_multiprogramacion;
		double TIEMPO_BLOCK_MAX;
		char* algoritmo;
		double estimacion_inicial;
		double alpha;
		char* IP_CPU;
		char* IP_MEMORIA;
		char* PUERTO_ESCUCHA;
		char* PUERTO_MEMORIA;
		char* PUERTO_CPU_DISPATCH;
		char* PUERTO_CPU_INTERRUPT;
		char* IP_KERNEL;

	}t_config_kernel;

/*
 * Listado de variables globales
*/
	t_log* logger;
	t_config_kernel* configuracion;

/*
 * Prototipo de funciones
*/

	void inicializar_loggers();
	void establecer_configuracion();
	void inicializar_semaforos();
	void inicializar_listas();

#endif /* KERNEL_H_ */
