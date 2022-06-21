#ifndef CONEXIONES_H_
#define CONEXIONES_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<string.h>
#include<pthread.h>

typedef enum
{
	MENSAJE_CLIENTE1=0,
	MENSAJE_CLIENTE2=1,
	INICIALIZAR_PROCESO=2,
	SUSPENDER_PROCESO=3,
	FINALIZAR_PROCESO=4,
	ACCEDER_TABLA_DE_PAGINAS=5,
	ACCEDER_ESPACIO_DE_USUARIO=6,
	FAIL,
	OK
}op_code;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

pthread_t thread;

t_log* logger;
void* recibir_buffer(int*, int);

void iniciar_servidor(void);
void esperar_cliente(int);
void* recibir_mensaje(int socket_cliente, int* size);
int recibir_operacion(int);
void process_request(int cod_op, int cliente_fd);
void serve_client(int *socket);
void* serializar_paquete(t_paquete* paquete, int bytes);
void devolver_mensaje(void* payload, int size, int socket_cliente);


#endif /* CONEXIONES_H_ */
