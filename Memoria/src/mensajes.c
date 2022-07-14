#include "mensajes.h"

//-----------------------------MENSAJES-----------------------------

//CPU
void entradas_y_tamanio_de_pagina(int socket_cliente){

	retardo_memoria();

	char* tamanio_pagina = malloc(tamanoPagina);
	char* entradas = malloc(entradasPorTabla);

	send(socket_cliente, entradas, sizeof(entradasPorTabla),0);
	send(socket_cliente, tamanio_pagina, sizeof(tamanoPagina),0);

	free(tamanio_pagina);
	free(entradas);
}

void devolver_numero_tabla_segundo_nivel(int socket_cliente){


	uint32_t numeroEntradaSegundoNivel;

	uint32_t *nroEntradaPrimerN = (uint32_t*)calloc(entradasPorTabla, sizeof(uint32_t));
	Coordenada_tabla* coordenada= malloc(sizeof(Coordenada_tabla));

	retardo_memoria();

	coordenada = recibir_coordenada(socket_cliente);

	nroEntradaPrimerN = list_get(tabla_paginas_primer_nivel_global, coordenada->id_tabla);

	numeroEntradaSegundoNivel = nroEntradaPrimerN[coordenada->numero_entrada];

	enviar_coordenada(coordenada, &numeroEntradaSegundoNivel , socket_cliente, ID_TABLA_SEGUNDO_NIVEL);

}

/*
uint32_t cargar_pagina_en_algun_marco(t_tabla_paginas_segundo_nivel* nroTablaSegundoNivel){

	//Falta ver lo del algoritmo aca TODO

	t_proceso* proceso = malloc(sizeof(t_proceso));
	if (nroTablaSegundoNivel->bPres == 0){

		if (cantidad_de_paginas_del_proceso_en_memoria(proceso->pid) >= config->quantity_frames_process || framesLibres == 0)

			nroTablaSegundoNivel->nroFrame = aplicar_algoritmo_reemplazo(proceso->pid);
	}

	else{
		nroTablaSegundoNivel->nroFrame = obtener_y_ocupar_frame();

	}

	return nroTablaSegundoNivel->nroFrame;
}

*/

int obtener_y_ocupar_frame(){

	int i = 0;
	int frameEncontrado = 0;

	int cantMarcosPpal = tamanoMemoria / tamanoPagina;

	while (i < cantMarcosPpal && !frameEncontrado){

		if(bitarray_test_bit(marcosOcupadosPpal, i) == 0){

			//ENCUENTRO UN MARCO DISPONBILE
			bitarray_set_bit(marcosOcupadosPpal, 1); //LO SELECCIONO COMO NO DISPONBILE, VER QUE VALER PONE TIENE QUE QUEDAR 1
			frameEncontrado = 1;
			framesLibres--;
		}
		else // DE LO CONTRRAIO SIGO BUSCANDO
			i++;

	}

	return i;
}


void devolver_numero_marco_asociado(int socket_cliente){

	t_entradas_segundo_nivel* nroTablaSegundoNivel = malloc(sizeof(t_entradas_segundo_nivel));;
	uint32_t* numeroDeMarco = malloc(sizeof(uint32_t));

	Coordenada_tabla* coordenada = malloc(sizeof(Coordenada_tabla));

	retardo_memoria();

	coordenada = recibir_coordenada(socket_cliente);

	nroTablaSegundoNivel = list_get(tabla_paginas_segundo_nivel_global, coordenada->numero_entrada);

	if(nroTablaSegundoNivel->bPres == 1)

		numeroDeMarco = &nroTablaSegundoNivel->nroFrame;

	else
		//TODO
		//numeroDeMarco = cargar_pagina_en_algun_marco(nroTablaSegundoNivel);

	enviar_coordenada(coordenada, numeroDeMarco, socket_cliente, MARCO_DE_LA_ENTRADA);

}


void devolver_lectura(int socket_cliente){

	t_paquete* respuesta = recibir_mensaje_cpu(socket_cliente, logger);
	uint32_t* direccion = malloc(sizeof(uint32_t));
	uint32_t tamanioMarco;// = respuesta->buffer->stream * config->page_size;

	retardo_memoria();
	memcpy(direccion, respuesta->buffer->stream, sizeof(uint32_t));

	mandar_lecto_escritura(direccion,respuesta->buffer->stream,RESULTADO_LECTURA, socket_cliente);

}

