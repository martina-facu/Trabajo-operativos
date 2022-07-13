/*
 * memoria.c
 *
 *  Created on: 22 jun. 2022
 *      Author: utnso
 */

#include "memoria.h"


int main(void)
{
	//	iniciar log
	logger = initLogger("memoria.log", "MEMORIA", LOG_LEVEL_INFO);

	//	cargar archivo de config
	config = cargarConfiguracion("memoria.config");

	log_info(logger, "MEMORIA: Estoy por comenzar el inicio del server Memoria");

	//	se inicializan las estructuras(array de memoria vacia, tabla de paginas
	iniciar_memoria();

	//	inicializa el servidor y se espera a los clientes
	iniciar_comunicacion();

	//Finalizamos todo (revisar por si falta finalizar algo)

	//liberar_memoria(acceptedConecctionKernel, acceptedConecctionCPU, logger, config);

	return EXIT_SUCCESS;
}

//-----------------------------CONFIGURACION-----------------------------

t_config_memoria* cargarConfiguracion(char* configPath)
{

	t_config* configFile = config_create(configPath);
	t_config_memoria* configTemp = crearConfigMemoria();

	configTemp->listen_port = config_get_string_value(configFile, PUERTO_ESCUCHA);
		//log_info(logger, "PUERTO_ESCUCHA: %s", configTemp->listen_port);
	configTemp->memory_size = config_get_int_value(configFile, TAM_MEMORIA);
		//log_info(logger, "TAM_MEMORIA: %d", configTemp->memory_size);
	configTemp->page_size = config_get_int_value(configFile, TAM_PAGINA);
		//log_info(logger, "TAM_PAGINA: %d", configTemp->page_size);
	configTemp->table_input = config_get_int_value(configFile, ENTRADAS_POR_TABLA);
		//log_info(logger, "ENTRADAS_POR_TABLA: %d", configTemp->table_input);
	configTemp->memory_time_delay = config_get_int_value(configFile, RETARDO_MEMORIA);
		//log_info(logger, "RETARDO_MEMORIA: %d", configTemp->memory_time_delay);
	configTemp->quantity_frames_process= config_get_int_value(configFile, MARCOS_POR_PROCESO);
		//log_info(logger, "MARCOS_POR_PROCESO: %d", configTemp->quantity_frames_process);
	configTemp->swap_time_delay = config_get_int_value(configFile, RETARDO_MEMORIA);
		//log_info(logger, "RETARDO_MEMORIA: %d", configTemp->swap_time_delay);
	configTemp->path_swap = config_get_string_value(configFile, PATH_SWAP);
		log_info(logger, "PATH_SWAP: %s", configTemp->path_swap);
	configTemp->replacement_algorithm = config_get_string_value(configFile, ALGORITMO_REEMPLAZO);
		//log_info(logger, "ALGORITMO_REEMPLAZO: %s", configTemp->replacement_algorithm);
	configTemp->memoryIP = config_get_string_value(configFile, IP_MEMORIA);
		//log_info(logger, "IP_MEMORIA: %s", configTemp->memoryIP);

	return configTemp;
}

t_config_memoria* crearConfigMemoria(void)
{
	t_config_memoria* config = malloc(sizeof(t_config_memoria));
	return config;
}




//-----------------------------MEMORIA-----------------------------

//Inicio la memoria principal y la virtual con paginacion

int iniciar_memoria(){

	log_info(logger, "MEMORIA: Reservando memoria");

	//Reservo memoria segun el archivo de config
	memoriaPrincipal = malloc(config->memory_size);

	//Verifico si el malloc reservo memoria correctamente
	if (memoriaPrincipal == NULL){
		perror("El malloc fallo\n");
		return 0;
	}

	//Seteo en toda la memoria \0 TODO
	//memset(&data,'\0',config->memory_size);

	//log_info(logger, "MEMORIA: Tamano memoria: %d", config->memory_size);

	//Obtengo cantidad de marcos
	int cantMarcosPpal = config->memory_size / config->page_size;
	//log_info(logger, "MEMORIA: Cantidad de marcos: %d", cantMarcosPpal);

	//Asigno cantidad de memoria a data
	data = asignarMemoriaBits(cantMarcosPpal);

	if (data == NULL){
		log_error(logger,"MEMORIA: El malloc fallo\n");
		return 0;
	}

	int cantMarcosDiv8 = cantMarcosPpal/8;

	memset(data,'\0',cantMarcosDiv8);

	//Inicializo el array de memoria para pagincaion en 0
	marcosOcupadosPpal = bitarray_create_with_mode(data, cantMarcosDiv8, MSB_FIRST);

	framesLibres = cantMarcosDiv8;

	//log_info(logger, "MEMORIA: Cantidad de marcos por proceso: %d", config->quantity_frames_process);

	//Genero la lista de procesos
	procesos = list_create();

	if (procesos == NULL){
			log_error(logger, "MEMORIA: Fallo creando tp_procesos");
			return 0;
	}

	//Genero las tablas de paginas
	tabla_paginas_primer_nivel_global = list_create();
	tabla_paginas_segundo_nivel_global = list_create();

	indice_tabla_primer_nivel = 0;
	indice_tabla_segundo_nivel = 0;

	//indice_tabla_primer_nivel_proceso = 0;

	//inicializo_tabla_global_primer_nivel();
	//inicializo_tabla_global_segundo_nivel();

	//mostrar_tabla_primer_nivel_global(tabla_paginas_primer_nivel);
	//mostrar_tabla_segundo_nivel_global(tabla_paginas_segundo_nivel);

	//imprimir_bitarray(marcosOcupadosPpal);

	//iniciarSwap();

	return 1;
}

