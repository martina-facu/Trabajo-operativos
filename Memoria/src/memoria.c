/*
 ============================================================================
 Name        : Memoria.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */


#include "memoria.h"

const char* CONFIG_FIELDS[] = {PUERTO_ESCUCHA, TAM_MEMORIA, TAM_PAGINA, ENTRADAS_POR_TABLA, ALGORITMO_REEMPLAZO, MARCOS_POR_PROCESO, RETARDO_MEMORIA, PATH_SWAP};

t_log* iniciar_logger(void){

	if((logger = log_create("ARCHIVO_LOG", "Memoria", 1, LOG_LEVEL_INFO)) == NULL){

		printf("No se pueded crear el log\n");
		exit(1);

	}

	else{
		logger = log_create("Memoria.log", "Memoria", 1, LOG_LEVEL_INFO);
		log_info(logger, "-------------------------");
		log_info(logger, "Iniciando log Proceso Memoria");
		log_info(logger, "-------------------------");

		return logger;

	}
}


t_config_memoria* cargar_configuracion(char* ruta_del_config){

	t_config* archivo_config = config_create(ruta_del_config);
	config = malloc(sizeof(t_config_memoria));

	if(config == NULL){
		log_error(logger, "Archivo de configuracion no encontrado");
		EXIT_FAILURE;
	}

	config->listen_port = config_get_string_value(archivo_config, PUERTO_ESCUCHA);
	config->memory_size = config_get_int_value(archivo_config, TAM_MEMORIA);
	config->page_size = config_get_int_value(archivo_config, TAM_PAGINA);
	config->table_input = config_get_int_value(archivo_config, ENTRADAS_POR_TABLA);
	config->memory_time_delay = config_get_int_value(archivo_config, RETARDO_MEMORIA);
	config->quantity_frames_process= config_get_int_value(archivo_config, MARCOS_POR_PROCESO);
	config->swap_time_delay = config_get_int_value(archivo_config, RETARDO_MEMORIA);
	config->path_swap = config_get_string_value(archivo_config, PATH_SWAP);
	config->replacement_algorithm = config_get_string_value(archivo_config, ALGORITMO_REEMPLAZO);

	//validaciones no se si van

	if ((config->memory_size % config->page_size) != 0) {
		log_error(logger, "El tamaño de memoria %d no es multiplo del tamaño de pagina", config->memory_size);
		EXIT_FAILURE;
	}

	if((config->page_size % 2) != 0){
		log_error(logger, "El tamaño de pagina debe ser multiplo de 2");
		EXIT_FAILURE;
	}

	return config;
}

void liberear_memoria (int conexion, t_log* logger, t_config* config){

//Por ultimo liberamos conexion, log y config
log_info(logger, "Finalizando memoria :(");


//Libero memoria paginacion
//liberarMemoriaPaginacion();

//Libero la memoria reservada
free(memoriaPrincipal);

if (logger != NULL)
	log_destroy(logger);

if (config != NULL){
	config_destroy(config);
}

//liberar_conexion(conexion);
}


int main(void){

//cargar archivo de config
config = cargar_configuracion(path_config);

//iniciar log
logger = iniciar_logger();

//muestro la configuracion
mostrar_logger();

/* mem = iniciar_memoria();

//
if (!mem)
return 0;

*/

log_trace(logger, "Estoy por comenzar el inicio del server Memoria");

//iniciar_servidor();

//TERMINAR SERVIDOR
//close(socket_server);

//terminar programa destruyendo configs y logger
//liberear_memoria (conexion, logger, config);


iniciar_memoria();

return EXIT_SUCCESS;


}


void mostrar_logger(){

	//LOGEAMOS EL VALOR DE LAS CONFIG
	log_info(logger, "PUERTO_ESCUCHA: %s", config->listen_port);
	log_info(logger, "TAM_MEMORIA: %d", config->memory_size);
	log_info(logger, "TAM_PAG: %d", config->page_size);
	log_info(logger, "ENTRADAS_POR_TABLA: %d", config->table_input);
	log_info(logger, "RETARDO_MEMORIA: %d", config->memory_time_delay);
	log_info(logger, "ALGORITMO_REEMPLAZO: %s", config->replacement_algorithm);
	log_info(logger, "MARCOS_POR_PROCESO: %d", config->quantity_frames_process);
	log_info(logger, "RETARDO_SWAP: %d", config->swap_time_delay);
	log_info(logger, "PATH_SWAP: %s", config->path_swap);
}

//Inicio la memoria principal y la virtual con paginacion

int iniciar_memoria(){

	//Reservo memoria segun el archivo de config
	memoriaPrincipal = malloc(config->memory_size);

	//Verifico si el malloc reservo memoria correctamente
	if (memoriaPrincipal == NULL){
		perror("El malloc fallo\n");
		return 0;
	}

	//Obtengo cantidad de marcos
	int cantMarcosPpal = config->memory_size / config->page_size;

	//asigno cantidad de memoria a data
	data = asignarMemoriaBits(cantMarcosPpal);

	if (data == NULL){
		perror("El malloc fallo\n");
		return 0;
	}

	int cantMarcosDiv8 = cantMarcosPpal/8;

	memset(data,0,cantMarcosDiv8);

	//inicializo el array de memoria para pagincaion en 0
	marcosOcupadosPpal = bitarray_create_with_mode(data, cantMarcosDiv8, MSB_FIRST);

	//Indico la cantidad de paginas por proceso
	cantidadDePaginasPorProceso = config->quantity_frames_process;

	tablaDePaginas = list_create();

	imprimir_bitarray(marcosOcupadosPpal);

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

void imprimir_bitarray(t_bitarray* marcosOcupadosPpal){

	int cantMarcosPpal = config->memory_size / config->page_size;
	printf("Imprimo bitarray para ver que hay ocupado\n");

	for (int i =0; i < cantMarcosPpal;i++){
		printf("%d", bitarray_test_bit(marcosOcupadosPpal,i));
	}

}

//FUNCIONES DE MENSAJES

char* leer_string(char* buffer, int* desplazamiento)
{
	//char* buf = (char*) buffer;

	int tamanio = leer_entero(buffer, &desplazamiento);
	//printf("allocating / copying %d \n",tamanio);
	/*

	memcpy(&tamanio, buffer + (*desplazamiento), sizeof(int));
	(*desplazamiento)+=sizeof(int);
	 */
	char* valor = malloc(tamanio);
	memcpy(valor, buffer+(*desplazamiento), tamanio);
	(*desplazamiento)+=tamanio;

	return valor;
}
