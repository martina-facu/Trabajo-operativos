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

	log_trace(logger, "Estoy por comenzar el inicio del server Memoria");

	//	se inicializan las estructuras(array de memoria vacia, tabla de paginas
	iniciar_memoria();

	//	inicializa el servidor y se espera a los clientes
	iniciar_comunicacion();

	//Finalizamos todo (revisar por si falta finalizar algo)
	liberar_memoria(acceptedConecctionKernel, acceptedConecctionCPU, logger, config);

	return EXIT_SUCCESS;
}



//-----------------------------CONFIGURACION-----------------------------

t_config_memoria* cargarConfiguracion(char* configPath)
{

	t_config* configFile = config_create(configPath);
	t_config_memoria* configTemp = crearConfigMemoria();

	configTemp->listen_port = config_get_string_value(configFile, PUERTO_ESCUCHA);
		log_info(logger, "PUERTO_ESCUCHA: %s", configTemp->listen_port);
	configTemp->memory_size = config_get_int_value(configFile, TAM_MEMORIA);
		log_info(logger, "TAM_MEMORIA: %d", configTemp->memory_size);
	configTemp->page_size = config_get_int_value(configFile, TAM_PAGINA);
		log_info(logger, "TAM_PAGINA: %d", configTemp->page_size);
	configTemp->table_input = config_get_int_value(configFile, ENTRADAS_POR_TABLA);
		log_info(logger, "ENTRADAS_POR_TABLA: %d", configTemp->table_input);
	configTemp->memory_time_delay = config_get_int_value(configFile, RETARDO_MEMORIA);
		log_info(logger, "RETARDO_MEMORIA: %d", configTemp->memory_time_delay);
	configTemp->quantity_frames_process= config_get_int_value(configFile, MARCOS_POR_PROCESO);
		log_info(logger, "MARCOS_POR_PROCESO: %d", configTemp->quantity_frames_process);
	configTemp->swap_time_delay = config_get_int_value(configFile, RETARDO_MEMORIA);
		log_info(logger, "RETARDO_MEMORIA: %d", configTemp->swap_time_delay);
	configTemp->path_swap = config_get_string_value(configFile, PATH_SWAP);
		log_info(logger, "PATH_SWAP: %s", configTemp->path_swap);
	configTemp->replacement_algorithm = config_get_string_value(configFile, ALGORITMO_REEMPLAZO);
		log_info(logger, "ALGORITMO_REEMPLAZO: %s", configTemp->replacement_algorithm);
	configTemp->memoryIP = config_get_string_value(configFile, IP_MEMORIA);
		log_info(logger, "IP_MEMORIA: %s", configTemp->memoryIP);

	return configTemp;
}

t_config_memoria* crearConfigMemoria(void)
{
	t_config_memoria* config = malloc(sizeof(t_config_memoria));
	return config;
}


//-----------------------------SERVIDOR-----------------------------


void iniciar_comunicacion(){

	//	Seteo en 0 a los set de descriptores a revisar por el select
		FD_ZERO(&read_fd_set);
		FD_ZERO(&master_fd_set);

		//	Levanto servers de atencion al Kernel y atencion al CPU
		//	Levanto el server para el Kernel
		memorySocketServer = levantar_server(config->memoryIP, config->listen_port, logger);

		//	Verifico si el descriptor es mayor o menor al maximo o al minimo del select
		//	Maximo
		if( fdmax < memorySocketServer)
			fdmax = memorySocketServer;
		//	Minimo
		if( memorySocketServer < fdmin)
			fdmin = memorySocketServer;
		//	Agrego el descriptor del server de DISPATCH al maestro del select
		FD_SET(memorySocketServer, &master_fd_set);

		while(1)
		{
			/*
			 * 	A partir de aca empieza el codigo de comunicaciones de la Memoria
			 *
			 *
			 *
			 */
			//	Seteo tiempos de respuesta del select
			tiempoSelect.tv_sec = 0;
			tiempoSelect.tv_usec = 500;

			//	Clono el set maestro en uno de read para revision del select
			read_fd_set = master_fd_set;

			//	Reviso las conexiones con el select
			//	Si hay un error lo logueo
			if (select(fdmax+1, &read_fd_set, NULL, NULL, &tiempoSelect) < 0)
			{
				strerror(errno);
				log_error(logger, "Error ejecutar el select %d", errno);
				//	Validar si la falla hace que tenga que matar la CPU
				//	return(EXIT_FAILURE);
			}
			//	De no haber errores las proceso
			else
			{
				for(int i = fdmin;  i <= fdmax; i++)
				{
					//	Valido los distintos casos y actuo de acuerdo a cada uno
					//	Si el descriptor a revisar es el server de Memoria
					//	acepto la comunicacion y luego valido si es el alguno de los
					//	clientes que tengo que atender. Caso contrario cierro la conexion
					if(i == memorySocketServer)
					{	//	Verifico si se recibio informacion en el descriptor de server sea del Kernel o del CPU
						if (FD_ISSET(memorySocketServer, &read_fd_set))
						{
							aceptoYEvaluoConexion(memorySocketServer);
						}
					}
					if(i == acceptedConecctionKernel)
						//	Verifico si se recibio informacion proveniente del Kernel
						if (FD_ISSET(acceptedConecctionKernel, &read_fd_set))
							manejo_mensajes_kernel(acceptedConecctionKernel);


					if(i == acceptedConecctionCPU)
						//	Verifico si se recibio informacion proveniente del Kernel
						if (FD_ISSET(acceptedConecctionCPU, &read_fd_set))
							manejo_mensajes_cpu(acceptedConecctionCPU);

				}
			}
		}

}

