/*
 * memoria.c
 *
 *  Created on: 22 jun. 2022
 *      Author: utnso
 */

#include "memoria.h"


int main(void)
{
	logger = initLogger("memoria.log", "MEMORIA", LOG_LEVEL_TRACE);

	config = cargarConfiguracion("memoria.config");

	marcosPorProceso = config->quantity_frames_process;

	entradasPorTabla = config->table_input;
	tamanoMemoria = config->memory_size;
	tamanoPagina = config->page_size;
	memoryDelay = config->memory_time_delay;
	swapDelay  = config->swap_time_delay;
	algorithm = config->replacement_algorithm;
	pSwap = config->path_swap;

	log_info(logger, "MEMORIA: Estoy por comenzar el inicio del server Memoria");

	//	se inicializan las estructuras(array de memoria vacia, tabla de paginas
	iniciar_memoria();

	//	inicializa el servidor y se espera a los clientes
	iniciar_comunicacion();

	//Finalizamos todo (revisar por si falta finalizar algo)

	mostrar_lista_procesos(procesos);
	mostrar_tabla_primer_nivel_global(tabla_paginas_primer_nivel_global);
	mostrar_tabla_segundo_nivel_global(tabla_paginas_segundo_nivel_global);

	liberar_memoria(acceptedConecctionKernel, acceptedConecctionCPU, logger, config);

	return EXIT_SUCCESS;
}

//-----------------------------CONFIGURACION-----------------------------

t_config_memoria* cargarConfiguracion(char* configPath)
{

	t_config* configFile = config_create(configPath);
	t_config_memoria* configTemp = crearConfigMemoria();

	configTemp->listen_port = config_get_string_value(configFile, PUERTO_ESCUCHA);
	configTemp->memory_size = config_get_int_value(configFile, TAM_MEMORIA);
	configTemp->page_size = config_get_int_value(configFile, TAM_PAGINA);
	configTemp->table_input = config_get_int_value(configFile, ENTRADAS_POR_TABLA);
	configTemp->memory_time_delay = config_get_int_value(configFile, RETARDO_MEMORIA);
	configTemp->quantity_frames_process= config_get_int_value(configFile, MARCOS_POR_PROCESO);
	configTemp->swap_time_delay = config_get_int_value(configFile, RETARDO_MEMORIA);
	configTemp->path_swap = config_get_string_value(configFile, PATH_SWAP);
	configTemp->replacement_algorithm = config_get_string_value(configFile, ALGORITMO_REEMPLAZO);
	configTemp->memoryIP = config_get_string_value(configFile, IP_MEMORIA);

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

	log_info(logger, "MEMORIA: Se inicia la memoria");
	log_info(logger, "SWAP: Se inicia el swap");

	log_info(logger, "MEMORIA: Reservando memoria");

	memoriaPrincipal = malloc(tamanoMemoria);
	log_info(logger, "MEMORIA PRINCIPAL: %d ", memoriaPrincipal);

	if (memoriaPrincipal == NULL){
		perror("El malloc fallo\n");
		return 0;
	}

	//Seteo en toda la memoria \0 TODO
	//memset(&data,'\0',config->memory_size);

	log_info(logger, "MEMORIA: Tamano memoria: %d", tamanoMemoria);

	int cantMarcosPpal = tamanoMemoria / tamanoPagina;

	//Asigno cantidad de memoria a data
	data = asignarMemoriaBits(cantMarcosPpal);

	if (data == NULL){
		log_error(logger,"MEMORIA: El malloc fallo\n");
		return 0;
	}

	int cantMarcosDiv8 = cantMarcosPpal/8;

	memset(data,'\0',cantMarcosDiv8);

	marcosOcupadosPpal = bitarray_create_with_mode(data, cantMarcosDiv8, MSB_FIRST);

	framesLibres = cantMarcosDiv8;

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

	//imprimir_bitarray(marcosOcupadosPpal);

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

	int cantMarcosPpal = tamanoMemoria / tamanoPagina;
	printf("\nImprimo bitarray para ver que hay ocupado\n");

	for (int i =0; i < cantMarcosPpal;i++){
		log_info(logger, "%d", bitarray_test_bit(marcosOcupadosPpal,i));

	}

}

//FINALIZAR MEMORIA

void liberar_memoria(int conexionKernel, int conexionCPU, t_log* logger, t_config* config){

	//Por ultimo liberamos conexion, log y config
	log_info(logger, "MEMORIA: Finalizando memoria");
	log_info(logger, "SWAP: Finalizando SWAP");

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
	sleep(memoryDelay/1000);
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
	printf("\n|Pagina|\tMarco\t|\tP\t|\tU\t|\tM|");

	t_entradas_segundo_nivel* entrada;// = malloc(sizeof(t_entradas_segundo_nivel));
	t_tabla_paginas_segundo_nivel* unaTabla;

	for (int i = 0; i < list_size(lista); i++){

		unaTabla = list_get(lista, i);
		entrada = list_get(unaTabla->tabla, i);

		printf("\n|%d\t|\t%d|\t%d\t|\t%d\t|\t%d\t|\n",i, entrada->nroFrame, entrada->bPres, entrada->bUso, entrada->bMod);
	}

}



void mostrar_lista_procesos(t_list* lista){

	printf("\nLista de procesos\n");
		printf("|Nro Proceso\t|Tamano Proceso\t|Nro Tabla primer nivel|\n");

	for (int i = 0; i < list_size(lista); i++){

		t_proceso* proceso;//= malloc(sizeof(t_proceso)); TODO: si rompe probar con el malloc

		proceso = list_get(lista, i);

		printf("|%d\t|%d\t\t\t\t|%d\t\t\t|",proceso->pid, proceso->tamanoProceso,proceso->entrada_tabla_primer_nivel);
	}

}




