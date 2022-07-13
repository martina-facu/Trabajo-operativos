#include "mensajes.h"

//-----------------------------MENSAJES-----------------------------

//CPU
void entradas_y_tamanio_de_pagina(int socket_cliente){

	retardo_memoria();

	char* tamanio_pagina = malloc(config->page_size);
	char* entradas = malloc(config->table_input);

	send(socket_cliente, entradas, sizeof(config->table_input),0);
	send(socket_cliente, tamanio_pagina, sizeof(config->page_size),0);

	free(tamanio_pagina);
	free(entradas);
}

void devolver_numero_tabla_segundo_nivel(int socket_cliente){


	int cantidadDeEntradas = config->table_input;
	uint32_t numeroEntradaSegundoNivel;

	uint32_t *nroEntradaPrimerN = (uint32_t*)calloc(cantidadDeEntradas, sizeof(uint32_t));
	Coordenada_tabla* coordenada= malloc(sizeof(Coordenada_tabla));

	retardo_memoria();

	coordenada = recibir_coordenada(socket_cliente);

	nroEntradaPrimerN = list_get(tabla_paginas_primer_nivel_global, coordenada->id_tabla);

	numeroEntradaSegundoNivel = nroEntradaPrimerN[coordenada->numero_entrada];

	enviar_coordenada(coordenada, &numeroEntradaSegundoNivel , socket_cliente, ID_TABLA_SEGUNDO_NIVEL);

}


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

int obtener_y_ocupar_frame(){

	int i = 0;
	int frameEncontrado = 0;

	int cantMarcosPpal = config->memory_size / config->page_size;

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

	t_tabla_paginas_segundo_nivel* nroTablaSegundoNivel = malloc(sizeof(t_tabla_paginas_segundo_nivel));;
	uint32_t* numeroDeMarco = malloc(sizeof(uint32_t));

	Coordenada_tabla* coordenada = malloc(sizeof(Coordenada_tabla));

	retardo_memoria();

	coordenada = recibir_coordenada(socket_cliente);

	nroTablaSegundoNivel = list_get(tabla_paginas_segundo_nivel_global, coordenada->numero_entrada);

	if(nroTablaSegundoNivel->bPres == 1)

		numeroDeMarco = &nroTablaSegundoNivel->nroFrame;

	else
		//TODO
		numeroDeMarco = cargar_pagina_en_algun_marco(nroTablaSegundoNivel);

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

	iniciar_proceso(pcb);

	uint32_t espacio;

	//TODO ver si esta bien mandar el pcb o necesitan un pid
	log_trace(logger, "MEMORIA: Se envia PCB con el numero de tabla de pagina");
	void* a_enviar= pcb_serializar(pcb,&espacio,0);
	send(socket_cliente,a_enviar,espacio,0);
}

void iniciar_proceso(pcb_t* pcb){

	t_proceso* proceso = malloc(sizeof(t_proceso));

	t_list* tabla_paginas_primer_nivel_proceso;
	//TODO: chequear si el pid recibido es correcto
	proceso->pid = pcb->pid;
	proceso->tamanoProceso = pcb->tamano;

	log_info(logger, "MEMORIA: Se inicializa la tabla de primer nivel");
	//proceso->entrada_tabla_primer_nivel = inicializo_tabla_primer_nivel_proceso(tabla_paginas_primer_nivel_proceso, proceso);

	indice_tabla_primer_nivel++;

	log_info(logger, "MEMORIA: Se accede a swap..");
	crear_archivo_swap(proceso->pid);
	log_info(logger, "SWAP: Se vuelve a memoria..");
	retardo_memoria();

	log_info(logger, "MEMORIA: Se agrega proceso a la lista de procesos");
	list_add(procesos, proceso);

	//pcb_mostrar(pcb, logger);

}

int inicializo_tabla_primer_nivel_proceso(t_list* tabla_paginas_primer_nivel_proceso, t_proceso* proceso){

	int cantidadDeEntradas = config->table_input;

	//TODO: asignar n tablas de segundo nivel segun tamanio proceso
	int cantidadDePaginasDelProceso = proceso->tamanoProceso / config->page_size;
	proceso->paginasDelProceso = list_create();
	uint32_t *vector_primer_nivel = (uint32_t*)calloc(cantidadDeEntradas, sizeof(uint32_t));

	for(int i = 0; i < cantidadDeEntradas; i++){

		vector_primer_nivel[i] = inicializo_tabla_segundo_nivel_proceso(proceso);
		indice_tabla_segundo_nivel++;
	}

	list_add(tabla_paginas_primer_nivel_global, vector_primer_nivel);

	return indice_tabla_primer_nivel;

/*
	for(int i = 0; i < config->table_input; i++){

		inicializo_tabla_segundo_nivel_proceso();
		indice_tabla_segundo_nivel++;

		list_add(proceso->tabla_paginas_primer_nivel, &indice_tabla_segundo_nivel);
		//indice_tabla_primer_nivel_proceso++;
	}
*/
}



