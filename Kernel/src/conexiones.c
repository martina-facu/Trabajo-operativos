/*
 * conexiones.c
 *
 *  Created on: 24 jun. 2022
 *      Author: utnso
 */

#include "conexiones.h"

void levantar_conexion_cpu_dispatch(char* ipServer, char* portServer, t_log* logger)
{
	socket_cpu_dispatch = crear_conexion(ipServer, portServer,logger);
	log_trace(logger, "KERNEL-CPU-DISPATCH: Creo conexion con la CPU DISPATCH en la IP %s y puerto %s con el descriptor:  %d", ipServer, portServer, socket_cpu_dispatch);


	uint8_t handshake = INICIAR_CONEXION_DISPATCH;
	send(socket_cpu_dispatch, &handshake, sizeof(uint8_t), 0);
	log_trace(logger, "KERNEL-CPU-DISPATCH: Se envia Handshake a CPU dispatch");

	uint8_t respuesta = 0;
	recv(socket_cpu_dispatch, &respuesta, sizeof(uint8_t), 0);
	log_trace(logger, "KERNEL-CPU-DISPATCH: Mensaje recibido del CPU:  %d", respuesta);

	if(respuesta == ACEPTAR_CONEXION_DISPATCH)
		log_info(logger, "KERNEL-CPU-DISPATCH: Conexion establecida con la CPU dispatch");
	else
	{
		log_error(logger, "KERNEL-CPU-DISPATCH: No se puede establecer conexion con CPU dispatch. El server contactado responde con un mensaje no predeterminado: %d.", respuesta);
		exit(EXIT_FAILURE);
	}

}

void levantar_conexion_cpu_interrupt(char* ipServer, char* portServer, t_log* logger)
{
	socket_cpu_interrupt = crear_conexion(ipServer, portServer,logger);
	log_trace(logger, "KERNEL-CPU-INTERRUPT: Creo conexion con la CPU INTERRUPT en la IP %s y puerto %s con el descriptor:  %d", ipServer, portServer, socket_cpu_interrupt);

	uint8_t handshake= INICIAR_CONEXION_INTERRUPT;
	send(socket_cpu_interrupt, &handshake, sizeof(uint8_t), 0);
	log_trace(logger, "KERNEL-CPU-INTERRUPT: Se envia Handshake a la CPU interrupt");

	uint8_t respuesta = 0;
	recv(socket_cpu_interrupt, &respuesta, sizeof(uint8_t), 0);
	log_trace(logger, "KERNEL-CPU-INTERRUPT: Mensaje recibido de la CPU interrupt:  %d", respuesta);

	if(respuesta == ACEPTAR_CONEXION_INTERRUPT)
		log_info(logger, "KERNEL-CPU-INTERRUPT: Conexion establecida con la CPU interrupt");
	else
	{
		log_error(logger, "KERNEL-CPU-INTERRUPT: No se puede establecer conexion con la CPU interrupt. El server contactado responde con un mensaje no predeterminado: %d.", respuesta);
		exit(EXIT_FAILURE);
	}
}

void levantar_conexion_memoria(char* ipServer, char* portServer, t_log* logger)
{
	socket_memoria = crear_conexion(ipServer, portServer,logger);
	log_trace(logger, "KERNEL-MEMORIA: Creo conexion con la MEMORIA en la IP %s y puerto %s con el descriptor:  %d", ipServer, portServer, socket_memoria);


	uint8_t handshake= INICIAR_CONEXION_KERNEL;
	send(socket_memoria, &handshake, sizeof(uint8_t), 0);
	log_trace(logger, "KERNEL-MEMORIA: Se envia Handshake a la memoria");

	uint8_t respuesta = 0;
	recv(socket_memoria, &respuesta, sizeof(uint8_t), 0);
	log_trace(logger, "KERNEL-MEMORIA: Mensaje recibido de la memoria:  %d", respuesta);

	if(respuesta == ACEPTAR_CONEXION_KERNEL)
		log_info(logger, "KERNEL-MEMORIA: Conexion establecida con la memoria");
	else
	{
		log_error(logger, "KERNEL-MEMORIA: No se puede establecer conexion con la memoria. El server contactado responde con un mensaje no predeterminado: %d.", respuesta);
		exit(EXIT_FAILURE);
	}
}