int levantar_server(char* ipServer, char* portServer, t_log* logger)
{
	int socket;

	//	Inicio el servidor en la IP y puertos leidos desde el archivo de configuracion
	socket = iniciar_servidor(ipServer, portServer);
	log_info(logger, "Socket en el que se levanta el server de Memoria: %d", socket);

	return socket;
}

/*
 *  Funcion: aceptoYEvaluoConexion
 *  Entradas: 	int socketAnalizar		socket que se esta analizando en el select
 *  Salidas: void
 *  Razon: 	Aceptar conexion (y de corresponder mantenerla) de acuerdo al cliente que se conecte
 *  Autor:
 */
void aceptoYEvaluoConexion(int socketAnalizar)
{
	int temporalAcceptedConnection;
//	uint8_t handshake;

	//	Acepto las conexiones para luego validar que tipo es y si corresponde mantenerla
	temporalAcceptedConnection = esperar_cliente(socketAnalizar);
	log_info(logger, "Se acepto temporalmente conexion en el puerto: %d para evaluar la misma", temporalAcceptedConnection);

	//	Recibo el tipo de mensaja para saber si es Kernel o CPU
	uint8_t mensaje = 0;
	recv(temporalAcceptedConnection, &mensaje, sizeof(uint8_t), 0);
	log_info(logger, "Mensaje recibido en el server Memoria para aceptar conexiones: %d", mensaje);

	switch(mensaje)
	{

		//	Se recibio mensaje del Kernel
		case INICIAR_CONEXION_KERNEL:
			//	Valido si ya hay un Kernel conectado
			//	Si el grado de concurrencia admite que se sigan aceptando
			//	conexiones, la acepto. Sino omito lo recibido.
			if(connectionsKernel < CONCURRENT_CONNECTION)
			{
				acceptedConecctionKernel = validoYAceptoConexionKernel(temporalAcceptedConnection);
			}
		break;
		//	Se recibio mensaje de la CPU
		case INICIAR_CONEXION_CPU:
			//	Valido si ya hay una CPU conectada
			//	Si el grado de concurrencia admite que se sigan aceptando
			//	conexiones, la acepto. Sino omito lo recibido.
			if(connectionsCPU < CONCURRENT_CONNECTION)
			{
				acceptedConecctionCPU = validoYAceptoConexionCPU(temporalAcceptedConnection);
			}
		break;
		default:
			log_info(logger,"El mensaje recibido no corresponde a ninguno de los preestablecidos: %d", mensaje);
			log_info(logger,"Procedo a cerrar sesion temporal establecida en el descriptor: %d", temporalAcceptedConnection);
			close(temporalAcceptedConnection);
		break;
		//	Cualquier otro mensaje desconozco como manejarlo por lo que lo omito.
	}
}
/*
 *  Funcion: validoYAceptoConexionKernel
 *  Entradas: 	int temporalAcceptedConnection	Socket aceptado en forma temporal
 *  Salidas: int	Socket donde quedo en forma definitiva establecida a conexion con el Kernel
 *  Razon: 	Validar que se haya establecido una conexion real con el Kernel y finalizar el contacto
 *  		con la misma
 *  Autor:
 */
