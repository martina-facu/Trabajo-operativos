/*
 * planificador_largo_plazo.c
 *
 *  Created on: 24 jun. 2022
 *      Author: utnso
 */
#include "planificador_largo_plazo.h"



// PCP: PLANIFICADOR CORTO PLAZO
// PLP: PLANIFICADOR LARGO PLAZO
// PMP: PLANIFICADOR MEDIANO PLAZO


comunicacion_t* comunicacion_create(sem_t* s,uint32_t pid){
	comunicacion_t* comunicacion= malloc(sizeof(comunicacion_t));
	comunicacion->pid = pid;
	comunicacion->s=s;
	return comunicacion;
}

bool ordenar_pcb(void* aux1,void* aux2){
	pcb_t* pcb1= aux1;
	pcb_t* pcb2= aux2;
	return pcb1->pid<pcb2->pid;
}



//--------------------------------------------------------------------------------------//
//FUNCION: 							GESTIONAR_COMUNIACION
//RESPONSABILIDAD: 	MANTENER LA COMUNICACION CON LA CONSOLA Y FINALIZARLA CUANDO CORRESPONDA
//ENTRADAS: 		SOCKET , ID,  AVISO DE QUE HAY QUE FINALIZAR
//SALIDAS:			MENSAJE DE FINALIZACION
//--------------------------------------------------------------------------------------//
void* gestionar_comunicacion(void* aux)
{
	int socket;
	int id;

	// RECIBO EL SOCKET Y EL ID POR PARAMETRO
	memcpy(&socket,aux,sizeof(uint32_t));
	memcpy(&id,aux+sizeof(uint32_t),sizeof(uint32_t));

	log_info(PLP,"Mi ID de Thread es: %d", id);
	log_info(PLP,"El descriptor para comunicarme con mi consola es: %d", socket);


	uint32_t espacio;
	// RECIBO LAS INSTRUCCIONES Y EL ESPACIO DEL PROCESO
	t_list* instrucciones = deserializar_mensaje(socket, &espacio, PLP);
//	t_list* instrucciones = deserializar_paquete_instrucciones(socket,&espacio, PLP);
	// CREO UN PCB CON ESAS INSTRUCCIONES Y EL ESPACIO QUE VA A OCUPAR
	pcb_t* pcb=  pcb_create(espacio,instrucciones,id, configuracion->estimacion_inicial, 0);


	// AÑADO EL PCB CREADO A NEW
	pthread_mutex_lock(&mx_new_l);
	list_add_sorted(new_l,pcb,ordenar_pcb);
	pthread_mutex_unlock(&mx_new_l);
	log_trace(PLP,"se agrego un proceso a new, ID: %d", pcb->pid);

	// CREO EL SEMAFORO CUYA FUNCION VA A SER ESPERAR QUE EL PROCESO FINALICE PARA PODER AVISARLE A LA CONSOLA QUE FINALIZO
	sem_t s;
	sem_init(&s,0,0);
	log_trace(PLP,"se creo una comunicacion");
	/*
	 *	CREO UNA VARIABLE DE ESTRUCTURA COMUNICACION PARA PODER GUARDAR EL SEMAFORO Y EL
	 *	IDENTIFICADOR DEL PROCESO PARA QUE LA FUNCION: FINALIZAR PROCESO SEPA A CUAL HILO
	 *	DEBE DESBLOQUEAR PARA AVISAR LA FINALIZACION DEL MISMO
	 */
	comunicacion_t* comunicacion = comunicacion_create(&s,pcb->pid);
	pthread_mutex_lock(&mx_comunicaciones_l);
	list_add(comunicaciones_l,comunicacion);
	pthread_mutex_unlock(&mx_comunicaciones_l);
	log_trace(PLP,"me voy a bloquear, ID: %d", pcb->pid);

	// AVISO QUE YA SE AGREGO UN PROCESO A NEW
	sem_post(&s_proceso_new);

	// LO BLOQUEO PARA ESPERAR QUE TERMINE DE EJECUTAR EL PROCESO
	sem_wait(&s);

	// AVISO QUE TERMINO BIEN!! :D
	log_trace(PLP,"Puedo finalizar el proceso ID: %d, envio mensaje", pcb->pid);
	uint8_t auxv= PROCESO_FINALIZADO;
	void* a_enviar = malloc(sizeof(uint8_t));
	memcpy(a_enviar,&auxv,sizeof(uint8_t));
	send(socket,a_enviar,sizeof(uint8_t),0);
	send(socket,&id,sizeof(uint32_t),0);

	// CHAU HILO GRACIAS POR TU SERVICIO
	return NULL;
}

