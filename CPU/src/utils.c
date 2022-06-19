#include "utils.h"

Pcb* obtener_pcb(int cliente)
{

	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->buffer = malloc(sizeof(t_buffer));
	t_buffer* buffer = paquete->buffer;

	//recibimos el codigo del tipo de mensaje que nos llega
	recv(cliente, &(paquete->codigo_operacion), sizeof(uint8_t), 0);

	//recibo el tamaño del paquete
	recv(cliente, &(buffer->size), sizeof(uint32_t), 0);

	//recibo el buffer con el pcb
	buffer->stream = malloc(buffer->size);
	recv(cliente, buffer->stream, buffer->size, 0);

	Pcb* pcb = pcb_deserializar(buffer);

	return pcb;
}


int levantar_conexion_memoria(t_config* config, uint32_t* cantidad_entradas,uint32_t* tamano_pagina)
{
	char * ip_memoria = malloc(sizeof(char) * 30);
	strcpy(ip_memoria, config_get_string_value(config, "IP_MEMORIA"));
	printf("\nIp de la memoria: %s", ip_memoria);

	char* puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
	int conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);

	uint8_t handshake_memoria = 8;
	send(conexion_memoria, &handshake_memoria, sizeof(uint8_t), 0);

	uint8_t respuesta_memoria = 0;
	recv(conexion_memoria, &respuesta_memoria, sizeof(uint8_t), 0);
	printf("\nMensaje recibido de la memoria: %d", respuesta_memoria);

//	TODO: descomentar estas lineas cuando se implemente la parte de la memoria
//	recv(conexion_memoria, cantidad_entradas, sizeof(uint32_t), 0);
//	recv(conexion_memoria, tamano_pagina, sizeof(uint32_t), 0);

	return conexion_memoria;
}

/*
 *  Funcion: thread_dispatch
 *  Entradas: void *fdConsola
 *  Salidas: void
 *  Razon: 	Funcion para la recepcion de la informacion de la consola que se conecto
 *  		y luego informarle a la misma cuando finalizo su proceso.
 *  		¡¡¡¡¡ Hay que tener cuidado cuando se toque la lista de PCB existentes !!!!!
 *  Author: Grupo 99
 */

/* Process to receive client message function */
//void *thread_dispatch(void *parameter)
//{
////	int nbytes;
//	int *fdServerDispatch;
////	char *msg;
//	//	Inicializo en cero el contador de conexiones concurrentes
//	int connection = 0;
//
//	//	Leo los parametros recibidos al momento de creacion del Thread
//	fdServerDispatch = (int *) parameter;
//
//	//	Acepto la conexion del cliente que se conecta
//	//	ESTO TIENE QUE IR DESPUES EN EL THREAD!!!!!!!!!!!!!!!!!!
//	fdDispatch = esperar_cliente(*fdServerDispatch);
//	log_info(logger, "Acepto un Dispatch Kernel en: %d", fdDispatch);
//
//	while (true)
//	{
//		 // Valido si llegue al limite de concurrencia en el Dispatch
//		 if (connection < CONCURRENT_CONNECTION)
//		 {
//			//	Acepto la conexion del cliente que se conecta
//			//	ESTO TIENE QUE IR DESPUES EN EL THREAD!!!!!!!!!!!!!!!!!!
//			fdDispatch = esperar_cliente(*socket);
//			log_info(logger, "Acepto un nuevo Dispatch Kernel en: %d", fdDispatch);
//		 }
//		 else
//		 {
//				//	Defino el mensaje a recibir (y lo recibo) del cliente cuando se conecta
//				uint8_t mensaje = 0;
//				recv(fdDispatch, &mensaje, sizeof(uint8_t), 0);
//				log_info("Mensaje recibido dispatch: %d", mensaje);
//
//				//	Defino y envio Handshake
//				uint8_t handshake = 4;
//				send(fdDispatch, &handshake, sizeof(uint8_t), 0);
//		 }
//
//
//
//
//	}
//
//
//
//
//
//
//	printf("\nSe muere el thread\n");
//}
//



/*
 *  Funcion: aceptoServerDispatch
 *  Entradas: 	int socketAnalizar		socket que se esta analizando en el select
 *  Salidas: void
 *  Razon: 	De corresponder acepto la conexion al Dispatch
 *  Autor:
 */
