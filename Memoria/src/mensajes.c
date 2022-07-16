#include "mensajes.h"

//-----------------------------MENSAJES-----------------------------

//CPU
void entradas_y_tamanio_de_pagina(int socket_cliente, t_config_memoria* tconfig, uint8_t codOp){

	log_info(logger, "MEMORIA-CPU: Se recibe codigo de operacion %d", codOp);

	retardo_memoria();

	uint32_t cEntradas = tconfig->table_input;
	uint32_t pSize = tconfig->page_size;

	log_info(logger, "cEntradas: %d", cEntradas);
	log_info(logger, "paginas size: %d", pSize);
	log_info(logger, "MEMORIA-CPU: se envia mensaje entradas y tamano");
	send(socket_cliente, &cEntradas, sizeof(uint32_t),0);
	send(socket_cliente, &pSize, sizeof(uint32_t),0);

}

void devolver_numero_tabla_segundo_nivel(int socket_cliente, uint8_t codOp){

	log_info(logger, "MEMORIA-CPU: Se recibe codigo de operacion %d", codOp);
	//uint32_t numeroEntradaSegundoNivel;

	Coordenada_tabla* coordenada = malloc(sizeof(Coordenada_tabla));

	t_proceso* proceso;

	//TODO: REHACER FUNCION COORDENADA
	coordenada = recibir_coordenada(socket_cliente);
	log_info(logger, "MEMORIA-CPU: Recibo mensaje de cpu");
	log_info(logger, "MEMORIA-CPU: Coordenada %d %d", coordenada->id_tabla, coordenada->numero_entrada);
	retardo_memoria();

	proceso = list_get(procesos, coordenada->id_tabla);

//	uint32_t indice = proceso->entrada_tabla_primer_nivel;

	uint32_t* tabla_primer_nivel = list_get(tabla_paginas_primer_nivel_global, coordenada->id_tabla);

	uint32_t valorr = *(tabla_primer_nivel + coordenada->numero_entrada);

	log_info(logger, "MEMORIA-CPU: Se envia el nro de tabla de segundo nivel");

	send(socket_cliente, &valorr, sizeof(uint32_t), 0);

	uint32_t entrada_tabla_segundo_nivel;

	recv(socket_cliente, &entrada_tabla_segundo_nivel, sizeof(uint32_t), 0);

	t_tabla_paginas_segundo_nivel* tabla_segundo_nivel = list_get(tabla_paginas_segundo_nivel_global, valorr);

	t_entradas_segundo_nivel* entrada_segundo_nivel = list_get(tabla_segundo_nivel->tabla, entrada_tabla_segundo_nivel);

	if(esta_en_memoria(entrada_segundo_nivel, entrada_tabla_segundo_nivel)){

		//me robo el marco en una variable
		send(socket_cliente, entrada_segundo_nivel->nroFrame, sizeof(uint32_t), 0);

	}
	else{
		log_info(logger, "asdasd");
		page_fault( tabla_segundo_nivel, valorr, proceso,socket_cliente);
		//SI HAY
	}
	//recv estructura de operacion

	EstructuraDeOperacion* operacion = malloc(sizeof(EstructuraDeOperacion));

	recv(socket_cliente,&(operacion->tipoDeOperacion),sizeof(uint8_t),0);

	uint32_t tamano;
	uint32_t direccion;
	uint32_t valor =0;
	void* buffer;

	if(operacion->tipoDeOperacion == 9){
		recv(socket_cliente,&direccion,sizeof(uint32_t),0);
		entrada_segundo_nivel->bUso=1;
		buffer = malloc(sizeof(uint32_t));
		memcpy(buffer,memoriaPrincipal + direccion, sizeof(uint32_t));
		send(socket_cliente,buffer,sizeof(uint32_t),0);

	}
	else if(operacion->tipoDeOperacion == 9){
		recv(socket_cliente,&direccion,sizeof(uint32_t),0);
		recv(socket_cliente,&valor,sizeof(uint32_t),0);
		memcpy(memoriaPrincipal + direccion,&valor, sizeof(uint32_t));
		entrada_segundo_nivel->bUso=1;
		entrada_segundo_nivel->bMod=1;
		uint8_t ok = 1;
		send(socket_cliente,&ok,sizeof(uint8_t),0);

	}

	//if( estructuraDeOperacion->tipoDeOperacion == 1){


		//variable marco
		//recv valor -> recibir con paquete de op
		//poenr bit de uso en 1 y modificado
		//nro marco * tamaño pagina + desp
		//memcpy en el offset
		//memcpy(memPrincipal + marco*tamanoPagina + offset, mensaje a escribir, tamano mensaje);}

	//else if ( estructuraDeOperacion->tipoDeOperacion == 0){

		//variable marco
		//void* buffer = malloc(tamanoMensaje);
		//poenr bit de uso en 1
		//memcpy(buffer, memPrincipal + marco*tamanoPagina + offset, tamanoMensaje);

		//send buffer

	//}
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
// TODO CUANDO INICIALIZAMOS EL PROCESO HAY QUE INICIALIZAR EL PUNTERO EN 0

void page_fault(t_tabla_paginas_segundo_nivel* tabla, uint32_t indice_tabla_segundo_nivel, 	t_proceso* proceso,int socket_cliente)
{



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

		send(socket_cliente,&frame,sizeof(uint8_t),0);
		proceso->contador++;
		proceso->punteroAlgoritmo= proceso->contador % marcosPorProceso;

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


void devolver_numero_marco_asociado(int socket_cliente, uint8_t codOp){
	/*
	log_info(logger, "MEMORIA-CPU: Se recibe codigo de operacion %d", codOp);
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
*/
}


void devolver_lectura(int socket_cliente, uint8_t codOp){
/*
 * 	log_info(logger, "MEMORIA-CPU: Se recibe codigo de operacion %d", codOp);
	t_paquete* respuesta = recibir_mensaje_cpu(socket_cliente, logger);
	uint32_t* direccion = malloc(sizeof(uint32_t));
	uint32_t tamanioMarco;// = respuesta->buffer->stream * config->page_size;

	retardo_memoria();



	memcpy(direccion, respuesta->buffer->stream, sizeof(uint32_t));

	mandar_lecto_escritura(direccion,respuesta->buffer->stream,RESULTADO_LECTURA, socket_cliente);
*/
}

void devolver_escritura (int socket_cliente, uint8_t codOp){
/*
	log_info(logger, "MEMORIA-CPU: Se recibe codigo de operacion %d", codOp);
	t_paquete* respuesta = recibir_mensaje_cpu(socket_cliente, logger);
	uint32_t* valorAEscribir = malloc(sizeof(uint32_t));

	retardo_memoria();

	memcpy(valorAEscribir, respuesta->buffer->stream, sizeof(uint32_t));

	//mandar_lecto_escritura(valorAEscribir,respuesta->buffer->stream,RESULTADO_LECTURA, socket_cliente);

	char* respuesta_escritura = "OK";
	send(socket_cliente, respuesta_escritura, sizeof(respuesta), 0);
*/
}



void inicializar_proceso(int socket_cliente, t_config_memoria* tconfig){

	log_info(logger, "MEMORIA: Voy a recibir pcb");
	pcb_t* pcb = recibirPCB(socket_cliente);
	log_info(logger, "MEMORIA-KERNEL: Se recibio un pcb");

	uint32_t mensaje = 0;

	t_proceso* proceso = malloc(sizeof(t_proceso));
	proceso->pid = pcb->pid;
	proceso->tamanoProceso = 300;//pcb->tamano;

	int ePorTabla = tconfig->table_input;
	int tamPagina = tconfig->page_size;

	int cantPag = cantidad_de_paginas_del_proceso(proceso->tamanoProceso, tamPagina);

	int cantidadEntradasP = cantidad_de_entrada_primer_nivel(proceso->tamanoProceso, ePorTabla, tamPagina);

	log_info(logger, "cantPag %d", cantPag);
	log_info(logger, "cantidadEntradasP %d", cantidadEntradasP);

	log_info(logger, "MEMORIA: Se inicializa la tabla de primer nivel del proceso %d", proceso->pid);

	proceso->entrada_tabla_primer_nivel = inicializo_tabla_primer_nivel_proceso(proceso, cantidadEntradasP, cantPag);
	indice_tabla_primer_nivel++;

	log_info(logger, "MEMORIA: Se accede a swap..");
	crear_archivo_swap(proceso->pid, cantPag);
	log_info(logger, "SWAP: Se vuelve a memoria..");

	retardo_memoria();

	log_info(logger, "MEMORIA: Se agrega proceso %d a la lista de procesos", proceso->pid);

	list_add(procesos, proceso);

	mostrar_lista_procesos(procesos);
	mostrar_tabla_primer_nivel_global(tabla_paginas_primer_nivel_global);
	mostrar_tabla_segundo_nivel_global(tabla_paginas_segundo_nivel_global);

	log_trace(logger, "MEMORIA: Se envia el numero de tabla de pagina");

	mensaje = proceso->entrada_tabla_primer_nivel;
	send(socket_cliente, &mensaje, sizeof(uint32_t), 0);

}


uint32_t inicializo_tabla_primer_nivel_proceso(t_proceso* proceso, int cantEntradas, int cantPag){

	//VER ALGORITMOS

	//listaUltimaSacada = list_create();

	uint32_t *vector_primer_nivel = (uint32_t*)calloc(entradasPorTabla, sizeof(uint32_t));

	log_info(logger, "entradas%d",cantEntradas);

	memset(vector_primer_nivel, -1, entradasPorTabla*sizeof(uint32_t));

	for(int i = 0; i < cantEntradas; i++){

		vector_primer_nivel[i] = inicializo_tabla_segundo_nivel_proceso(proceso, entradasPorTabla);

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


int inicializo_tabla_segundo_nivel_proceso(t_proceso* proceso, int entradasPorTabla){

	log_info(logger, "MEMORIA: Se reserva espacio para una tabla");
	t_tabla_paginas_segundo_nivel* unaTabla = malloc(sizeof(t_tabla_paginas_segundo_nivel));
	proceso->paginasDelProceso = list_create();
	unaTabla->tabla = list_create();
	//Algoritmos, ver si funca TODO
	//ultimaSacada_t* entrada = malloc(sizeof(ultimaSacada_t));

	for (int j = 0; j < entradasPorTabla; j++){

		t_entradas_segundo_nivel* entrada = malloc(sizeof(t_entradas_segundo_nivel));

		entrada->bMod = 1;
		entrada->bPres = 0;
		entrada->bUso = 0;
		entrada->nroFrame = j;

		list_add(unaTabla->tabla, entrada);
		list_add(proceso->paginasDelProceso, entrada);
	}

	list_add(tabla_paginas_segundo_nivel_global, unaTabla);
	int indice = list_size(tabla_paginas_segundo_nivel_global) - 1;

	//log_info(logger, "Inicializo los valores de las entradas (para algoritmos)");
	//entrada->pid = proceso->pid;
	//entrada->posicion=0;

	//log_info(logger, "MEMORIA: se agrega la entrada a una lista");
	//list_add(listaUltimaSacada,entrada);

	return indice;

}


void finalizar_proceso(int socket_cliente){

	uint32_t pid = 0;
	recv(socket_cliente, &pid, sizeof(uint32_t), 0);
	log_info(logger, "MEMORIA-KERNEL: Se recibe un pid %d para finalizar", pid);

	iniciar_eliminacion_proceso(pid);
}


void limpiar_posiciones(t_bitarray* marcosOcupados, t_proceso* proceso){

	t_list* aux = list_create();

	for (int i = 0; !list_is_empty(aux); i++){

		aux = proceso->paginasDelProceso;

		t_entradas_segundo_nivel* entrada = list_get(aux, i);

		uint32_t indice = entrada->nroFrame;

		log_info(logger, "MEMORIA: LIMPIO EL BITARRAY");
		bitarray_clean_bit(marcosOcupados, indice);
		imprimir_bitarray(marcosOcupados);
		framesLibres++;

		memset(memoriaPrincipal + indice*tamanoPagina, '\0', tamanoPagina);
	}

}

void iniciar_eliminacion_proceso(uint32_t pid){

	for(int i = 0; i < list_size(procesos); i++ ){
		t_proceso* proceso;
		proceso = list_get(procesos, i);

		log_info(logger, "%d", proceso->pid);

		if (proceso->pid == pid){
			log_info(logger, "MEMORIA: Encontre el proceso %d", pid);

			limpiar_posiciones(marcosOcupadosPpal, proceso);
			log_info(logger, "MEMORIA: Se liberan los marcos ocupados del proceso");

//			list_remove_and_destroy_element(procesos, i, free);
			log_info(logger, "MEMORIA: Se elimina el proceso %d", pid);

			log_info(logger, "MEMORIA: Ingresando a SWAP..");
			eliminar_archivo_swap(pid);
			log_info(logger, "SWAP: Ingresando a MEMORIA..");

			log_info(logger, "MEMORIA: Se elimina el archivo del proceso %d", pid);
		}

	}

}

void suspender_proceso(int socket_cliente, t_config_memoria* tconfig, void* memoriaPrincipal){

	uint32_t pid = 0;
	recv(socket_cliente, &pid, sizeof(uint32_t), 0);
	log_info(logger, "MEMORIA-KERNEL: Se recibe un pid %d para suspender", pid);
	int ePorTabla = tconfig->table_input;
	int tamPagina = tconfig->page_size;

	t_proceso* proceso;

	proceso = list_get(procesos, pid);


	while(!list_is_empty(proceso->paginasDelProceso)){
		int frame;
		t_entradas_segundo_nivel* entrada_segundo_nivel=list_get(proceso->paginasDelProceso,0);
		if (entrada_segundo_nivel->bMod == 1){
				log_info(logger, "MEMORIA: Voy a guardar la memoria en swap.");
//				guardar_archivo_en_swap(proceso->pid, proceso->tamanoProceso,entrada_segundo_nivel->nroFrame, cantPag, tamPagina, memoriaPrincipal);
				guardar_pagina_swap(pid, entrada_segundo_nivel->nroFrame,tamPagina, memoriaPrincipal);
				log_info(logger, "MEMORIA-KERNEL: Archivo del proceso %d guardado en swap", proceso->pid);
				entrada_segundo_nivel->bMod = 0;
				log_info(logger, "MEMORIA-KERNEL: Se libera memoria del proceso %d en memoria principal", proceso->pid);
				bitarray_clean_bit(marcosOcupadosPpal, entrada_segundo_nivel->nroFrame);
				imprimir_bitarray(marcosOcupadosPpal);
				framesLibres++;

				memset(memoriaPrincipal + entrada_segundo_nivel->nroFrame*tamanoPagina, '\0', tamanoPagina);
		}

		entrada_segundo_nivel->bPres = 0;

		}

//	list_clean(proceso->paginasDelProceso);
//	list_destroy(proceso->paginasDelProceso); //HAY MEMORY LEACKS

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

int cantidad_de_paginas_del_proceso(int tamanioProceso,  int tamanoPagina){

	double cantidadPaginas, fractPart,  intpart;

	cantidadPaginas = (double)tamanioProceso / tamanoPagina;

	fractPart = modf(cantidadPaginas, &intpart);

	if (fractPart > 0){
		return (int)intpart + 1;
	}
	else
		return (int)intpart;


}


int cantidad_de_entrada_primer_nivel(int tamanoProceso, int entradasPorTabla, int tamanoPagina){

	double cantidadPaginas, cantEntradas, fractPart, intPart;

	cantidadPaginas = cantidad_de_paginas_del_proceso(tamanoProceso,  tamanoPagina);

	cantEntradas = (double)cantidadPaginas / entradasPorTabla;

	fractPart = modf(cantEntradas, &intPart);

		if (fractPart > 0.0){

			return (int)intPart + 1;
		}
		else
			return (int)cantEntradas;

	return 1;

}