int bitsToBytes(int bits){
	int bytes;
	if (bits < 8)
		bytes = 1;
	else{
		double c = (double) bits;
		bytes = ceil (c / 8.0);
	}
	return bytes;
}

char* asignarMemoriaBits(int bits){
	char* aux;
	int bytes = bitsToBytes(bits);
	//printf("BYTES: %d\n", bytes);
	aux = malloc(bytes);
	memset(aux, 0 ,bytes);
	return aux;
}

char* asignarMemoriaBytes(int bytes){
	char* aux;
	aux = malloc(bytes);
	memset(aux, 0 ,bytes);
	return aux;
}

//Imprimir vector de arrays de la memoria

void imprimir_bitarray(t_bitarray* marcosOcupadosPpal){

	int cantMarcosPpal = config->memory_size / config->page_size;
	printf("\nImprimo bitarray para ver que hay ocupado\n");

	for (int i =0; i < cantMarcosPpal;i++){
		printf("%d", bitarray_test_bit(marcosOcupadosPpal,i));
	}

}

//FINALIZAR MEMORIA

void liberar_memoria(int conexionKernel, int conexionCPU, t_log* logger, t_config* config){

	//Por ultimo liberamos conexion, log y config
	log_info(logger, "MEMORIA: Finalizando memoria :(");

	//Libero memoria paginacion
	liberar_memoria_paginacion();

	//Destruyo el bitarray
	bitarray_destroy(marcosOcupadosPpal);

	//Libero la memoria reservada
	free(memoriaPrincipal);

	//Destruyo el logger
	if (logger != NULL)
		log_destroy(logger);

	//Destruyo el config
	if (config != NULL)
		config_destroy(config);

	liberar_conexion(conexionKernel);
	liberar_conexion(conexionCPU);
}

void retardo_memoria(){

	log_info(logger, "MEMORIA: Ingresando a memoria..");
	sleep(config->memory_time_delay/1000);
}


void inicializo_tabla_global_primer_nivel(){

	//Calculo la cantidad de procesos que puede haber
	//Si tengo el tamaño de memoria y el tamaño del marco/pagina
	//Obtengo la cantidad de entradas de la memoria principal
	//Y si cada entrada tiene un tamaño fijo -> saco la cantidad de procesos

	int cantidadDeMarcos = config->memory_size / config->page_size;
	//int cantidadDeProcesos = cantidadDeMarcos / config->quantity_frames_process;

	for(int i = 0; i < cantidadDeMarcos; i++){
		//Agrego a la lista global las tablas asignadas
		list_add(tabla_paginas_primer_nivel_global, &indice_tabla_primer_nivel);

		//Aumento el indice para el proximo indice de la tabla
		indice_tabla_primer_nivel++;
	}
}

void inicializo_tabla_global_segundo_nivel(){

	int cantidadDeMarcos = config->memory_size / config->page_size;

		for(int i = 0; i < cantidadDeMarcos * config->quantity_frames_process; i++){

			t_tabla_paginas_segundo_nivel* entrada_tabla_segundo_nivel = malloc(sizeof(t_tabla_paginas_segundo_nivel));

			entrada_tabla_segundo_nivel->bMod = 0;
			entrada_tabla_segundo_nivel->bPres = 0;
			entrada_tabla_segundo_nivel->bUso = 0;
			entrada_tabla_segundo_nivel->nroFrame = i; //se pone en -1 pq no tiene ningun frame asignado todavía

			list_add(tabla_paginas_segundo_nivel_global, entrada_tabla_segundo_nivel);

		}
}


void mostrar_tabla_primer_nivel_global(t_list* lista){

	printf("\nNro de tabla de primer nivel\n");

	for (int i = 0; i < list_size(lista); i++){

		//t_tabla_paginas_primer_nivel* entrada_tabla_primer_nivel = malloc(sizeof(t_tabla_paginas_primer_nivel));

		//entrada_tabla_primer_nivel = list_get(tabla_paginas_primer_nivel, i);

		//printf("\n|\t %d \t|", entrada_tabla_primer_nivel->nroDeTabla);
		printf("\n|\t %d \t|", i);
	}

}

void mostrar_tabla_segundo_nivel_global(t_list* lista){

	printf("\nTabla de paginas de segundo nivel\n");
	printf("\n|\tMarco\t|\tP\t|\tU\t|\tM|");

	for (int i = 0; i < list_size(lista); i++){

		t_tabla_paginas_segundo_nivel* entrada_tabla_segundo_nivel = malloc(sizeof(t_tabla_paginas_segundo_nivel));

		entrada_tabla_segundo_nivel = list_get(tabla_paginas_segundo_nivel_global, i);

		printf("\n|\t%d \t|\t%d \t|\t%d \t|\t%d|", entrada_tabla_segundo_nivel->nroFrame, entrada_tabla_segundo_nivel->bPres, entrada_tabla_segundo_nivel->bUso, entrada_tabla_segundo_nivel->bMod);
	}

}

int cantidad_de_paginas_que_ocupa(int tamanio){

	return floor(tamanio / config->page_size);
}