void aceptoServerDispatch(int socketAnalizar)
{
	//	Verifico si se recibio informacion en este descriptor
	if (FD_ISSET(socketAnalizar, &read_fd_set))
	{
		//	Si el grado de concurrencia admite que se sigan aceptando
		//	conexiones, la acepto. Sino omito lo recibido.
		if(connectionsDispatch < CONCURRENT_CONNECTION)
		{
			//	Acepto la conexion del cliente que se conecta
			//	ESTO TIENE QUE IR DESPUES EN EL THREAD!!!!!!!!!!!!!!!!!!
			acceptedConecctionDispatch = esperar_cliente(socketAnalizar);
			log_info(logger, "Se acepto la conexion del Dispatch en el socket: %d", acceptedConecctionDispatch);

			//	Agrego el descrilptor al maestro
			FD_SET(acceptedConecctionDispatch, &master_fd_set);
			//	Valido si tengo que cambiar el maximo o el minimo
			//	Maximo
			if (acceptedConecctionDispatch > fdmax)
			{
				fdmax = acceptedConecctionDispatch;
			}
			//	Minimo
			if (acceptedConecctionDispatch < fdmin)
			{
				fdmin = acceptedConecctionDispatch;
			}
			log_info(logger, "Se agrego al set de descriptores el descriptor: %d", acceptedConecctionDispatch);


			//	Defino el mensaje a recibir (y lo recibo) del cliente cuando se conecta
			uint8_t mensaje = 0;
			recv(acceptedConecctionDispatch, &mensaje, sizeof(uint8_t), 0);
			log_info("Mensaje recibido dispatch: %d", mensaje);

			//	Defino y envio Handshake
			uint8_t handshake = 4;
			send(acceptedConecctionDispatch, &handshake, sizeof(uint8_t), 0);
			connectionsDispatch++;
		}
	}
}
/*
 *  Funcion: aceptoServerInterrupt
 *  Entradas: 	int socketAnalizar		socket que se esta analizando en el select
 *  Salidas: void
 *  Razon: 	De corresponder acepto la conexion al Dispatch y genero Thread de atencion al mismo
 *  Autor:
 */
void aceptoServerInterrupt(int socketAnalizar)
{
	//	Verifico si se recibio informacion en este descriptor
	if (FD_ISSET(socketAnalizar, &read_fd_set))
	{
		//	Si el grado de concurrencia admite que se sigan aceptando
		//	conexiones, la acepto. Sino omito lo recibido.
		if(connectionsDispatch < CONCURRENT_CONNECTION)
		{
			//	Acepto la conexion del cliente que se conecta
			acceptedConecctionInterrupt = esperar_cliente(socketAnalizar);
			log_info(logger, "Se acepto la conexion del Interrupt en el socket: %d", acceptedConecctionDispatch);

			//	Agrego el descrilptor al maestro
			FD_SET(acceptedConecctionInterrupt, &master_fd_set);
			//	Valido si tengo que cambiar el maximo o el minimo
			//	Maximo
			if (acceptedConecctionInterrupt > fdmax)
			{
				fdmax = acceptedConecctionInterrupt;
			}
			//	Minimo
			if (acceptedConecctionInterrupt < fdmin)
			{
				fdmin = acceptedConecctionInterrupt;
			}
			log_info(logger, "Se agrego al set de descriptores el descriptor: %d", acceptedConecctionInterrupt);


			//	Defino el mensaje a recibir (y lo recibo) del cliente cuando se conecta
			uint8_t mensaje = 0;
			recv(acceptedConecctionInterrupt, &mensaje, sizeof(uint8_t), 0);
			log_info("Mensaje recibido interrupt: %d", mensaje);

			//	Defino y envio Handshake
			uint8_t handshake = 5;
			send(acceptedConecctionInterrupt, &handshake, sizeof(uint8_t), 0);
			connectionsInterrupt++;

			/*
			 * TENGO QUE CREAR EL THREAD DE ATENCION AL INTERRUPT
			 */





		}
	}
}
/*
 *  Funcion: levantar_server
 *  Entradas: 	t_config* config		Archivo de configuracion
 *  			int* socket				Socket
 *  			char* sTipo				Tipo de server a levantar
 *  Salidas: int	socket donde se levanto el server
 *  Razon: 	generar un socket server
 *  Autor:
 */

//int levantar_server(t_config* config, int* socket, char* sTipo)
//{
//	//	Leo el puerto en el que voy a levantar el server de acuerdo al TIPO
//	//	de server que indique en la funcion
//	char* puerto = config_get_string_value(config, sTipo);
//	log_info(logger, "Puerto escucha %s", puerto);
//
//	//	Leo la IP sobre la cual me voy a levantar como server
//	char* ipCpu = config_get_string_value(config, CPU_IP);
//	log_info(logger, "La direccion IP del CPU es %s", ipCpu);
//
//	//	Inicio el servidor en la IP y puertos leidos desde el archivo de configuracion
//	*socket = iniciar_servidor(ipCpu, puerto);
//	log_info(logger, "Socket %d", *socket);
//
//
//
//	//	Retorno el socket del cliente creado
//	//	ESTO CAMBIA POR EL SOCKET DEL SERVER CUANDO ENTREN LOS THREADS
//	return cliente1;
//}

