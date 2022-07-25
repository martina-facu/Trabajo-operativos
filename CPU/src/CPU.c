#include "utils.h"
#include "CPU.h"


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
		// conexiones, la acepto. Sino omito lo recibido.
		if(cantidad_clientes_dispatch < CONCURRENT_CONNECTION)
		{
			//	Acepto la conexion del cliente que se conecta
			cliente_dispatch = esperar_cliente(socketAnalizar, logger);
			log_trace(logger, "CPU-COMUNICACION-KERNEL Se acepto la conexion del Dispatch en el socket: %d", cliente_dispatch);

			//	Valido si tengo que cambiar el maximo o el minimo
			compararLimitesConNuevoDescriptor(cliente_dispatch);
			log_trace(logger, "CPU-COMUNICACION-KERNEL Se agrego al set de descriptores el descriptor: %d", cliente_dispatch);


			//	Defino el mensaje a recibir (y lo recibo) del cliente cuando se conecta
			uint8_t mensaje = 0;
			recv(cliente_dispatch, &mensaje, sizeof(uint8_t), 0);
			log_trace(logger, "CPU-COMUNICACION-KERNEL Mensaje recibido dispatch: %d", mensaje);

			if(mensaje == INICIAR_CONEXION_DISPATCH)
			{
				//	Defino y envio Handshake
				uint8_t handshake = ACEPTAR_CONEXION_DISPATCH;
				send(cliente_dispatch, &handshake, sizeof(uint8_t), 0);
				log_trace(logger, "CPU-COMUNICACION-KERNEL Se envia Handshake %d para dejar establecida la conexion del Dispatch", mensaje);
				//	Agrego el descrilptor al maestro
				FD_SET(cliente_dispatch, &master_fd_set);
				//	Incremento el grado de concurrencia
				cantidad_clientes_dispatch++;
			}
			else
			{
				//	Como el mensaje es incorrecto desestimo el mensaje recibido.
				log_trace(logger, "CPU-COMUNICACION-KERNEL Mensaje recibido del Dispatch es incorrecto, se desestima el mismo");
				close(cliente_dispatch);
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
		log_trace(logger, "CPU-COMUNICACION-KERNEL Se quiso conectar alguien al Server Interrupt");
		//	Si el grado de concurrencia admite que se sigan aceptando
		//	conexiones, la acepto. Sino omito lo recibido.
		if(cantidad_clientes_interrupt < CONCURRENT_CONNECTION)
		{
			//	Acepto la conexion del cliente que se conecta
			cliente_interrupt = esperar_cliente(socketAnalizar, logger);
			log_trace(logger, "CPU-COMUNICACION-KERNEL Se acepto la conexion del Interrupt en el socket: %d", cliente_interrupt);

			//	Defino el mensaje a recibir (y lo recibo) del cliente cuando se conecta
			uint8_t mensaje = 0;
			recv(cliente_interrupt, &mensaje, sizeof(uint8_t), 0);
			log_trace(logger,"CPU-COMUNICACION-KERNEL Mensaje recibido interrupt: %d", mensaje);

			if(mensaje == INICIAR_CONEXION_INTERRUPT)
			{
				//	Recibi el mensaje de conexion de un Kernel Interrupt
				//	Defino para finalizar la aceptacion de la conexion
				uint8_t handshake = ACEPTAR_CONEXION_INTERRUPT;
				//	Envio el mensaje
				send(cliente_interrupt, &handshake, sizeof(uint8_t), 0);
				//	Levanto el nivel de concurrencia activa de Interrupt
				log_trace(logger, "CPU-COMUNICACION-KERNEL Se envia Handshake %d para dejar establecida la conexion del Dispatch", mensaje);
				cantidad_clientes_interrupt++;

				//	Inicializo los atributos del thread a crear.
				resThread = pthread_attr_init(&attr);
				if (resThread != 0)
					log_error(logger,"CPU-COMUNICACION-KERNEL Error al inicializar pthread_attr_init");
				resThread = pthread_create(&threadId, &attr, &atencionInterrupt, (void *) cliente_interrupt);
				if (resThread != 0)
					log_error(logger,"CPU-COMUNICACION-KERNEL Error al crear el Thread");
				else
					log_trace(logger, "CPU-COMUNICACION-KERNEL Se genera el Thread del Interrupt con TID: %d", resThread);
				pthread_detach(resThread);

			}
			else
			{
				//	El mensaje recibido no es lo esperado
				//	Se procede a cerrar la conexion establecida
				close(cliente_interrupt);
			}
		}
	}
}

