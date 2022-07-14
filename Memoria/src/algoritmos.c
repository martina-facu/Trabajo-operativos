#include "algoritmos.h"

//------------------------ALGORITMOS DE REEMPLAZO----------------------


int buscar_utlima_posicion_sacada(int pid){

	int i;
	int size = list_size(listaUltimaSacada);

	for(i=0; i < size; i++){

		ultimaSacada_t* entrada = list_get(listaUltimaSacada, i);

		if (entrada->pid == pid)
				return entrada->posicion;
	}

	return -1;
}


void cambiar_ultima_posicion(int pid, int ultima){

	int i;
	int size = list_size(listaUltimaSacada);


	for(i=0; i < size; i++){

		ultimaSacada_t* entrada = list_get(listaUltimaSacada, i);

		if (entrada->pid == pid)
				entrada->posicion = ultima;
		}
}

t_proceso* buscar_tabla (int pid){

	int size = list_size(procesos);
	int i;

	for (i = 0; i < size; i++){

		t_proceso* proceso_tabla;
		proceso_tabla = list_get(procesos,i);

		if(proceso_tabla->pid == pid)

			return proceso_tabla;
	}

	log_info(logger, "El proceso no está en memoria principal");
	return NULL; //No existe, avisa que hay que agregarla para reservar pagina
}



//TODO: Testear
int sacar_con_clock(int pid){

	log_info(logger, "ALGORITMO DE REEMPLAZO: CLOCK");

	int posicionAReemplazar = 0;
	t_entradas_segundo_nivel* puntero;

	t_proceso* proceso = malloc(sizeof(t_proceso));
	proceso = buscar_tabla(pid);

	int cantidadDePaginasDelProceso = list_size((t_list*)proceso->paginasDelProceso);

		for(posicionAReemplazar = buscar_utlima_posicion_sacada(pid); posicionAReemplazar - buscar_utlima_posicion_sacada(pid) < cantidadDePaginasDelProceso; posicionAReemplazar++){

		puntero = list_get((t_list*)proceso->paginasDelProceso ,posicionAReemplazar % cantidadDePaginasDelProceso);
				if (puntero->bUso == 0 && puntero->bPres == 1){

					cambiar_ultima_posicion(pid, posicionAReemplazar % cantidadDePaginasDelProceso);
//					pthread_mutex_unlock(&lock_accesoUltimaPos);
	//				pthread_mutex_unlock(&lock_accesoTabla);

					return posicionAReemplazar % cantidadDePaginasDelProceso;
				}
				else{
					puntero->bUso = 0;
				}


	//SEGUNDA VUELTA
	for(posicionAReemplazar = buscar_utlima_posicion_sacada(pid); posicionAReemplazar - buscar_utlima_posicion_sacada(pid) < cantidadDePaginasDelProceso; posicionAReemplazar++){

		puntero = list_get((t_list*)proceso->paginasDelProceso ,posicionAReemplazar % cantidadDePaginasDelProceso);

			cambiar_ultima_posicion(pid, posicionAReemplazar % cantidadDePaginasDelProceso);
			//pthread_mutex_unlock(&lock_accesoUltimaPos);
		//	pthread_mutex_unlock(&lock_accesoTabla);

			return posicionAReemplazar % cantidadDePaginasDelProceso;
		}
	}


	//pthread_mutex_unlock(&lock_accesoUltimaPos);
	//pthread_mutex_unlock(&lock_accesoTabla);

	log_error(logger, "%d Algo fallo en CLOCK",pid);
	return -1;

}

