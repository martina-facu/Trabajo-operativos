#include "utils.h"
#include "CPU.h"




void ejecutar_ciclo_instrucciones(pcb_t* pcb,t_config* config, bool* devolver_pcb) {
	t_list* instrucciones = pcb->instrucciones;
	uint32_t program_counter = pcb->program_counter;

	//fetch
	Instruccion* instruccion = list_get(instrucciones,program_counter);
	program_counter++;
	pcb->program_counter = program_counter;

	//decode
	bool requiere_fetch_operands = false;
	if (instruccion->id == 4) {
		requiere_fetch_operands = true;
	}

	//fetch_operands
	if (requiere_fetch_operands) {
		// TODO: Ir a buscar los fetch operands, seria solo en la instruccion copy, va a ser una llamada a memoria
		printf("\nES UNA INSTRUCCION COPY");
		printf("\n");
	}

	//execute
	*devolver_pcb = execute(instruccion,configuracion->retardoNoOp,pcb);

	//check interrupt
	if (*devolver_pcb == false) {
		*devolver_pcb = interrupcion;
	}
}

t_config_cpu* crearConfigCPU(void)
{
	t_config_cpu* config = malloc(sizeof(t_config_cpu));
	return config;
}

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
			acceptedConecctionDispatch = esperar_cliente(socketAnalizar, logger);
			log_info(logger, "Se acepto la conexion del Dispatch en el socket: %d", acceptedConecctionDispatch);

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
			log_info(logger, "Mensaje recibido dispatch: %d", mensaje);

			if(mensaje == INICIAR_CONEXION_DISPATCH)
			{
				//	Defino y envio Handshake
				uint8_t handshake = ACEPTAR_CONEXION_DISPATCH;
				send(acceptedConecctionDispatch, &handshake, sizeof(uint8_t), 0);
				log_info(logger, "Se envia Handshake %d para dejar establecida la conexion del Dispatch", mensaje);
				//	Agrego el descrilptor al maestro
				FD_SET(acceptedConecctionDispatch, &master_fd_set);
				//	Incremento el grado de concurrencia
				connectionsDispatch++;
			}
			else
			{
				//	Como el mensaje es incorrecto desestimo el mensaje recibido.
				log_info(logger, "Mensaje recibido del Dispatch es incorrecto, se desestima el mismo");
				close(acceptedConecctionDispatch);
			}
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
		log_info(logger, "Se quiso conectar alguien al Server Interrupt");
		//	Si el grado de concurrencia admite que se sigan aceptando
		//	conexiones, la acepto. Sino omito lo recibido.
		if(connectionsInterrupt < CONCURRENT_CONNECTION)
		{
			//	Acepto la conexion del cliente que se conecta
			acceptedConecctionInterrupt = esperar_cliente(socketAnalizar, logger);
			log_info(logger, "Se acepto la conexion del Interrupt en el socket: %d", acceptedConecctionDispatch);

			//	Defino el mensaje a recibir (y lo recibo) del cliente cuando se conecta
			uint8_t mensaje = 0;
			recv(acceptedConecctionInterrupt, &mensaje, sizeof(uint8_t), 0);
			log_info("Mensaje recibido interrupt: %d", mensaje);

			if(mensaje == INICIAR_CONEXION_INTERRUPT)
			{
				//	Recibi el mensaje de conexion de un Kernel Interrupt
				//	Defino para finalizar la aceptacion de la conexion
				uint8_t handshake = ACEPTAR_CONEXION_INTERRUPT;
				//	Envio el mensaje
				send(acceptedConecctionInterrupt, &handshake, sizeof(uint8_t), 0);
				//	Levanto el nivel de concurrencia activa de Interrupt
				log_info(logger, "Se envia Handshake %d para dejar establecida la conexion del Dispatch", mensaje);
				connectionsInterrupt++;

				//	Inicializo los atributos del thread a crear.
				resThread = pthread_attr_init(&attr);
				if (resThread != 0)
					log_info(logger,"Error al inicializar pthread_attr_init");
					//handle_error_en(resThread, "Error al inicializar pthread_attr_init");
//			   struct thread_info *tinfo = calloc(num_threads_interrupt, sizeof(*tinfo));
//			   if (tinfo == NULL)
//				   handle_error("Error al alocar la memoria para la informacion de los thread de interrupt");
				resThread = pthread_create(&threadId, &attr, &atencionInterrupt, (void *) acceptedConecctionInterrupt);
				if (resThread != 0)
					log_info(logger,"Error al crear el Thread");
				else
					log_info(logger, "Se genera el Thread del Interrupt con TID: %d", resThread);


			}
			else
			{
				//	El mensaje recibido no es lo esperado
				//	Se procede a cerrar la conexion establecida
				close(acceptedConecctionInterrupt);
			}
		}
	}
}