int main(void)
{
	config = config_create("cpu.config");
	uint32_t cantidad_entradas=0 , tamano_pagina = 0;
	devolver_pcb = false;
	recibiPCB = false;

//	logger = initLogger("cpu.log", "CPU", LOG_LEVEL_TRACE);
	//	Cargo archivo de config
	configuracion = cargarConfiguracion("cpu.config");
	//	Seteo en 0 a los set de descriptores a revisar por el select

	FD_ZERO(&read_fd_set);
	FD_ZERO(&master_fd_set);

//	Iniciar conexiones
	cliente_memoria = levantar_conexion_memoria_CPU(configuracion->IPMemoria, configuracion->puertoMemoria, &cantidad_entradas,&tamano_pagina);
	//	Marco el descriptor en donde me conecte al server de memoria como limite maximo y minimo del select
	fdmax = cliente_memoria;
	fdmin = cliente_memoria;

	//	Levanto el server para el DISPATCH
	int kernel_dispatch =  levantarServerDispatch();

	//	Levanto el server para el Interrupt
	int kernel_interrupt = levantarServerInterrupt();

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
			log_error(logger, "CPU-COMUNICACION-SELECT Error ejecutar el select %d", errno);
			break;
			//	Validar si la falla hace que tenga que matar la CPU
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
					else if(i == cliente_dispatch)
					{
						if (FD_ISSET(cliente_dispatch, &read_fd_set))
							reciboPCBdesdeKernel(cliente_dispatch);
					}
				}
			}
		//	Termine de revisar las conexiones por lo que supongo que ya recibi un PCB
		//	Proceso lo recibido, es decir actuo como CPU
		//	Ejecutar ciclo de instrucciones

		if(recibiPCB == true)
			procesarPCB();

		//	Vuelvo a iniciar el proceso del While
	}

	log_trace(logger, "CPU-COMUNICACION-SELECT Sali del while infinito y voy a cerrar las conexiones generadas");

//	Cierro conexiones
	close(cliente_memoria);
	close(socket_dispatch);
	close(kernel_dispatch);
	close(socket_interrupt);
	close(cliente_dispatch);
	close(cliente_interrupt);

	return EXIT_SUCCESS;
}

/*
 *  Funcion: reciboPCBdesdeKernel
 *  Entradas: 	int cliente_dispatch		Descriptor del Dispatch
 *  Salidas: void
 *  Razon: 	Recibo un PCB a procesar desde el Kernel
 *  Autor:
 */
void reciboPCBdesdeKernel(int acceptedConnectionDispatch)
{
	if(idAnteriorPCB == -1)
	{
		log_info(logger, "CPU-TLB Inicializo la tlb");
		tlb = list_create();
	}
	else if(pcb->pid != idAnteriorPCB){
		log_info(logger, "CPU-TLB Borro el contenido de la TLB ya que no es el mismo proceso que el anterior");
		limpiar_tlb(tlb);
	}


	if((cantidad_clientes_dispatch > 0) && (cantidad_clientes_interrupt > 0) )
	{
		pcb = recibirPCB(acceptedConnectionDispatch, logger);
		recibiPCB = true;
//		log_trace(logger, "CPU-KERNEL-PCB Voy a loguear informacion del PCB recibida por el Dispatch");
//		pcb_mostrar(pcb, logger);

		log_trace(logger, "CPU-EXECUTE PID DEL PROCESO ANTERIOR %d",idAnteriorPCB);

		if(idAnteriorPCB == -1){
			log_info(logger, "CPU-TLB Inicializo la tlb");
			tlb = list_create();
			inicializar_mmu(config,tlb,logger);
		} else if(pcb->pid != idAnteriorPCB){
			mostrar_entradas(tlb);
			log_info(logger, "CPU-TLB Borro el contenido de la TLB ya que no es el mismo proceso que el anterior");
			limpiar_tlb(tlb);
		}
	}

}

/*
 *  Funcion: procesarPCB
 *  Entradas: 	void
 *  Salidas: void
 *  Razon: 	Proceso las instrucciones del PCB
 *  Autor:
 */
