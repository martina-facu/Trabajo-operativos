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

	Coordenada_tabla_cpu* coordenada = malloc(sizeof(Coordenada_tabla_cpu));

	t_proceso* proceso;

	//TODO: REHACER FUNCION COORDENADA
	coordenada = recibir_coordenada(socket_cliente);

	retardo_memoria();

	proceso = list_get(procesos, coordenada->pid);

	uint32_t indice = proceso->entrada_tabla_primer_nivel;

	uint32_t* tabla_primer_nivel = list_get(tabla_paginas_primer_nivel_global, indice);

	uint32_t valor = *(tabla_primer_nivel + coordenada->indice_tabla_primer_nivel);


	enviar_coordenada(coordenada, &numeroEntradaSegundoNivel , socket_cliente, ID_TABLA_SEGUNDO_NIVEL);

	uint32_t entrada_tabla_segundo_nivel; //recibe de cpu

	//TODO:recv(entrada_tabla_segundo_nivel

	t_tabla_paginas_segundo_nivel* tabla2 = list_get(tabla_paginas_segundo_nivel_global, valor);

	if(esta_en_memoria(tabla2, entrada_tabla_segundo_nivel)){

		//me robo el marco en una variable
		//send(cpu del nro de marco)
	}
	else{

		//EJECUTAR un PF
		//SI HAY
	}

	//recv estructura de operacion

	EstructuraDeOperacion* estructuraDeOperacion;

	if( estructuraDeOperacion->tipoDeOperacion == 1){


		//variable marco
		//recv valor -> recibir con paquete de op
		//poenr bit de uso en 1 y modificado
		//nro marco * tamaño pagina + desp
		//memcpy en el offset
		//memcpy(memPrincipal + marco*tamanoPagina + offset, mensaje a escribir, tamano mensaje);

	}

	else if ( estructuraDeOperacion->tipoDeOperacion == 0){

		//variable marco
		//void* buffer = malloc(tamanoMensaje);
		//poenr bit de uso en 1
		//memcpy(buffer, memPrincipal + marco*tamanoPagina + offset, tamanoMensaje);

		//send buffer

	}


}

bool esta_en_memoria(t_tabla_paginas_segundo_nivel* tabla, uint32_t indice_tabla_segundo_nivel){

	t_entradas_segundo_nivel* entrada;

	entrada = list_get(tabla->tabla, indice_tabla_segundo_nivel);

	return entrada->bPres == 1;

}

bool ordenar(void* entrada1, void* entrada2){

	t_entradas_segundo_nivel* entrada = entrada1;
	t_entradas_segundo_nivel* aux = entrada2;

	return entrada->nroFrame < aux->nroFrame;

}

void page_fault(t_tabla_paginas_segundo_nivel* tabla, uint32_t indice_tabla_segundo_nivel, 	t_proceso* proceso){


	if( list_size(proceso->paginasDelProceso) == marcosPorProceso){

		//TODO:EJECUTAR ALGORITMO

	}

	else{

		t_entradas_segundo_nivel* entrada;

		int frame = obtener_y_ocupar_frame();

		entrada = list_get(tabla->tabla, indice_tabla_segundo_nivel);

		entrada->bPres = 1;
		entrada->nroFrame = frame;

		list_add_sorted(proceso->paginasDelProceso, entrada, ordenar);

		//send a cpu nro marco

	}
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

	//PONER COMO GLOBAL
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

	mostrar_vector(vector_primer_nivel);
	list_add(tabla_paginas_primer_nivel_global, vector_primer_nivel);

	return indice_tabla_primer_nivel;

}

void mostrar_vector(uint32_t* vector_primer_nivel){

	printf("\nTabla de primer nivel\n");
	printf("|Indice tabla segundo nivel|\n");

	for(int i = 0; i < sizeof(vector_primer_nivel);i++)
		printf("|%d|\n", vector_primer_nivel[i]);
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

	t_list* aux = proceso->paginasDelProceso;

	for (int i = 0; !list_is_empty(aux); i++){

		t_entradas_segundo_nivel* entrada = list_get(aux, i);

		uint32_t indice = entrada->nroFrame;

		bitarray_clean_bit(marcosOcupados, indice);
		framesLibres++;

		memset(memoriaPrincipal + indice*tamanoPagina, '\0', tamanoPagina);
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
			limpiar_posiciones(marcosOcupadosPpal, proceso);
			log_info(logger, "MEMORIA: Se liberan los marcos ocupados del proceso");

			list_remove_and_destroy_element(procesos, i, free);
			log_info(logger, "MEMORIA: Se elimina el proceso %d", pid);

			log_info(logger, "MEMORIA: Ingresando a SWAP..");
			eliminar_archivo_swap(pid);
			log_info(logger, "SWAP: Ingresando a MEMORIA..");

			log_info(logger, "MEMORIA: Se elimina el archivo del proceso %d", pid);
		}

	}

}



void suspender_proceso(int socket_cliente){

	uint32_t pidRecibido = 0;
	recv(socket_cliente, &pidRecibido, sizeof(uint32_t), 0);
	log_info(logger, "MEMORIA-KERNEL: Se recibe un pid %d", pidRecibido);


	iniciar_suspension_proceso(pidRecibido);

}

void iniciar_suspension_proceso(uint32_t pid){

	t_proceso* proceso;

	proceso = list_get(procesos, pid);

	t_list* paginas = proceso->paginasDelProceso;

	for(int i = 0; i < list_size(paginas); i++){

		t_entradas_segundo_nivel* tabla;

		tabla = list_get(paginas, i);

		if (tabla->bMod == 1){
			guardar_archivo_en_swap(proceso->pid, proceso->tamanoProceso, tabla->nroFrame);
			log_info(logger, "MEMORIA-KERNEL: Archivo del proceso %d guardado en swap", proceso->pid);
			tabla->bMod = 0;
			log_info(logger, "MEMORIA-KERNEL: Se libera memoria del proceso %d en memoria principal", proceso->pid);
		}

		tabla->bPres = 0;

	}

	limpiar_posiciones(marcosOcupadosPpal, proceso);

	proceso->paginasDelProceso = NULL;
	list_destroy(proceso->paginasDelProceso); //HAY MEMORY LEACKS


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