t_config_cpu* cargarConfiguracion(char* configPath)
{

	t_config* configFile = config_create(configPath);
	t_config_cpu* configTemp = crearConfigCPU();

	configTemp->entradasTLB = config_get_int_value(configFile, ENTRADAS_TLB);
		log_info(logger, "Se leyo la variable ENTRADAS_TLB: %d", configTemp->entradasTLB);
	configTemp->algoritmoReemplazoTLB = config_get_string_value(configFile, ALG_TLB);
		log_info(logger, "Se leyo la variable REEMPLAZO_TLB: %s", configTemp->algoritmoReemplazoTLB);
	configTemp->retardoNoOp = config_get_int_value(configFile, RETARDO_NOOP);
		log_info(logger, "Se leyo la variable RETARDO_NOOP: %d", configTemp->retardoNoOp);
	configTemp->IPCPU = config_get_string_value(configFile, IP_CPU);
		log_info(logger, "Se leyo la variable IP_CPU: %s", configTemp->IPCPU);
	configTemp->IPMemoria = config_get_string_value(configFile, IP_MEMORIA);
		log_info(logger, "Se leyo la variable IP_MEMORIA: %s", configTemp->IPMemoria);
	configTemp->puertoMemoria = config_get_string_value(configFile, PUERTO_MEMORIA);
		log_info(logger, "Se leyo la variable PUERTO_MEMORIA: %s", configTemp->puertoMemoria);
	configTemp->puertoDispatch = config_get_string_value(configFile, PUERTO_DISPATCH);
		log_info(logger, "Se leyo la variable PUERTO_DISPATCH: %s", configTemp->puertoDispatch);
	configTemp->puertoInterrupt = config_get_string_value(configFile, PUERTO_INTERRUPT);
		log_info(logger, "Se leyo la variable PUERTO_INTERRUPT: %s", configTemp->puertoInterrupt);

	return configTemp;
}

