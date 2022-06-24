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
				{
					//	Verifico si se recibio informacion proveniente del Kernel
					if (FD_ISSET(acceptedConecctionKernel, &read_fd_set))
					{
						/*
						 *	Aca va la logica de mensajes con el kernel
						 */

					}
				}
				if(i == acceptedConecctionCPU)
				{
					//	Verifico si se recibio informacion proveniente del Kernel
					if (FD_ISSET(acceptedConecctionCPU, &read_fd_set))
					{
						/*
						 *	Aca va la logica de mensajes con la CPU
						 */

					}
				}
			}
		}
	}
	return EXIT_SUCCESS;
}


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
		case 1:
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
	handshake = 42;
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
	handshake = 2;
	send(acceptedConecctionCPU, &handshake, sizeof(uint8_t), 0);


	//	Incremento el grado de concurrencia actual del Kernel
	connectionsCPU++;

	return acceptedConecctionCPU;
}


t_config_memoria* crearConfigMemoria(void)
{
	t_config_memoria* config = malloc(sizeof(t_config_memoria));
	return config;
}
