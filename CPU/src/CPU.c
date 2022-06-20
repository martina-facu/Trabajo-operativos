#include "utils.h"


uint32_t interrupcion = 0;

void ejecutar_ciclo_instrucciones(Pcb* pcb,t_config* config, bool* devolver_pcb) {
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
	*devolver_pcb = execute(instruccion,config,pcb);

	//check interrupt
	if (*devolver_pcb == false) {
		*devolver_pcb = interrupcion;
	}
}

int main(void)
{
	config = config_create("cpu.config");
	int socket_dispatch = 0;
	int socket_interrupt = 0;
	fdmax = -1;
	fdmin = 201669;
	connectionsDispatch = 0;
	connectionsInterrupt = 0;
	uint32_t cantidad_entradas, tamano_pagina = 0;
	devolver_pcb = false;
	recibiPCB = false;


	logger = initLogger("cpu.log", "CPU", LOG_LEVEL_INFO);
	//	Seteo en 0 a los set de descriptores a revisar por el select
	FD_ZERO(&read_fd_set);
	FD_ZERO(&master_fd_set);

//	Iniciar conexiones
	int conexion_memoria = levantar_conexion_memoria(config,&cantidad_entradas,&tamano_pagina);
	//	Marco el descriptor en donde me conecte al server de memoria como limite maximo y minimo del select
	fdmax = conexion_memoria;
	fdmin = conexion_memoria;

	//	Levanto el server para el DISPATCH
	int kernel_dispatch = levantar_server(config, SERVER_DISPATCH);
	//	Verifico si el descriptor es mayor o menor al maximo o al minimo del select
	//	Maximo
	if( fdmax < kernel_dispatch)
		fdmax = kernel_dispatch;
	//	Minimo
	if( kernel_dispatch < fdmin)
		fdmin = kernel_dispatch;
	//	Agrego el descriptor del server de DISPATCH al maestro del select
	FD_SET(kernel_dispatch, &master_fd_set);


//	int kernel_dispatch = levantar_server(config, &socket_dispatch, SERVER_DISPATCH);
//	int kernel_dispatch = levantar_canal_dispatch(config, &socket_dispatch);
	int kernel_interrupt = levantar_server(config, SERVER_INTERRUPT);
	//	Verifico si el descriptor es mayor o menor al maximo o al minimo del select
	//	Maximo
	if( fdmax < kernel_interrupt)
		fdmax = kernel_interrupt;
	//	Minimo
	if(kernel_interrupt < fdmin)
		fdmin = kernel_interrupt;
	//	Agrego el descriptor del server de DISPATCH al maestro del select
	FD_SET(kernel_interrupt, &master_fd_set);




//	int kernel_interrupt = levantar_server(config, &socket_interrupt, SERVER_INTERRUPT);
//	int kernel_interrupt = levantar_puerto_interrupt(config, &socket_interrupt);

//	Obtener informacion de memoria
	// cantidad de entradas por tabla de páginas y tamaño de página.

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
			log_info(logger, "ERROR SELECT");
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
						pcb_mostrar(pcb);
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

			pcb_mostrar(pcb);

			//	DEVOLVER PCB AL KERNEL
			uint32_t* tamano_mensaje = malloc(sizeof(uint32_t));
			void* a_enviar = pcb_serializar(pcb,tamano_mensaje,1);
			send(acceptedConecctionDispatch, a_enviar, *tamano_mensaje, 0);
			recibiPCB = false;

		}



		//	Vuelvo a iniciar el proceso del While
	}



//	//	Recibir pcb del kernel
//		Pcb* pcb = obtener_pcb(socket_dispatch, kernel_dispatch);
//		pcb_mostrar(pcb);


////	Ejecutar ciclo de instrucciones
//	bool devolver_pcb = false;
//	while (devolver_pcb == false) {
//		ejecutar_ciclo_instrucciones(pcb,config,&devolver_pcb);
//	}
//
//	pcb_mostrar(pcb);
//
////	DEVOLVER PCB AL KERNEL
//	uint32_t* tamano_mensaje = malloc(sizeof(uint32_t));
//	void* a_enviar = pcb_serializar(pcb,tamano_mensaje,1);
//	send(kernel_dispatch, a_enviar, *tamano_mensaje, 0);


//	pthread_join( thread_id , NULL);

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
