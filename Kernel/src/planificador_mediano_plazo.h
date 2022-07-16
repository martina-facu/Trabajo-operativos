/*
 * planificador_mediano_plazo.h
 *
 *  Created on: 24 jun. 2022
 *      Author: utnso
 */

#ifndef PLANIFICADOR_MEDIANO_PLAZO_H_
#define PLANIFICADOR_MEDIANO_PLAZO_H_

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
#include "pcb.h"
#include "listas.h"
#include "semaforos.h"
#include <conexion.h>
#include "var_glob.h"

/*
 * Prototipo de funciones
*/

	void* administrador_mediano_plazo();
	void* avisar_proceso_susp_ready();
	void* suspender_proceso();

#endif /* PLANIFICADOR_MEDIANO_PLAZO_H_ */
