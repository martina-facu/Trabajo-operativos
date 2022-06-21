
#include"util.h"

void iniciar_servidor(void)
{
	int socket_servidor;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo("127.0.0.1", "8002", &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next)
    {
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor);
            continue;
        }
        break;
    }

	listen(socket_servidor, SOMAXCONN);

    freeaddrinfo(servinfo);

    while(1)
    	esperar_cliente(socket_servidor);
}

void esperar_cliente(int socket_servidor)
{
	struct sockaddr_in dir_cliente;

	int tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	pthread_create(&thread,NULL,(void*)serve_client,&socket_cliente);
	pthread_detach(thread);

}

void serve_client(int* socket)
{
	int cod_op;
	if(recv(*socket, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		cod_op = -1;
	process_request(cod_op, *socket);
}

void process_request(int cod_op, int cliente_fd) {
	//uint8_t mensaje = 0;
	//uint8_t handshake = 6;
	int size;
	void* msg;

	switch(cod_op){
	case MENSAJE_CLIENTE1:
		//recv(cliente_fd, &mensaje, sizeof(uint8_t), 0);
		//printf("\nMensaje recibido desde el kernel: %d", mensaje);
		//send(cliente_fd, &handshake, sizeof(uint8_t), 0);

		msg = recibir_mensaje(cliente_fd, &size);
		//devolver_mensaje(msg, size, cliente_fd);
		free(msg);
	break;
	case MENSAJE_CLIENTE2:
		msg = recibir_mensaje(cliente_fd, &size);
		//devolver_mensaje(msg, size, cliente_fd);
		free(msg);
	break;
	case INICIALIZAR_PROCESO:

	//inicializar_proceso(cliente_fd);
	break;
	case SUSPENDER_PROCESO:

	//suspender_proceso(cliente_fd);
	break;
	case FINALIZAR_PROCESO:

	//finalizar_proceso(cliente_fd);
	break;
	case ACCEDER_TABLA_DE_PAGINAS:

	//acceder_tabla_de_paginas(cliente_fd);
	break;
	case ACCEDER_ESPACIO_DE_USUARIO:

	//acceder_espacio_de_usuario(cliente_fd);
	break;
	case -1:
		log_error(logger, "El cliente se desconecto. Terminando servidor");
		break;
	default:
	log_warning(logger, "Operacion desconocida. Cuidaditoooo");
	break;
	}
}


void* recibir_mensaje(int socket_cliente, int* size)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void* serializar_paquete(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}

void devolver_mensaje(void* payload, int size, int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));


	paquete->codigo_operacion = MENSAJE_CLIENTE2;

	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = size;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, payload, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}
/*
void finalizar_proceso(int pid){

	t_proceso* proceso = obtenerProceso(cliente_fd);
	if (proceso != NULL){
		if (!proceso->abortado)

		liberar_espacio_proceso(proceso);
		clientes[cliente_fd].atendido = false;
	}
}
*/

void enviar_paquete(t_paquete* paquete, int socket_cliente)
{
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

t_paquete* crear_paquete_con_codigo_op(op_code codigo_op)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = codigo_op;
	crear_buffer(paquete);
	return paquete;
}

void crear_buffer(t_paquete* paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void enviarFail(int socket){

	t_paquete* paquete = crear_paquete_con_codigo_op(FAIL); //VER DE AGREGAR ESTE MENSAJE O CAMBIARLO

	enviar_paquete(paquete,socket);

	eliminar_paquete(paquete);

	liberar_conexion(socket);

}

void liberar_conexion(int socket_cliente){

	close(socket_cliente);
}

void enviarOK(int socket){

	//pthread_mutex_lock(&mutexOk);

	t_paquete* paquete = crear_paquete_con_codigo_op(OK);

	enviar_paquete(paquete,socket);

	eliminar_paquete(paquete);

	//pthread_mutex_unlock(&mutexOk);

	liberar_conexion(socket);

}

int leer_entero(char*buffer, int* desplazamiento)
{
	int ret;
	memcpy(&ret, buffer + (*desplazamiento), sizeof(int));
	(*desplazamiento)+=sizeof(int);
	return ret;
}
/*
char* leer_string(char* buffer, int* desplazamiento)
{
	//char* buf = (char*) buffer;

	int tamanio = leer_entero(buffer, desplazamiento);
	//printf("allocating / copying %d \n",tamanio);
/*

	memcpy(&tamanio, buffer + (*desplazamiento), sizeof(int));
	(*desplazamiento)+=sizeof(int);
//
	char* valor = malloc(tamanio);
	memcpy(valor, buffer+(*desplazamiento), tamanio);
	(*desplazamiento)+=tamanio;

	return valor;
}
*/