//TODO: Testear
int sacar_con_clock_modificado(int pid){

	log_info(logger, "ALGORITMO DE REEMPLAZO: CLOCK-M");

	int posicionAReemplazar = 0;
	t_entradas_segundo_nivel* puntero;
	t_proceso* proceso = malloc(sizeof(t_proceso));
	proceso = buscar_tabla(pid);

	int cantidadDePaginasDelProceso = list_size((t_list*)proceso->paginasDelProceso);


	//Primera vuelta, me fijo si hay alguno en 0 0 sin modificar nada

	for(posicionAReemplazar = buscar_utlima_posicion_sacada(pid); posicionAReemplazar - buscar_utlima_posicion_sacada(pid) < cantidadDePaginasDelProceso; posicionAReemplazar++){

		puntero = list_get((t_list*)proceso->paginasDelProceso ,posicionAReemplazar % cantidadDePaginasDelProceso);

		if (puntero->bUso == 0 && puntero->bMod == 0 && puntero->bPres == 1){

			cambiar_ultima_posicion(pid, posicionAReemplazar % cantidadDePaginasDelProceso);
			//pthread_mutex_unlock(&lock_accesoUltimaPos);
			//pthread_mutex_unlock(&lock_accesoTabla);

			return posicionAReemplazar % cantidadDePaginasDelProceso;

		}
	}

	//SEGUNDA VUELTA

	for(posicionAReemplazar = buscar_utlima_posicion_sacada(pid);posicionAReemplazar - buscar_utlima_posicion_sacada(pid)<cantidadDePaginasDelProceso;posicionAReemplazar++){

		puntero = list_get((t_list*)proceso->paginasDelProceso ,posicionAReemplazar % cantidadDePaginasDelProceso);

		if(puntero->bUso == 0 && puntero->bMod == 1 && puntero->bPres == 1){

			cambiar_ultima_posicion(pid,posicionAReemplazar%cantidadDePaginasDelProceso);
//			pthread_mutex_unlock(&lock_accesoUltimaPos);
//			pthread_mutex_unlock(&lock_accesoTabla);

			return posicionAReemplazar % cantidadDePaginasDelProceso;

		}else{

			puntero->bUso = 0;

		}
	}

	//Primer vuelta

	for(posicionAReemplazar = buscar_utlima_posicion_sacada(pid); posicionAReemplazar - buscar_utlima_posicion_sacada(pid)<cantidadDePaginasDelProceso; posicionAReemplazar++){

		puntero = list_get((t_list*)proceso->paginasDelProceso ,posicionAReemplazar % cantidadDePaginasDelProceso);

		if(puntero->bUso == 0 && puntero->bPres == 1){
			cambiar_ultima_posicion(pid,posicionAReemplazar % cantidadDePaginasDelProceso);
			//pthread_mutex_unlock(&lock_accesoUltimaPos);
			//pthread_mutex_unlock(&lock_accesoTabla);
			return posicionAReemplazar % cantidadDePaginasDelProceso;
		}else{
			puntero->bUso=0;
		}
	}

	//Segunda vuelta
	for(posicionAReemplazar=buscar_utlima_posicion_sacada(pid);posicionAReemplazar - buscar_utlima_posicion_sacada(pid)<cantidadDePaginasDelProceso;posicionAReemplazar++){
		puntero = list_get((t_list*)proceso->paginasDelProceso ,posicionAReemplazar % cantidadDePaginasDelProceso);
		if(puntero->bUso == 0 && puntero->bMod == 1 && puntero->bPres == 1){

			cambiar_ultima_posicion(pid,posicionAReemplazar % cantidadDePaginasDelProceso);
//			pthread_mutex_unlock(&lock_accesoUltimaPos);
	//		pthread_mutex_unlock(&lock_accesoTabla);

			return posicionAReemplazar % cantidadDePaginasDelProceso;
		}else{
			puntero->bUso=0;
		}
	}

	//pthread_mutex_unlock(&lock_accesoUltimaPos);
	//pthread_mutex_unlock(&lock_accesoTabla);

	log_error(logger, "%d Algo fallo en CLOCK MODIFICADO",pid);


	return -1;

}

void aplicar_algoritmo_reemplazo(t_proceso* proceso){

	if(strcmp(config->replacement_algorithm,"CLOCK")==0)
		sacar_con_clock(proceso->pid);
	else if (strcmp(config->replacement_algorithm,"CLOCK_M") == 0)
		sacar_con_clock_modificado(proceso->pid);
	else
		log_error(logger, "Error, algoritmo de reemplazo no identificado: CLOCK o CLOCK_M");
}

int cantidad_de_paginas_del_proceso_en_memoria(int pid){

	//pthread_mutex_lock(&lock_accesoTabla);
	t_proceso* tabla_proceso = buscar_tabla(pid);
	uint32_t *nroEntradaPrimerN = (uint32_t*)calloc(config->table_input, sizeof(uint32_t));

	int i;
	int contador = 0;

	//TODO ver si cambiar cantidad de entradas
	for (int j = 0; j < config->table_input;j++){

		nroEntradaPrimerN = list_get(tabla_paginas_primer_nivel_global, tabla_proceso->entrada_tabla_primer_nivel);

		for (i = 0; i < config->table_input; i++){

			t_entradas_segundo_nivel* unaPagina;
			unaPagina = list_get(tabla_paginas_segundo_nivel_global, nroEntradaPrimerN[j]);

			if(unaPagina[i].bPres == 1)
				contador++;
		}
	}
	//pthread_mutex_unlock(&lock_accesoTabla);

	return contador;
}
