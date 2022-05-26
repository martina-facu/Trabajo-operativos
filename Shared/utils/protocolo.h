#ifndef PROTOCOLO
#define PROTOCOLO

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/config.h>
#include "instrucciones.h"

void llenar_stream(t_list* instrucciones,void* stream){
	int desplazamiento=0;
	Instruccion* instruccion = malloc(sizeof(instruccion));
	uint32_t aux=list_size(instrucciones);
	memcpy(stream+desplazamiento,&aux,sizeof(uint32_t)); // CANT DE INSTRUCCIONES
	desplazamiento+= sizeof(uint32_t);
	uint8_t aux2;
	int* prm;
	for(int i=0;i<list_size(instrucciones);i++){
		instruccion= list_get(instrucciones,i);
		aux2= instruccion->id;
		memcpy(stream+desplazamiento,&aux2,sizeof(uint8_t)); // ID
		desplazamiento+=sizeof(uint8_t);
		int cant_prm = list_size(instruccion->parametros);
		for(int i=0;i<cant_prm;i++){							// PARAMETROS
			prm= list_get(instruccion->parametros,i);
			memcpy(stream+desplazamiento,prm,sizeof(uint32_t));
			desplazamiento+=sizeof(uint32_t);
		}
	}
}

void deserializar_instrucciones(t_buffer* buffer,t_list* instrucciones){
	void* stream = buffer->stream;
	uint32_t* prm;
	uint32_t cant_instrucciones;
	int cant_prm;
	uint8_t id;
	memcpy(&cant_instrucciones,stream,sizeof(uint32_t));
	stream+=sizeof(uint32_t);
	for(int i=0;i<cant_instrucciones;i++){
		Instruccion* instruccion = malloc(sizeof(Instruccion));
		instruccion->parametros= list_create();
		memcpy(&id,stream,sizeof(uint8_t));
		stream+=sizeof(uint8_t);
		instruccion->id = id;
		cant_prm = getCantidadParametros(instruccion->id);
		for(int i=0;i<cant_prm;i++){
			prm= malloc(sizeof(uint32_t));
			memcpy(prm,stream,sizeof(uint32_t));
			stream+=sizeof(uint32_t);
			list_add(instruccion->parametros,prm);
		}
		list_add(instrucciones,instruccion);
	}
}

int iniciar_servidor(char *ip, t_config* config)
{
	// Quitar esta línea cuando hayamos terminado de implementar la funcion

	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, config_get_string_value(config,"PUERTO_ESCUCHA"), &hints, &servinfo); // TODO : hacer que lea del archivo bien el puerto

	// Creamos el socket de escucha del servidor
	int socketserv=socket(servinfo->ai_family,servinfo->ai_socktype,servinfo->ai_protocol);
	// Asociamos el socket a un puerto
	if(bind(socketserv,servinfo->ai_addr,servinfo->ai_addrlen)!=0){
		perror("fallo el bind");
		return 1;
	}
	// Escuchamos las conexiones entrantes
	if(listen(socketserv,SOMAXCONN)==-1){
		perror("error en listen");
		return -1;
	}

	freeaddrinfo(servinfo);
	return socketserv;
}

int esperar_cliente(int socket_servidor)
{
	int socket_cliente=accept(socket_servidor,NULL,NULL);
	if(socket_cliente==-1){
		perror("error al aceptar");
		return -1;
	}

	return socket_cliente;
}


#endif
