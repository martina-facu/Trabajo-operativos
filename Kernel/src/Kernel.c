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


char* nombre[6]={"NO_OP", "I/O", "WRITE", "COPY", "READ", "EXIT"};

typedef struct {
	t_list* parametros;
	int id; //NO_OP, I/O, READ, WRITE, COPY, EXIT
} Instruccion;

typedef struct {
	uint32_t size;
	void* stream;
} t_buffer;

typedef struct {
	uint8_t codigo_operacion;
	t_buffer* buffer;
} t_paquete;

void mostrar_parametros(t_list* list){
	int aux = list_size(list);

	for(int i=0;i<aux;i++){
		int* parametro= list_get(list,i);
		printf(" %d ", *parametro);
	}
	printf("\n");
}
void mostrar_instrucciones(t_list* list){
	int aux = list_size(list);
	Instruccion* instruccion = malloc(sizeof(instruccion));

	for(int i=0;i<aux;i++){
		instruccion= list_get(list,i);
		printf("\nEl codigo de la instruccion es: %d , nombre: %s \n parametros: ", instruccion->id, nombre[(instruccion->id)-1]);
		mostrar_parametros(instruccion->parametros);
	}
}

int getCantidadParametros(int id) {
	switch (id) {
	case 1: case 2: case 5:
		return 1;
		break;
	case 3: case 4:
		return 2;
		break;
	case 6:
		return 0;
		break;
	default:
		printf("\n Se ingreso una operacion incorrecta");
		return -1;
		break;
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

int iniciar_servidor(void)
{
	// Quitar esta línea cuando hayamos terminado de implementar la funcio


	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	t_config* config = config_create("kernel.config");
	getaddrinfo("127.0.0.1", config_get_string_value(config,"PUERTO_ESCUCHA"), &hints, &servinfo);

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

int main(){
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_buffer));
	int socket_serv = iniciar_servidor();
	int cliente=esperar_cliente(socket_serv);

	recv(cliente, &(paquete->codigo_operacion), sizeof(uint8_t), 0);
	// Después ya podemos recibir el buffer. Primero su tamaño seguido del contenido
	recv(cliente, &(paquete->buffer->size), sizeof(uint32_t), 0);
	paquete->buffer->stream = malloc(paquete->buffer->size);
	recv(cliente, paquete->buffer->stream, paquete->buffer->size, 0);

	t_list* instrucciones = list_create();
	deserializar_instrucciones(paquete->buffer,instrucciones);


	mostrar_instrucciones(instrucciones);
	close(socket_serv);

	return 0;
}
