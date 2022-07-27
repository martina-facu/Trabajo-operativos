/*
 * fifo.h
 *
 *  Created on: 24 jun. 2022
 *      Author: utnso
 */

#ifndef FIFO_H_
#define FIFO_H_

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
#include "planificador_mediano_plazo.h"
#include "var_glob.h"
#include "planificador_largo_plazo.h"

/*
 * Prototipo de funciones
*/

	void* fifo();
	void* recibir_proceso_de_cpu();
	void* bloquear_proceso(void* pcb_);
	void remover_de_lista(t_list* lista,pcb_t* pcb);
	void* enviar_a_ejecutar();
	void* agregar_a_ready();
	void mostrar_lista_ready(t_list* lista);
	bool esta_pcb(t_list* lista, pcb_t* pcb);
	void* suspencion(void* pcb_);
	void* planificador_io();
	void* io();


#endif /* FIFO_H_ */
