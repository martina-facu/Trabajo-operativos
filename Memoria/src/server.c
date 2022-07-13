#include "server.h"

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
				log_error(logger, "MEMORIA: Error ejecutar el select %d", errno);
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
	socket = iniciar_servidor(ipServer, portServer, logger);
	log_info(logger, "MEMORIA: Socket en el que se levanta el server de Memoria: %d", socket);

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
	temporalAcceptedConnection = esperar_cliente(socketAnalizar, logger);
	log_info(logger, "MEMORIA: Se acepto temporalmente conexion en el descriptor: %d para evaluar la misma", temporalAcceptedConnection);

	//	Recibo el tipo de mensaja para saber si es Kernel o CPU
	uint8_t mensaje = 0;
	recv(temporalAcceptedConnection, &mensaje, sizeof(uint8_t), 0);
	log_info(logger, "MEMORIA: Mensaje recibido en el server Memoria para aceptar conexiones: %d", mensaje);

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
			log_info(logger,"MEMORIA: El mensaje recibido no corresponde a ninguno de los preestablecidos: %d", mensaje);
			log_info(logger,"MEMORIA: Procedo a cerrar sesion temporal establecida en el descriptor: %d", temporalAcceptedConnection);
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
	log_info(logger, "MEMORIA-KERNEL: Se acepto la conexion del Kernel en el socket: %d", acceptedConecctionKernel);
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
	log_info(logger, "MEMORIA-KERNEL: Se agrego al set de descriptores el descriptor: %d", acceptedConecctionKernel);

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


	//	Acepto la conexion del CPU
	acceptedConecctionCPU = temporalAcceptedConnection;
	log_info(logger, "MEMORIA-CPU: Se acepto la conexion del CPU en el socket: %d", acceptedConecctionCPU);
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
	log_info(logger, "MEMORIA-CPU: Se agrego al set de descriptores el descriptor: %d", acceptedConecctionCPU);

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
	log_info(logger, "MEMORIA-KERNEL: Mensaje recibido en el server Memoria para aceptar conexiones: %d", mensaje);

	switch(mensaje){

				case INICIALIZAR_PROCESO: //KERNEL
					log_info(logger, "MEMORIA-KERNEL Voy a iniciar proceso");
					inicializar_proceso(acceptedConecctionKernel);
				break;

				case FINALIZAR_PROCESO: //KERNEL
					finalizar_proceso(acceptedConecctionKernel);
				break;
				case SUSPENDER_PROCESO: //KERNEL
					suspender_proceso(acceptedConecctionKernel);
				break;
				default:
					log_info(logger,"MEMORIA-KERNEL: El mensaje recibido no corresponde a ninguno de los preestablecidos: %d", mensaje);
					log_info(logger,"MEMORIA-KERNEL: Procedo a cerrar sesion temporal establecida en el descriptor: %d", acceptedConecctionKernel);
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
	recv(acceptedConecctionCPU, &mensaje, sizeof(uint8_t), 0);
	log_info(logger, "MEMORIA-CPU: Mensaje recibido en el server Memoria para aceptar conexiones: %d", mensaje);

	switch(mensaje){
					case ENTRADAS_Y_TAMANIO_DE_PAGINA:
						entradas_y_tamanio_de_pagina(acceptedConecctionCPU);
						break;

					break;
					case ID_TABLA_SEGUNDO_NIVEL:
						devolver_numero_tabla_segundo_nivel(acceptedConecctionCPU);
					break;
					case MARCO_DE_LA_ENTRADA:
						devolver_numero_marco_asociado(acceptedConecctionCPU);
					break;
					case RESULTADO_LECTURA:
						devolver_lectura(acceptedConecctionCPU);
					break;
					case RESULTADO_ESCRITURA:
						devolver_escritura(acceptedConecctionCPU);
					break;
					default:
						log_info(logger,"MEMORIA-CPU: El mensaje recibido no corresponde a ninguno de los preestablecidos: %d", mensaje);
						log_info(logger,"MEMORIA-CPU: Procedo a cerrar sesion temporal establecida en el descriptor: %d", acceptedConecctionKernel);
						close(acceptedConecctionKernel);
					break;
				}

		return acceptedConecctionCPU; //ESTO ESTA BIEN HERNAN?
}


void liberar_conexion(int socket_cliente){

	close(socket_cliente);
}

pcb_t* recibir_paquete_pcb_kernel()
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	t_buffer* buffer = malloc(sizeof(t_buffer));
	recv(acceptedConecctionKernel,&paquete->codigo_operacion,sizeof(uint8_t),0);
	recv(acceptedConecctionKernel,&buffer->size,sizeof(uint32_t),0);
	buffer->stream = malloc(buffer->size);
	recv(acceptedConecctionKernel,buffer->stream,buffer->size,0);
	pcb_t* pcb = pcb_deserializar(buffer);
	pcb_mostrar(pcb, logger);
	log_trace(logger,"MEMORIA-KERNEL: recibi un proceso ID: %d ",pcb->pid);
	return pcb;
}

t_paquete* recibir_mensaje_cpu(int socket_cliente, t_log* logger)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->buffer = malloc(sizeof(t_buffer));
	t_buffer* buffer = paquete->buffer;

	//recibimos el codigo del tipo de mensaje que nos llega
	recv(socket_cliente, &(paquete->codigo_operacion), sizeof(uint8_t), 0);

	//recibo el tamaño del paquete
	recv(socket_cliente, &(buffer->size), sizeof(uint32_t), 0);

	//recibo el buffer con el pcb
	buffer->stream = malloc(buffer->size);
	recv(socket_cliente, buffer->stream, buffer->size, 0);

	return paquete;
}

void mandar_lecto_escritura(uint32_t direccion, uint32_t* valor, uint8_t operacion, int conexion)
{
	void* stream;
	int tamano_mensaje = 0;

	if(operacion == 9){
		tamano_mensaje = sizeof(uint32_t);
		stream = malloc(tamano_mensaje);
		memcpy(stream, &direccion, sizeof(uint32_t));
	}else if(operacion == 11){
		tamano_mensaje = sizeof(uint32_t)*2;
		stream = malloc(tamano_mensaje);
		memcpy(stream, &direccion, sizeof(uint32_t));
		memcpy(stream + sizeof(uint32_t), valor, sizeof(uint32_t));
	}

	t_buffer* buffer = armar_buffer(tamano_mensaje, stream);
	t_paquete* paquete = empaquetar_buffer(buffer,9);

	void* a_enviar = malloc(paquete->size);
	a_enviar = serializar_paquete(paquete, a_enviar);

	send(conexion, a_enviar, paquete->size, 0);
}

pcb_t* recibirPCB(int socket)
{

	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->buffer = malloc(sizeof(t_buffer));
	t_buffer* buffer = paquete->buffer;
	//recibimos el codigo del tipo de mensaje que nos llega
	recv(socket, &(paquete->codigo_operacion), sizeof(uint32_t), 0);

	//recibo el tamaño del paquete
	recv(socket, &(buffer->size), sizeof(uint32_t), 0);

	//recibo el buffer con el pcb
	buffer->stream = malloc(buffer->size);
	recv(socket, buffer->stream, buffer->size, 0);

	pcb_t* pcb = pcb_deserializar(buffer);

	return pcb;
}
