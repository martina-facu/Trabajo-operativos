#include "conexion.h"

int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family,server_info->ai_socktype,server_info->ai_protocol);

	if(connect(socket_cliente, server_info->ai_addr,server_info->ai_addrlen)==0)
		{
		printf("\nme conecte con exito");
	}
	else{
		printf("\nerror: NO ME PUDE CONECTAR");
		return -1;
	}


	freeaddrinfo(server_info);

	return socket_cliente;
}

int iniciar_servidor(char *ip, char* puerto_escucha)
{
	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto_escucha, &hints, &servinfo);

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

	printf("\nServidor escuchando en %s",puerto_escucha);

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
