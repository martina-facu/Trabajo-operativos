/*
 * sjf.h
 *
 *  Created on: 24 jun. 2022
 *      Author: utnso
 */

#ifndef SJF_H_
#define SJF_H_


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
#include "planificador_largo_plazo.h"
#include "var_glob.h"

/*
 * Listado de variables globales
*/

	time_t tiempo_de_ejecucion_inicial;
	time_t tiempo_de_ejecucion_final;
	float tiempo_de_ejecucion;

/*
 * Prototipo de funciones
*/
	void* sjf(); //
	void* bloquear_proceso_sjf(void* pcb_); //
	void remover_de_lista_sjf(t_list* lista,pcb_t* pcb); //
//	pcb_t* recibir_paquete_pcb_sjf(); //
	void recibir_paquete_pcb_sjf(pcb_t* pcb);
	void* agregar_a_ready_sjf(); //
	bool menor_estimacion(void* pcb_aux,void* pcb_aux2); //
	void* hablar_con_cpu(); //
	void* devoluciones(); //
	void* interrupciones();
	void* io_sjf();
	void* planificador_io_sjf();
	bool esta_pcb_sjf(t_list* lista, pcb_t* pcb);
	void* suspencion_sjf(void* pcb_);
	void actualizar_estimacion(pcb_t* pcb); //
	void interrumpir(); //
	void* enviar_a_ejecutar_sjf(); //
	void mostrar_lista_ready_sjf(t_list* lista); //


#endif /* SJF_H_ */
