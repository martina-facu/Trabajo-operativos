#include "conexion.h"

int crear_conexion(char *ip, char* puerto, t_log* logger)
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
		log_info(logger, "Conexion establecida con exito");
	else
	{
		log_error(logger, "Fallo la conexion al server");
		return -1;
	}


	freeaddrinfo(server_info);

	return socket_cliente;
}

int iniciar_servidor(char *ip, char* puerto_escucha, t_log* logger)
{
	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto_escucha, &hints, &servinfo);

	// Creamos el socket de escucha del servidor
	int socketserv=socket(servinfo->ai_family,servinfo->ai_socktype,servinfo->ai_protocol);
	//	Marcamos el socket como reutilizable
		if (setsockopt(socketserv, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
		    perror("setsockopt(SO_REUSEADDR) failed");
	// Asociamos el socket a un puerto
	if(bind(socketserv,servinfo->ai_addr,servinfo->ai_addrlen)!=0){
		log_error(logger, "Fallo el bind al generar server");
		return 1;
	}
	// Escuchamos las conexiones entrantes
	if(listen(socketserv,SOMAXCONN)==-1)
	{
		log_error(logger, "Fallo el listen al generar server");
		return -1;
	}
	log_info(logger, "Server escuchando");

	freeaddrinfo(servinfo);
	return socketserv;
}

int esperar_cliente(int socket_servidor, t_log* logger)
{
	int socket_cliente=accept(socket_servidor,NULL,NULL);
	if(socket_cliente==-1)
	{
		log_error(logger, "Error al aceptar cliente");
		return -1;
	}

	return socket_cliente;
}