void devolver_escritura (int socket_cliente){

	t_paquete* respuesta = recibir_mensaje_cpu(socket_cliente, logger);
	uint32_t* valorAEscribir = malloc(sizeof(uint32_t));

	retardo_memoria();

	memcpy(valorAEscribir, respuesta->buffer->stream, sizeof(uint32_t));

	//mandar_lecto_escritura(valorAEscribir,respuesta->buffer->stream,RESULTADO_LECTURA, socket_cliente);

	char* respuesta_escritura = "OK";
	send(socket_cliente, respuesta_escritura, sizeof(respuesta), 0);

}

void inicializar_proceso(int socket_cliente){

	log_info(logger, "MEMORIA: Voy a recibir pcb");
	pcb_t* pcb = recibirPCB(socket_cliente);
	log_info(logger, "MEMORIA-KERNEL: Se recibio un pcb");

	log_trace(logger, "MEMORIA: Se envia el numero de tabla de pagina");
	uint32_t mensaje = iniciar_proceso(pcb);

	send(socket_cliente, &mensaje, sizeof(uint32_t), 0);
}

uint32_t iniciar_proceso(pcb_t* pcb){

	t_proceso* proceso = malloc(sizeof(t_proceso));

	proceso->pid = pcb->pid;
	proceso->tamanoProceso = pcb->tamano;

	log_info(logger, "MEMORIA: Se inicializa la tabla de primer nivel del proceso %d", proceso->pid);
	proceso->entrada_tabla_primer_nivel = inicializo_tabla_primer_nivel_proceso(proceso->tamanoProceso);
	indice_tabla_primer_nivel++;

	log_info(logger, "MEMORIA: Se accede a swap..");
	crear_archivo_swap(proceso->pid, proceso->tamanoProceso);
	log_info(logger, "SWAP: Se vuelve a memoria..");

	retardo_memoria();

	log_info(logger, "MEMORIA: Se agrega proceso %d a la lista de procesos", proceso->pid);

	list_add(procesos, proceso);

	mostrar_lista_procesos(procesos);
	mostrar_tabla_primer_nivel_global(tabla_paginas_primer_nivel_global);
	mostrar_tabla_segundo_nivel_global(tabla_paginas_segundo_nivel_global);

	return proceso->entrada_tabla_primer_nivel;

}

uint32_t inicializo_tabla_primer_nivel_proceso(int tamanoProceso){

	//VER ALGORITMOS
	//proceso->paginasDelProceso = list_create();
	//listaUltimaSacada = list_create();

	uint32_t *vector_primer_nivel = (uint32_t*)calloc(entradasPorTabla, sizeof(uint32_t));
	memset(vector_primer_nivel, -1, entradasPorTabla*sizeof(uint32_t));

	for(int i = 0; i < cantidad_de_entrada_primer_nivel(tamanoProceso); i++){

		vector_primer_nivel[i] = inicializo_tabla_segundo_nivel_proceso();
		//printf("%d", vector_primer_nivel[i]);
		//indice_tabla_segundo_nivel++;

	}

	list_add(tabla_paginas_primer_nivel_global, vector_primer_nivel);

	return indice_tabla_primer_nivel;

}

void mostrar_vector(uint32_t* vector_primer_nivel){

	t_proceso* proceso = malloc(sizeof(t_proceso));

	proceso->entrada_tabla_primer_nivel;

	for(int i = 0; i < sizeof(vector_primer_nivel);i++){


		printf("%d\n", vector_primer_nivel[i]);

	}


}


int inicializo_tabla_segundo_nivel_proceso(){

	log_info(logger, "MEMORIA: Se reserva espacio para una tabla");
	t_tabla_paginas_segundo_nivel* unaTabla = malloc(sizeof(t_tabla_paginas_segundo_nivel));
	unaTabla->tabla = list_create();

	//Algoritmos, ver si funca TODO
	//ultimaSacada_t* entrada = malloc(sizeof(ultimaSacada_t));

	t_entradas_segundo_nivel* entrada;

	for (int j = 0; j < entradasPorTabla; j++){

		entrada = malloc(sizeof(t_entradas_segundo_nivel));

		entrada->bMod = 0;
		entrada->bPres = 0;
		entrada->bUso = 0;
		entrada->nroFrame = -1;

		list_add(unaTabla->tabla, entrada);
	}

	list_add(tabla_paginas_segundo_nivel_global, unaTabla);
	int indice = list_size(tabla_paginas_segundo_nivel_global) - 1;
	//log_info(logger, "Inicializo los valores de las entradas (para algoritmos)");
	//entrada->pid = proceso->pid;
	//entrada->posicion=0;

	//log_info(logger, "MEMORIA: se agrega la entrada a una lista");
	//list_add(listaUltimaSacada,entrada);

	//log_info(logger, "MEMORIA: agrego la entrada a la tabla de segundo nivel");

	//log_info(logger, "Agrego la tabla a una lista del proceso");
	//list_add(proceso->paginasDelProceso, unaTabla);

	return indice;

}