int levantar_server(t_config* config, char* sTipo)
{
	int socket;

	//	Leo el puerto en el que voy a levantar el server de acuerdo al TIPO
	//	de server que indique en la funcion
	char* puerto = config_get_string_value(config, sTipo);
	log_info(logger, "Puerto escucha %s", puerto);

	//	Leo la IP sobre la cual me voy a levantar como server
	char* ipCpu = config_get_string_value(config, CPU_IP);
	log_info(logger, "La direccion IP del CPU es %s", ipCpu);

	//	Inicio el servidor en la IP y puertos leidos desde el archivo de configuracion
	socket = iniciar_servidor(ipCpu, puerto);
	log_info(logger, "Socket %d", socket);

	return socket;
}
//int levantar_server(t_config* config, int* socket, char* sTipo)
//{
//	//	Leo el puerto en el que voy a levantar el server de acuerdo al TIPO
//	//	de server que indique en la funcion
//	char* puerto = config_get_string_value(config, sTipo);
//	log_info(logger, "Puerto escucha %s", puerto);
//
//	//	Leo la IP sobre la cual me voy a levantar como server
//	char* ipCpu = config_get_string_value(config, CPU_IP);
//	log_info(logger, "La direccion IP del CPU es %s", ipCpu);
//
//	//	Inicio el servidor en la IP y puertos leidos desde el archivo de configuracion
//	*socket = iniciar_servidor(ipCpu, puerto);
//	log_info(logger, "Socket %d", *socket);
//
//	//	Acepto la conexion del cliente que se conecta
//	//	ESTO TIENE QUE IR DESPUES EN EL THREAD!!!!!!!!!!!!!!!!!!
//	int cliente1 = esperar_cliente(*socket);
//	log_info(logger, "Cliente: %d", cliente1);
//
//	//	Defino el mensaje a recibir (y lo recibo) del cliente cuando se conecta
//	uint8_t mensaje = 0;
//	recv(cliente1, &mensaje, sizeof(uint8_t), 0);
//	log_info("Mensaje recibido dispatch: %d", mensaje);
//
//	//	Defino y envio Handshake
//	uint8_t handshake = 4;
//	send(cliente1, &handshake, sizeof(uint8_t), 0);
//
//	//	Retorno el socket del cliente creado
//	//	ESTO CAMBIA POR EL SOCKET DEL SERVER CUANDO ENTREN LOS THREADS
//	return cliente1;
//}

int levantar_canal_dispatch(t_config* config, int* socket_dispatch) {
	char* puerto_dispatch = config_get_string_value(config,
			"PUERTO_ESCUCHA_DISPATCH");
	printf("\nPuerto escucha %s", puerto_dispatch);

	*socket_dispatch = iniciar_servidor("127.0.0.1", puerto_dispatch);
	printf("\nSocket %d", *socket_dispatch);

	int cliente1 = esperar_cliente(*socket_dispatch);
	printf("\nCliente: %d", cliente1);
	printf("\n");

	uint8_t mensaje = 0;
	recv(cliente1, &mensaje, sizeof(uint8_t), 0);
	printf("Mensaje recibido dispatch: %d", mensaje);
	printf("\n");

	uint8_t handshake = 4;

	send(cliente1, &handshake, sizeof(uint8_t), 0);

	return cliente1;
}

int levantar_puerto_interrupt(t_config* config, int* socket_interrupt) {
	char* puerto_interrupt = config_get_string_value(config,
			"PUERTO_ESCUCHA_INTERRUPT");
	printf("\nPuerto interrupt %s", puerto_interrupt);

	*socket_interrupt = iniciar_servidor("127.0.0.1", puerto_interrupt);
	printf("\nSocket %d", *socket_interrupt);

	int cliente2 = esperar_cliente(*socket_interrupt);
	printf("\nCliente: %d", cliente2);

	uint8_t mensaje1 = 0;

	recv(cliente2, &mensaje1, sizeof(uint8_t), 0);
	printf("\nMensaje recibido interrupt: %d", mensaje1);
	printf("\n");

	uint8_t handshake1 = 5;
	send(cliente2, &handshake1, sizeof(uint8_t), 0);

	return cliente2;
}

bool execute(Instruccion* instruccion,t_config* config, Pcb* pcb) { // TODO : encapsular la logica de las instrucciones mas complicadas
	uint8_t id = instruccion->id;
	t_list* parametros = instruccion->parametros;

	int catidad_parametros = getCantidadParametros(id);
	int* parametro1,parametro2 = 0;

	if(catidad_parametros == 1){
		parametro1 = list_get(parametros,0);
	}else if(catidad_parametros >= 1){
		parametro1 = list_get(parametros,0);
		parametro2 = list_get(parametros,1);
	}

	int dormir = config_get_int_value(config,"RETARDO_NOOP");

	switch (id) {
	case 1:
		dormir = dormir/1000;
		sleep(dormir);
		return false;
		break;
	case 2:
		pcb->estado = BLOQUEADO;
		pcb->tiempo_bloqueo = *parametro1;
		return true;
		break;
	case 3: // TODO : Implementar la instruccion WRITE
		return false;
		break;
	case 4: // TODO : Implementar la instruccion COPY
		return false;
		break;
	case 5: // TODO : Implementar la instruccion READ
		return false;
		break;
	case 6:
		return true;
		break;
	default:
		printf("\nHUBO UN FALLO EN LA EJECUCION DE LAS INSTRUCCIONES");
		return true;
		break;
	}
}