int inicializo_tabla_segundo_nivel_proceso(t_proceso* proceso){

	t_tabla_paginas_segundo_nivel* unaTabla = malloc(sizeof(t_tabla_paginas_segundo_nivel));

	//Algoritmos, ver si funca TODO
	ultimaSacada_t* entrada = malloc(sizeof(ultimaSacada_t));

	for (int j = 0; j < config->table_input; j++){
	unaTabla->bMod = 0;
	unaTabla->bPres = 0;
	unaTabla->bUso = 0;
	unaTabla->nroFrame = -1;

	entrada->pid=proceso->pid;
	entrada->posicion=0;
	list_add(listaUltimaSacada,entrada);

	list_add(tabla_paginas_segundo_nivel_global, unaTabla);
	list_add(proceso->paginasDelProceso, unaTabla);
	}

	return indice_tabla_segundo_nivel;

}


void finalizar_proceso(int socket_cliente){
	//TODO cambiar a pid
	pcb_t* pcb = recibir_paquete_pcb_kernel();

	//inicio todas las funciones de finalización
	iniciar_eliminacion_proceso(pcb->pid);

	char* c1 = "Finalizó el proceso ";
	char* contenido = strcpy(c1, pcb->pid);

	send(socket_cliente, contenido, sizeof(contenido), 0);

	//free(serialPid);

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

void iniciar_eliminacion_proceso(int pid){

	t_proceso* proceso = malloc(sizeof(t_proceso));

	int b = 0, i = 0;

	while(b == 0){


		proceso = list_get(procesos, i);
		i++;

		if (proceso->pid == pid){

			//REVISAR ESTA FUNCION TODO
			limpiar_posiciones(marcosOcupadosPpal, proceso);
			log_info(logger, "MEMORIA: Se liberan los marcos ocupados del proceso");

			list_remove_and_destroy_element(procesos, i, free);
			log_info(logger, "MEMORIA: Se elimina el proceso %d", pid);

			eliminar_archivo_swap(pid);
			log_info(logger, "MEMORIA: Se elimina el archivo del proceso %d", pid);
			b = 1;
		}
	}

}


void liberar_memoria_suspension(t_proceso* proceso){

	t_tabla_paginas_segundo_nivel* tabla_segundo_nivel;

	//Le cambio el bit de prescencia pq lo paso a swap
	//tabla->tabla_segundo_nivel->bPres = 0;
	tabla_segundo_nivel->bPres = 0;

	//Liberar marcos de memoria principal
	limpiar_posiciones(marcosOcupadosPpal, proceso);

	//Liberar paginas del proceso?
	//liberar_paginas_proceso(proceso);

}

void suspender_proceso(int socket_cliente){

	int pidRecibido; //ver que pasarle aca

	t_proceso* proceso = malloc(sizeof(t_proceso));

	t_tabla_paginas_segundo_nivel* tabla_segundo_nivel;

	int cantidadDeProcesos = list_size(procesos);

		for (int i = 0; i < cantidadDeProcesos; i++){

			proceso = list_get(procesos, i);

			if (proceso->pid == pidRecibido){

				//Si el bit de modificado está en 1, es pq tengo que guardar algo en swap
				//indice = buscar_indice_segundo_nivel(proceso->indice_tabla_primer_nivel);
				//tabla = list_get(tabla_segundo_nivel, indice);
				//if (tabla->tabla->segundo_nivel->bMod == 1)..
				if (tabla_segundo_nivel->bMod == 1){

					//falta guardar el numero de pag y contenido
					guardar_archivo_en_swap(proceso->pid, proceso->tamanoProceso);
					log_info(logger, "MEMORIA-KERNEL:Archivo del proceso %d guardado en swap", proceso->pid);
					//tabla->tabla_segundo_nivel->bMod = 0;
					tabla_segundo_nivel->bMod = 0;

					//agregar el indice de la tabla de segundo nivel
					liberar_memoria_suspension(proceso);
					log_info(logger, "MEMORIA-KERNEL: Se libera memoria del proceso %d en memoria principal", proceso->pid);
				}
			}
		}
}




//-----------------------------PAGINACION-----------------------------



void mostrar_tabla_primer_nivel(t_list* lista){

	printf("\nNro de tabla de primer nivel\n");

	for (int i = 0; i < list_size(lista); i++){

		//t_tabla_paginas_primer_nivel* entrada_tabla_primer_nivel = malloc(sizeof(t_tabla_paginas_primer_nivel));

		//entrada_tabla_primer_nivel = list_get(tabla_paginas_primer_nivel, i);

		//printf("\n|\t %d \t|", entrada_tabla_primer_nivel->nroDeTabla);
		printf("\n|\t %d \t|", i);
	}

}


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