void procesarPCB(void)
{
	log_trace(logger, "CPU-EXECUTE Se recibio un PCB y procedo a ejecutar el mismo");
	while (devolver_pcb == false)
		ejecutar_ciclo_instrucciones(pcb, &devolver_pcb, configuracion->retardoNoOp, cant, cliente_memoria,tam, &interrupcion);
//		ejecutar_ciclo_instrucciones(pcb,config,&devolver_pcb);


//	log_info(logger, "CPU-KERNEL-PCB Voy a mostrar como quedo el contenido del PCB luego de la ejecucion");
//	pcb_mostrar(pcb, logger);

	//	DEVOLVER PCB AL KERNEL
	uint32_t* tamano_mensaje = malloc(sizeof(uint32_t));
	log_trace(logger, "CPU-COMUNICACION-KERNEL Se arma el stream para devolver el PCB %d al Kernel", pcb->pid);
	log_trace(logger,"--------------------------------------------------------------------ESTADO: %d", pcb->estado);
	void* a_enviar = pcb_serializar(pcb,tamano_mensaje,1);
	send(cliente_dispatch, a_enviar, *tamano_mensaje, 0);
	log_info(logger, "CPU-COMUNICACION-KERNEL Se devuelve el PCB %d al Kernel", pcb->pid);
	interrupcion=false;
	devolver_pcb = false;
	recibiPCB = false;
	idAnteriorPCB = pcb->pid;
	log_trace(logger, "CPU-COMUNICACION-KERNEL Seteo el flag para poder volver a recibir otro PCB del Kernel");

}


/*
 *  Funcion: levantarServerDispatch
 *  Entradas: 	void
 *  Salidas: void
 *  Razon: 	Se levanta el server para crear comunicacion con un Dispatch
 *  Autor:
 */


int levantarServerDispatch()
{
	//	Levanto el server para el DISPATCH
	int kernel_dispatch = levantar_server(configuracion->IPCPU, configuracion->puertoDispatch, SERVER_DISPATCH);
	//	Verifico si el descriptor es mayor o menor al maximo o al minimo del select
	compararLimitesConNuevoDescriptor(kernel_dispatch);
	//	Agrego el descriptor del server de DISPATCH al maestro del select
	FD_SET(kernel_dispatch, &master_fd_set);
	return kernel_dispatch;
}

/*
 *  Funcion: levantarServerInterrupt
 *  Entradas: 	void
 *  Salidas: void
 *  Razon: 	Se levanta el server para crear comunicacion con un Interrupt
 *  Autor:
 */
int levantarServerInterrupt(void)
{
	int kernel_interrupt = levantar_server(configuracion->IPCPU, configuracion->puertoInterrupt, SERVER_INTERRUPT);
	//	Verifico si el descriptor es mayor o menor al maximo o al minimo del select
	compararLimitesConNuevoDescriptor(kernel_interrupt);
	//	Agrego el descriptor del server de DISPATCH al maestro del select
	FD_SET(kernel_interrupt, &master_fd_set);

	return kernel_interrupt;

}
/*
 *  Funcion: atencionInterrupt
 *  Entradas: 	void * socketInterrupt		parametros a castear enviados en la creacion del Thread
 *  Salidas: void
 *  Razon: 	Se genera un thread de atencion a las interrupciones enviadas por el Kernel.
 *  		En caso de recibir el mensaje correcto setea el flag de interrupcion para que el
 *  		CPU deje de procesar y devuelva el PCB al Kernel.
 *  		Caso contrario desestima el mensaje.
 *  Autor:
 */

void * atencionInterrupt(void * socketInterrupt)
{
	int iSocketInterrupt = (int) socketInterrupt;

	//	Defino el mensaje a recibir del Kernel Interrupt
	uint8_t mensaje = 0;
	while(1)
	{
		recv(iSocketInterrupt, &mensaje, sizeof(uint8_t), 0);
		pthread_mutex_lock(&mutex_interrupt);
		if(mensaje == SOLICITAR_INTERRUPCION)
		{
			//	Como el mensaje es correcto seteo la variable para que el CPU devuelva el PCB
			interrupcion = true;
			log_info(logger,"CPU-KERNEL-INTERRUPT Se recibio una INTERRUPCION");
		}
		else
		{
			//	Como el mensaje es incorrecto desestimo el mensaje recibido.
			log_error(logger,"CPU-KERNEL-INTERRUPT Mensaje recibido del interrupt %d es incorrecto, se desestima el mismo", mensaje);
		}
		pthread_mutex_unlock(&mutex_interrupt);

	}
}

/*
 *  Funcion: compararLimitesConNuevoDescriptor
 *  Entradas: 	int nuevoSocket		Nuevo socket que se ingresara al select
 *  Salidas: void
 *  Razon: 	Se compara el nuevo socket con los maximos y minimos del select.
 *  		En caso de el mismo ser mayor o menor que el anterior limite
 *  		se lo modifica.
 *  Autor:
 */

void compararLimitesConNuevoDescriptor(int nuevoSocket)
{
	if( fdmax < nuevoSocket)
		fdmax = nuevoSocket;
	//	Minimo
	if( nuevoSocket < fdmin)
		fdmin = nuevoSocket;
}