void finalizar_proceso(int socket_cliente){

	uint32_t pid = 0;
	recv(socket_cliente, &pid, sizeof(uint32_t), 0);
	log_info(logger, "MEMORIA-KERNEL: Se recibe un pid %d", pid);
	iniciar_eliminacion_proceso(pid);
}


//TODO
void limpiar_posiciones(t_bitarray* marcosOcupados, t_proceso* proceso){

	int i = 0;

	int posicionInicial = list_get(proceso->entrada_tabla_primer_nivel, 0);

	for (i = posicionInicial; i < posicionInicial + proceso->tamanoProceso; i++) {

		if (bitarray_test_bit(marcosOcupados, i) == 1)
			framesLibres++;
		bitarray_clean_bit(marcosOcupados, i);
	}

}

void iniciar_eliminacion_proceso(uint32_t pid){

	t_proceso* proceso;

	for(int i = 0; i < list_size(procesos); i++ ){
		proceso = malloc(sizeof(t_proceso));
		proceso = list_get(procesos, i);

		log_info(logger, "%d", proceso->pid);

		if (proceso->pid == pid){
			log_info(logger, "MEMORIA: Encontre el proceso %d", pid);
			//REVISAR ESTA FUNCION TODO
			//limpiar_posiciones(marcosOcupadosPpal, proceso);
			log_info(logger, "MEMORIA: Se liberan los marcos ocupados del proceso");
			//list_remove_and_destroy_element(procesos, i, free);
			log_info(logger, "MEMORIA: Se elimina el proceso %d", pid);

			log_info(logger, "MEMORIA: Ingresando a SWAP..");
			eliminar_archivo_swap(pid);
			log_info(logger, "SWAP: Ingresando a MEMORIA..");

			log_info(logger, "MEMORIA: Se elimina el archivo del proceso %d", pid);
		}

	}

	//free(proceso);
}


void liberar_memoria_suspension(t_proceso* proceso){

	t_tabla_paginas_segundo_nivel* tabla_segundo_nivel;

	//Le cambio el bit de prescencia pq lo paso a swap
	//tabla->tabla_segundo_nivel->bPres = 0;
	//tabla_segundo_nivel->bPres = 0;

	//Liberar marcos de memoria principal
	limpiar_posiciones(marcosOcupadosPpal, proceso);

	//Liberar paginas del proceso?
	//liberar_paginas_proceso(proceso);

}

void suspender_proceso(int socket_cliente){

	int pidRecibido; //ver que pasarle aca

	t_proceso* proceso = malloc(sizeof(t_proceso));

	t_entradas_segundo_nivel* tabla_segundo_nivel;

	int cantidadDeProcesos = list_size(procesos);

		for (int i = 0; i < cantidadDeProcesos; i++){

			//proceso = list_get(procesos, i);

			if (proceso->pid == pidRecibido){

				//Si el bit de modificado está en 1, es pq tengo que guardar algo en swap
				//indice = buscar_indice_segundo_nivel(proceso->indice_tabla_primer_nivel);
				//tabla = list_get(tabla_segundo_nivel, indice);
				//if (tabla->tabla->segundo_nivel->bMod == 1)..
				if (tabla_segundo_nivel->bMod == 1){

					//falta guardar el numero de pag y contenido
					//guardar_archivo_en_swap(proceso->pid, proceso->tamanoProceso);
					log_info(logger, "MEMORIA-KERNEL:Archivo del proceso %d guardado en swap", proceso->pid);
					//tabla->tabla_segundo_nivel->bMod = 0;
					tabla_segundo_nivel->bMod = 0;

					//agregar el indice de la tabla de segundo nivel
					//liberar_memoria_suspension(proceso);
					log_info(logger, "MEMORIA-KERNEL: Se libera memoria del proceso %d en memoria principal", proceso->pid);
				}
			}
		}
}




//-----------------------------PAGINACION-----------------------------


void liberar_memoria_paginacion(){

	//LIMPIO LA LISTA

	list_clean(tabla_paginas_primer_nivel_global);
	list_clean(tabla_paginas_segundo_nivel_global);
	list_clean(procesos);

	//LA DESTRUYO
	list_destroy(tabla_paginas_primer_nivel_global);
	list_destroy(tabla_paginas_segundo_nivel_global);
	list_destroy(procesos);
}