int validoYAceptoConexionKernel(int temporalAcceptedConnection)
{
	int acceptedConecctionKernel;
	uint8_t handshake;


	//	Acepto la conexion del Kernel
	acceptedConecctionKernel = temporalAcceptedConnection;
	log_info(logger, "Se acepto la conexion del Kernel en el socket: %d", acceptedConecctionKernel);
	//	Agrego el descrilptor al maestro
	FD_SET(acceptedConecctionKernel, &master_fd_set);
	//	Valido si tengo que cambiar el maximo o el minimo
	//	Maximo
	if (acceptedConecctionKernel > fdmax)
	{
		fdmax = acceptedConecctionKernel;
	}
	//	Minimo
	if (acceptedConecctionKernel < fdmin)
	{
		fdmin = acceptedConecctionKernel;
	}
	log_info(logger, "Se agrego al set de descriptores el descriptor: %d", acceptedConecctionKernel);

	//	Devuelvo el handshake predeterminado
	handshake = ACEPTAR_CONEXION_KERNEL;
	send(acceptedConecctionKernel, &handshake, sizeof(uint8_t), 0);


	//	Incremento el grado de concurrencia actual del Kernel
	connectionsKernel++;

	return acceptedConecctionKernel;
}

/*
 *  Funcion: validoYAceptoConexionCPU
 *  Entradas: 	int temporalAcceptedConnection	Socket aceptado en forma temporal
 *  Salidas: int	Socket donde quedo en forma definitiva establecida a conexion con la CPU
 *  Razon: 	Validar que se haya establecido una conexion real con la CPU y finalizar el contacto
 *  		con la misma
 *  Autor:
 */
int validoYAceptoConexionCPU(int temporalAcceptedConnection)
{
	int acceptedConecctionCPU;
	uint8_t handshake;


	//	Acepto la conexion del Kernel
	acceptedConecctionCPU = temporalAcceptedConnection;
	log_info(logger, "Se acepto la conexion del CPU en el socket: %d", acceptedConecctionCPU);
	//	Agrego el descrilptor al maestro
	FD_SET(acceptedConecctionCPU, &master_fd_set);
	//	Valido si tengo que cambiar el maximo o el minimo
	//	Maximo
	if (acceptedConecctionCPU > fdmax)
	{
		fdmax = acceptedConecctionCPU;
	}
	//	Minimo
	if (acceptedConecctionCPU < fdmin)
	{
		fdmin = acceptedConecctionCPU;
	}
	log_info(logger, "Se agrego al set de descriptores el descriptor: %d", acceptedConecctionCPU);

	//	Devuelvo el handshake predeterminado
	handshake = ACEPTAR_CONEXION_CPU;
	send(acceptedConecctionCPU, &handshake, sizeof(uint8_t), 0);


	//	Incremento el grado de concurrencia actual del Kernel
	connectionsCPU++;

	return acceptedConecctionCPU;
}

/*
 *  Funcion: validoYAceptoConexionKernel
 *  Entradas: 	int acceptedConnectionKernel	Socket aceptado del kernel
 *  Salidas: int
 *  Razon: 	Validar el tipo de mensaje que se recibe del kernel y responder
 */

int manejo_mensajes_kernel(int acceptedConecctionKernel){

	uint8_t mensaje = 0;
	recv(acceptedConecctionKernel, &mensaje, sizeof(uint8_t), 0);
	log_info(logger, "Mensaje recibido en el server Memoria para aceptar conexiones: %d", mensaje);

	switch(mensaje){

				case INICIALIZAR_PROCESO:
					inicializar_proceso(acceptedConecctionKernel);
				break;

				case SUSPENDER_PROCESO: //CPU
					suspender_proceso(acceptedConecctionKernel);
				break;

				case FINALIZAR_PROCESO:
					finalizar_proceso(acceptedConecctionKernel);
				break;

				case ACCEDER_TABLA_DE_PAGINAS:
					//acceder_tabla_de_paginas(acceptedConecctionKernel);
				break;

				case ACCEDER_ESPACIO_DE_USUARIO:
					//acceder_espacio_de_usuario(acceptedConecctionKernel);
				break;

				default:
					log_info(logger,"El mensaje recibido no corresponde a ninguno de los preestablecidos: %d", mensaje);
					log_info(logger,"Procedo a cerrar sesion temporal establecida en el descriptor: %d", acceptedConecctionKernel);
					close(acceptedConecctionKernel);
				break;
			}

	return acceptedConecctionKernel; //ESTO ESTA BIEN HERNAN?

}