//--------------------------------------------------------------------------------------//
//FUNCION:						COMUNICACION_CON_CONSOLAS
//RESPONSABILIDAD: RECIBIR CONSOLAS Y CREAR HILOS QUE SE COMUNIQUEN CON LAS MISMAS
//ENTRADAS:		   CONEXION CON CONSOLA
//SALIDAS:		   HILO COMUNICADOR, ID Y SOCKET DE LA CONSOLA
//--------------------------------------------------------------------------------------//
void* comunicacion_con_consolas()
{
	uint32_t acum=0; // acumulador se va a encargar de contar las consolas y va a servir para asignarles un id unico
	uint8_t handshake;
	uint8_t mensajeConsola = 0;
	while(1)
	{
		//	Acepto de forma temporal la conexion hasta que valide que es un cliente
		uint32_t socket_cliente = accept(server_fd,NULL,NULL);
		log_trace(PLP,"Se acepto temporalmente la conexion en el descriptor: %d hasta validar la misma", socket_cliente);
		if( socket_cliente < 0)
			log_trace(PLP,"Error al intentar aceptar conexion de un cliente");
		else
		{
			/*
			 * 	Valido con Handshake sino es correcto cierro comunicacion y si es correcto sigo con
			 * 	el codigo ya existente.
			 */
			//	Recibo el mensaje de la consola
			mensajeConsola = 0;
			recv(socket_cliente, &mensajeConsola, sizeof(uint8_t), 0);
			log_info(PLP, "Mensaje recibido de la consola:  %d", mensajeConsola);

			if(mensajeConsola == INICIAR_CONEXION_CONSOLA)
			{

				handshake = ACEPTAR_CONEXION_CONSOLA;
				send(socket_cliente, &handshake, sizeof(uint8_t), 0);
				log_info(PLP, "Conexion establecida con la Consola");

				//guardo el socket y el acumulador para pasarselos a la funcion GESTIONAR_COMUNICACION
				void* aux = malloc(sizeof(uint32_t)*2);
				memcpy(aux,&socket_cliente,sizeof(uint32_t));
				memcpy(aux+sizeof(uint32_t),&acum,sizeof(uint32_t));

				//	CREO UN HILO PARA MANTENER LA COMUNICACION CON UNA NUEVA CONSOLA
				//	Y LE PASO EL BUFFER AUX QUE CONTIENE EL SOCKET Y EL ACUMULADOR
				pthread_t hilo;
				int status =pthread_create(&hilo,NULL,gestionar_comunicacion,aux);
				pthread_detach(hilo);
				// AUMENTO EL ACUMULADOR PARA LA PROXIMA COMUNICACION
				acum++;
				// EVALUO QUE EL HILO SE HAYA CREADO CORRECTAMENTE
				if(status <0)
				{
					log_error(PLP, "No pudo crearse un nuevo hilo de atencion a consola. Cierro la conexion con la misma");
					close(socket_cliente);
				}
				else
					log_info(PLP, "Se creo un nuevo hilo para comunicarme con una consola.");

			}
			else
			{
				log_error(PLP, "Handshake recibido de consola invalido: %d",mensajeConsola);
				close(socket_cliente);
			}
		}
	}
	return NULL;
}
//--------------------------------------------------------------------------------------//
//FUNCION:						PASAR A READY
//RESPONSABILIDAD: AVISAR AL PCP QUE HAY UN PROCESO EN NEW CON MEMORIA ASIGNADA, SIEMPRE QUE EL GRADO DE MULTIPROGRAMACION LO PERMITA
//ENTRADAS:		   HAY UN PROCESO NUEVO Y HAY GRADO DE MULTIPROGRAMACION
//SALIDAS:		   SEM POST DE QUE HAY UN PROCESO PARA PASAR READY
//--------------------------------------------------------------------------------------//
void* pasar_a_ready(){
	while(1){
		// 	ESPERO QUE HAYA UN PROCESO EN NEW Y QUE EL GRADO DE MULTIPROGRAMACION
		//	ME PERMITA ASIGNAR MEMORIA
		sem_wait(&s_proceso_new);
		sem_wait(&s_grado_multiprogramacion);
		// SACO AL PCB DE LA LISTA DE NEW
		pthread_mutex_lock(&mx_new_l);
		pcb_t* pcb= list_remove(new_l,0);
		pthread_mutex_unlock(&mx_new_l);
		log_trace(PLP,"se pasa un proceso a ready, ID: %d",pcb->pid);
		pthread_mutex_lock(&mx_mensaje_memoria);
		uint8_t mensaje = INICIALIZAR_PROCESO;
		send(socket_memoria,&mensaje,sizeof(uint8_t),0);
		log_trace(PLP,"SE ENVIO UN MENSAJE");
		send(socket_memoria,&pcb->pid,sizeof(uint32_t),0);
		send(socket_memoria,&pcb->tamano,sizeof(uint32_t),0);
		pthread_mutex_unlock(&mx_mensaje_memoria);
		//	LO AGREGO A UN BUFFER ENTRE PLP Y PCP, PARA QUE EL PCP LO SAQUE
		//	DEL BUFFER Y NO DE LA LISTA DE NEW
		recv(socket_memoria,&pcb->tabla_paginas,sizeof(uint32_t),0);

		log_trace(PLP,"RECIBI LA ENTRADA: %d", pcb->tabla_paginas);

		pthread_mutex_lock(&mx_newM_l);
		list_add(newM_l,pcb);
		pthread_mutex_unlock(&mx_newM_l);

		// AVISO QUE HAY UN PROCESO LISTO PARA QUE EL PCP LO AGREGUE A READY

		sem_post(&s_proceso_ready);
	}
	return NULL;
}

