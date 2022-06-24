#include "utils.h"

pcb_t* obtener_pcb(int cliente)
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

	pcb_t* pcb = pcb_deserializar(buffer);

	recibiPCB = true;

	return pcb;
}


int levantar_conexion_memoria(char* ipServer, char* portServer, t_log* logger, uint32_t* cantidad_entradas,uint32_t* tamano_pagina)
{
	int conexion_memoria = crear_conexion(ipServer, portServer, logger);
	log_info(logger, "Conexion establecida en la memoria en la IP %s y puerto %s con el descriptor:  %d", ipServer, portServer, conexion_memoria);


	uint8_t handshake_memoria = INICIAR_CONEXION_CPU;
	send(conexion_memoria, &handshake_memoria, sizeof(uint8_t), 0);
	log_info(logger, "Se envia Handshake a la memoria");

	uint8_t respuesta_memoria = 0;
	recv(conexion_memoria, &respuesta_memoria, sizeof(uint8_t), 0);
	log_info(logger, "Mensaje recibido de la memoria:  %d", respuesta_memoria);

	if(respuesta_memoria == ACEPTAR_CONEXION_CPU)
		log_info(logger, "Conexion establecida con la memoria");
	else
	{
		log_error(logger, "No se puede establecer conexion con la memoria. El server contactado responde con un mensaje no predeterminado: %d.", respuesta_memoria);
		exit(EXIT_FAILURE);
	}
//	TODO: descomentar estas lineas cuando se implemente la parte de la memoria
//	recv(conexion_memoria, cantidad_entradas, sizeof(uint32_t), 0);
//	recv(conexion_memoria, tamano_pagina, sizeof(uint32_t), 0);

	return conexion_memoria;
}


int levantar_server(char* ipServer, char* portServer, t_log* logger, char* sTipo)
{
	int socket;

	//	Inicio el servidor en la IP y puertos leidos desde el archivo de configuracion
	socket = iniciar_servidor(ipServer, portServer, logger);
	log_info(logger, "Socket en el que se levanta el server %s: %d en la IP: %s y Puerto: %s",sTipo, socket, ipServer, portServer);

	return socket;
}
bool execute(Instruccion* instruccion,int dormir, pcb_t* pcb)
{ // TODO : encapsular la logica de las instrucciones mas complicadas
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

//	int dormir = config_get_int_value(config,"RETARDO_NOOP");

	switch (id) {
	case 1:
		dormir = dormir/1000;
		sleep(dormir);
		return false;
		break;
	case 2:
		pcb->estado = BLOQUEADO;
		pcb->tiempo_block = *parametro1;
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