/*
 *  Funcion: validoYAceptoConexionCPU
 *  Entradas: 	int acceptedConnectionCPU	Socket aceptado del cpu
 *  Salidas: int
 *  Razon: 	Validar el tipo de mensaje que se recibe del cpu y responder
 */

int manejo_mensajes_cpu(int acceptedConecctionCPU){
	uint8_t mensaje = 0;
	recv(acceptedConecctionKernel, &mensaje, sizeof(uint8_t), 0);
	log_info(logger, "Mensaje recibido en el server Memoria para aceptar conexiones: %d", mensaje);

	switch(mensaje){
					default:
						log_info(logger,"El mensaje recibido no corresponde a ninguno de los preestablecidos: %d", mensaje);
						log_info(logger,"Procedo a cerrar sesion temporal establecida en el descriptor: %d", acceptedConecctionKernel);
						close(acceptedConecctionKernel);
					break;
				}

		return acceptedConecctionCPU; //ESTO ESTA BIEN HERNAN?
}

void liberar_conexion(int socket_cliente){

	close(socket_cliente);
}

//-----------------------------MEMORIA-----------------------------

//Inicio la memoria principal y la virtual con paginacion

int iniciar_memoria(){

	log_info(logger, "Reservando memoria");
	//Reservo memoria segun el archivo de config
	memoriaPrincipal = malloc(config->memory_size);

	//Verifico si el malloc reservo memoria correctamente
	if (memoriaPrincipal == NULL){
		perror("El malloc fallo\n");
		return 0;
	}

	log_info(logger, "Tamano memoria: %d", config->memory_size);

	//Obtengo cantidad de marcos
	int cantMarcosPpal = config->memory_size / config->page_size;
	log_info(logger, "Cantidad de marcos: %d", cantMarcosPpal);

	//Asigno cantidad de memoria a data
	data = asignarMemoriaBits(cantMarcosPpal);

	if (data == NULL){
		perror("El malloc fallo\n");
		return 0;
	}

	int cantMarcosDiv8 = cantMarcosPpal/8;

	memset(data,'\0',cantMarcosDiv8);

	//Inicializo el array de memoria para pagincaion en 0
	marcosOcupadosPpal = bitarray_create_with_mode(data, cantMarcosDiv8, MSB_FIRST);

	//Indico la cantidad de paginas por proceso
	cantidadDePaginasPorProceso = config->quantity_frames_process;
	log_info(logger, "Cantidad de marcos por proceso: %d", config->quantity_frames_process);

	//Genero la lista de tabla de paginas 
	tp_procesos = list_create();

	if (tp_procesos == NULL){
		log_error(logger, "Fallo creando tp_procesos");
		return 0;
	}

	//Se puede comentar despues, es para probar
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

//Imprimir vector de arrays de la memoria

void imprimir_bitarray(t_bitarray* marcosOcupadosPpal){

	int cantMarcosPpal = config->memory_size / config->page_size;
	printf("Imprimo bitarray para ver que hay ocupado\n");

	for (int i =0; i < cantMarcosPpal;i++){
		printf("%d", bitarray_test_bit(marcosOcupadosPpal,i));
	}

}

//FINALIZAR MEMORIA

void liberar_memoria(int conexionKernel, int conexionCPU, t_log* logger, t_config_memoria* config){

	//Por ultimo liberamos conexion, log y config
	log_info(logger, "Finalizando memoria :(");


	//Libero memoria paginacion
	//liberarMemoriaPaginacion();

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



//-----------------------------SWAP-----------------------------

void iniciarSwap(){

 //TODO: Ver si tengo que iniciarlo

}


/*
 *  Funcion: existe_archivo
 *  Entradas: 	char* path	le paso un directorio
 *  Salidas: int	ret me devuelve un entero (1) si el archivo existe
 *  Razon: 	Verificar si el archivo existe para no tener que crearlo.
 */

int existe_archivo(char* path){

	int ret = 0;

	int f = open(path, O_RDONLY);

	if (f != -1){ //si f no es -1, el archivo existe
		ret = 1;
		close(f);
	}

	return ret;
}


/*
 *  Funcion: crear_archivo_swap
 *  Entradas: 	int pid	le paso un pid (id del proceso)
 *  Salidas: void La devolucion de esta funcion es la creacion del archivo con
 *  el pid + .swap
 *  Razon: 	Cuando se inicializa un proceso en memoria, hay que crear su archivo en swap
 */
void crear_archivo_swap(int pid){ //archivo del tamanio del proceso

	//para guardar el pid
	char buffer[5];
	//lo paso a entero
	sprintf(buffer, "%d",pid);

	//Copio el nomrbe del proceso y el string .swap
	char* nombreArchivo = strcpy(buffer, ".swap");
	char* pathArchivo = strcpy(config->path_swap, strcpy("/", nombreArchivo));
	//Verifico si el archivo no existe

	if(!existe_archivo(pathArchivo)){


		FILE* archivo = fopen(nombreArchivo, "wt");

		log_info(logger, "Se creo el archivo %s", nombreArchivo);
		log_info(logger, " del proceso %d", pid);

		//completar con info de memoria
	}
}


/*
 *  Funcion: eliminar_archivo_swap
 *  Entradas: 	int pid	le paso un pid (id del proceso)
 *  Salidas: void La devolucion de esta funcion es la eliminacion del archivo con
 *  el pid + .swap
 *  Razon: 	Cuando se finaliza un proceso en memoria, hay que eliminar su archivo en swap.
 *  Para eso valido si existe, si es así lo borro, de lo contrario no hago nada.
 */

void eliminar_archivo_swap(int pid){

	//para guardar el pid
	char buffer[5];
	//lo paso a entero
	sprintf(buffer, "%d",pid);

	//hago la concatenación
	char* nombreArchivo = strcpy(buffer, ".swap");
	char* pathArchivo = strcpy(config->path_swap, strcpy("/", nombreArchivo));

	if(existe_archivo(pathArchivo)){
		if(remove(nombreArchivo)){
			log_info(logger, "Se eliminó el archivo %s", nombreArchivo);
		}
		else
			log_info(logger, "Ocurrio un error al eliminar el archivo %s", nombreArchivo);
	}

	else
		log_info(logger, "No se encontro el archivo %s", nombreArchivo);
}


/*
 *  Funcion: crear_archivo_swap
 *  Entradas: 	void No recibe ningun parametro
 *  Salidas: void La devolucion de esta funcion es un entero, que emula un delay
 *  Razon: 	Cuando se hacen operaciones entre memoria y el swap suele haber un tiempo
 *  de espera, con esta función lo emulamos.
 */

int retardo_swap(){

	return sleep(config->swap_time_delay);
}


//-----------------------------PAGINACION-----------------------------




//-----------------------------MENSAJES-----------------------------

void inicializar_proceso(int socket_cliente){


}

void finalizar_proceso(int socket_cliente){

	// 1) Buscar el proceso en memoria y libero el espacio utilizado
	// 2) Eliminar pcb
	// 3) Eliminar tablas? No es necesario para este tp
	// 4) Eliminar archivo de swap

	//recibir pid por mensaje
	//Pcb* pcb = obtener_pcb(, socket_cliente);
/*
	int puedeEliminar = iniciar_eliminacion_proceso(pcb->pid);

	if (puedeEliminar == 1){
		send(socket_cliente, OK, sizeof(uint8_t), 0);
		log_info(logger, "Se finalizo el proceso ok");
	}
	else{
		log_error(logger, "Ocurrio un error al finalizar el proceso, lola");
		send(socket_cliente, FAIL, sizeof(uint8_t), 0);
	}

*/

}

int iniciar_eliminacion_proceso(int pid){

	/*//Busco el proceso en la lista de espacio utilizado y lo elimino de la lista

	int cantidadProcesos = list_size(espacioUtilizado);

	for (int i = 0; i < cantidadProcesos; i++){
	proceso = list_get(espacioUtilizado,i);

	if (proceso->pid == pid){
		limpiar_posiciones(espacio, proceso->posPagina, proceso->cantidadDePaginas);
		list_remove_and_destroy_element(espacioUtilizado,i, free);
		eliminar_proceso(unaTabla);
		log_info(logger, "El proceso %d - Pagina inicial:%d Tamanio:%d fue eliminado correctamente", pid, proceso->posPagina, proceso->cantidadDePaginas);
	break;//?
	}

}

	eliminar_archivo_swap(pid);
*/
	return 0;
}


void suspender_proceso(int socket_cliente){

	//TODO

}

void acceder_tabla_de_paginas(int socket_cliente){
	//TODO
}

void acceder_espacio_de_usuario(int socket_cliente){
	//TODO
}