comunicacion_t* buscar_comunicacion(pcb_t* pcb){
	comunicacion_t* comunicacion= malloc(sizeof(comunicacion_t));
	for(int i=0;i<list_size(comunicaciones_l);i++){
		comunicacion=list_get(comunicaciones_l,i);
		if(comunicacion->pid == pcb->pid){
			printf("encontre la comunicacion %d, %d", comunicacion->pid, pcb->pid);
			comunicacion = list_remove(comunicaciones_l,i);
			return comunicacion;
		}
	}
	return NULL;
}

//--------------------------------------------------------------------------------------//
//FUNCION: 						FINALIZAR_PROCESOS
//RESPONSABILIDAD: AGREGAR EL PROCESO FINALIZADO AL ESTADO EXIT Y AVISAR AL HILO QUE MANTENIA LA COMUNICACION CON LA CONSOLA
//				   QUE PUEDE TERMINAR TRANQUILO
// ENTRADAS:	   AVISO QUE HAY QUE FINALIZAR UN PROCESO
// SALIDAS:		   AVISO QUE SE PUEDE CARGAR UN NUEVO PROCESOE EN MEMORIA
//--------------------------------------------------------------------------------------//

void* finalizar_procesos(){
	while(1)
	{
		// ESPERO QUE HAYA UN PROCESO FINALIZADO
		sem_wait(&s_proceso_finalizado);
		log_trace(logP,"se va a finalizar un proceso");
		pcb_t* pcb_finalizado = malloc(sizeof(pcb_t));

		// LO SACO DE LA LISTA DE FINALIZADOR QUE FUNCIONA COMO UN BUFFER
		pthread_mutex_lock(&mx_finalizado_l);
		pcb_finalizado=list_remove(finalizado_l,0);
		printf("proceso finalizado: \n");
		pcb_mostrar(pcb_finalizado, PLP);
		pthread_mutex_unlock(&mx_finalizado_l);
		uint8_t mensaje= FINALIZAR_PROCESO;
		pthread_mutex_lock(&mx_mensaje_memoria);
		send(socket_memoria,&mensaje,sizeof(uint8_t),0);
		send(socket_memoria,&pcb_finalizado->pid,sizeof(uint32_t),0);
		pthread_mutex_unlock(&mx_mensaje_memoria);
		comunicacion_t* comunicacion =buscar_comunicacion(pcb_finalizado);
		log_trace(PLP,"se encontro una comunicacion");

		// HAGO EL SEM POST PARA QUE SE DESBLOQUEE EL HILO Y AVISE
		sem_post(comunicacion->s);

		// AÑADO A LA LISTA DE EXIT
		list_add(exit_l,pcb_finalizado);

		// AVISO QUE SE PUEDE AGREGAR A MEMORIA UN PROCESO NUEVO
		sem_post(&s_grado_multiprogramacion);
	}
	return NULL;
}

void* administrador_largo_plazo()
{


	pthread_t hilo1;
	pthread_t hilo2;
	pthread_t hilo3;


	pthread_create(&hilo1,NULL,comunicacion_con_consolas,NULL);
	pthread_create(&hilo2,NULL,pasar_a_ready,NULL);
	pthread_create(&hilo3,NULL,finalizar_procesos,NULL);
	pthread_detach(hilo2);

	pthread_detach(hilo3);

	pthread_join(hilo1,NULL);

	return NULL;
}
