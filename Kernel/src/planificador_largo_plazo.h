/*
 * planificador_largo_plazo.h
 *
 *  Created on: 24 jun. 2022
 *      Author: utnso
 */

#ifndef PLANIFICADOR_LARGO_PLAZO_H_
#define PLANIFICADOR_LARGO_PLAZO_H_

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <pcb.h>
#include "listas.h"
#include "semaforos.h"
#include "planificador_mediano_plazo.h"
#include "var_glob.h"
#include <conexion.h>
#include <instrucciones.h>

/*
 * Prototipo de funciones
*/

	void* administrador_largo_plazo();
	void* finalizar_procesos();
	void* pasar_a_ready();
	void* agregar_estado_new();


#endif /* PLANIFICADOR_LARGO_PLAZO_H_ */