int main(void)
{
	config = config_create("cpu.config");
	uint32_t cantidad_entradas, tamano_pagina = 0;
	devolver_pcb = false;
	recibiPCB = false;


	logger = initLogger("cpu.log", "CPU", LOG_LEVEL_INFO);
	//	Cargo archivo de config
	configuracion = cargarConfiguracion("cpu.config");
	//	Seteo en 0 a los set de descriptores a revisar por el select

	FD_ZERO(&read_fd_set);
	FD_ZERO(&master_fd_set);

//	Iniciar conexiones
	int conexion_memoria = levantar_conexion_memoria(configuracion->IPMemoria, configuracion->puertoMemoria, logger, &cantidad_entradas,&tamano_pagina);
	//	Marco el descriptor en donde me conecte al server de memoria como limite maximo y minimo del select
	fdmax = conexion_memoria;
	fdmin = conexion_memoria;

	//	Levanto el server para el DISPATCH
	int kernel_dispatch = levantar_server(configuracion->IPCPU, configuracion->puertoDispatch, logger, SERVER_DISPATCH);
	//	Verifico si el descriptor es mayor o menor al maximo o al minimo del select
	//	Maximo
	if( fdmax < kernel_dispatch)
		fdmax = kernel_dispatch;
	//	Minimo
	if( kernel_dispatch < fdmin)
		fdmin = kernel_dispatch;
	//	Agrego el descriptor del server de DISPATCH al maestro del select
	FD_SET(kernel_dispatch, &master_fd_set);

	int kernel_interrupt = levantar_server(configuracion->IPCPU, configuracion->puertoInterrupt, logger, SERVER_INTERRUPT);
	//	Verifico si el descriptor es mayor o menor al maximo o al minimo del select
	//	Maximo
	if( fdmax < kernel_interrupt)
		fdmax = kernel_interrupt;
	//	Minimo
	if(kernel_interrupt < fdmin)
		fdmin = kernel_interrupt;
	//	Agrego el descriptor del server de DISPATCH al maestro del select
	FD_SET(kernel_interrupt, &master_fd_set);

	//	Comienzo con el ciclo de comunicaciones y ejecuciones
	while(1)
	{
		/*
		 * 	A partir de aca empieza el codigo del CPU
		 *
		 * 	Primero voy con el select para ver si recibi el accept de un kernel
		 * 	y despues si recibi informacion del kernel en si
		 *
		 * 	Segundo proceso el PCB que recibi
		 *
		 * 	La interrupcion se maneja con un thread independiente
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

					//	Si el descriptor a revisar es el del server de Dispatch
					//	valido y de corresponder acepto la conexion nueva
					if(i == kernel_dispatch)
						aceptoServerDispatch(i);
					//	Si el descriptor a revisar es el del server de Interrupt
					//	valido y de corresponder acepto la conexion nueva
					else if(i == kernel_interrupt)
						aceptoServerInterrupt(i);
					//	Si el descriptor a revisar es la comunicacion aceptada
					//	con el Dispatch proceso la misma (recibo PCB
					else if(i == acceptedConecctionDispatch)
					{
						//	Recibir pcb del kernel
						//	REVISAR EL PRIMER PARAMETRO PORQUE NO SE USA Y NO SERIA NECESARIO
						pcb = obtener_pcb(acceptedConecctionDispatch);
						pcb_mostrar(pcb, logger);
					}

				}
			}
		//	Termine de revisar las conexiones por lo que supongo que ya recibi un PCB
		//	Proceso lo recibido, es decir actuo como CPU
		//	Ejecutar ciclo de instrucciones

		if(recibiPCB == true)
		{
			while (devolver_pcb == false)
			{
				log_info(logger, "Entre a ejecutar instrucciones");
				ejecutar_ciclo_instrucciones(pcb,config,&devolver_pcb);
			}

			pcb_mostrar(pcb, logger);

			//	DEVOLVER PCB AL KERNEL
			uint32_t* tamano_mensaje = malloc(sizeof(uint32_t));
			void* a_enviar = pcb_serializar(pcb,tamano_mensaje,1);
			send(acceptedConecctionDispatch, a_enviar, *tamano_mensaje, 0);
			recibiPCB = false;

		}



		//	Vuelvo a iniciar el proceso del While
	}

	log_info(logger, "Sali del while infinito y voy a cerrar las conexiones generadas");

//	Cierro conexiones
	close(conexion_memoria);
	close(socket_dispatch);
	close(kernel_dispatch);
	close(socket_interrupt);
	close(acceptedConecctionDispatch);
	close(acceptedConecctionInterrupt);

	return EXIT_SUCCESS;
}


void * atencionInterrupt(void * socketInterrupt)
{
	int iSocketInterrupt = (int) socketInterrupt;

	while(1)
	{
		//	Defino el mensaje a recibir del Kernel Interrupt
		uint8_t mensaje = 0;
		recv(iSocketInterrupt, &mensaje, sizeof(uint8_t), 0);
		log_info("Mensaje recibido interrupt: %d", mensaje);
		if(mensaje == 25)
		{
			//	Como el mensaje es correcto seteo la variable para que el CPU devuelva el PCB
			interrupcion = true;
		}
		else
		{
			//	Como el mensaje es incorrecto desestimo el mensaje recibido.
			log_info("Mensaje recibido del interrupt %d es incorrecto, se desestima el mismo", mensaje);
		}


	}


    return NULL;
}
