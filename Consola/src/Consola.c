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
		printf("me conecte con exito");
	}
	else{
		printf("error: NO ME PUDE CONECTAR");
		return -1;
	}


	freeaddrinfo(server_info);

	return socket_cliente;
}

// void exit_gracefully(int return_nr) {
//   exit(return_nr);
// }

//------------------------------------------Estructuras ----------------------------------------------------------
typedef struct {
	t_list* parametros;
	uint8_t id; //NO_OP, I/O, READ, WRITE, COPY, EXIT
} Instruccion;

typedef struct {
	uint32_t size;
	void* stream;
} t_buffer;

typedef struct {
	uint8_t codigo_operacion;
	t_buffer* buffer;
} t_paquete;

//----------------------------------Funciones---------------------------------------------
int definirCodigo(char* id) {
	if (string_contains(id, "NO_OP")) {
		return 1;
	}
	if (string_contains(id, "I/O")) {
		return 2;
	}
	if (string_contains(id, "WRITE")) {
		return 3;
	}
	if (string_contains(id, "COPY")) {
		return 4;
	}
	if (string_contains(id, "READ")) {
		return 5;
	}
	if (string_contains(id, "EXIT")) {
		return 6;
	}else{
		return -1;
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
	char* nombre[6]={"NO_OP", "I/O", "WRITE", "COPY", "READ", "EXIT"};


	for(int i=0;i<aux;i++){
		instruccion= list_get(list,i);
		printf("\nEl codigo de la instruccion es: %d , nombre: %s \n cantidad de parametros: %d parametros: ", instruccion->id, nombre[(instruccion->id)-1], getCantidadParametros(instruccion->id));
		mostrar_parametros(instruccion->parametros);
	}
}

int calcular_espacio_instrucciones(t_list* instrucciones){
	int size = sizeof(uint32_t); // cantidad de instrucciones
	Instruccion* instruccion = malloc(sizeof(instruccion));
	int aux=list_size(instrucciones);

	for(int i=0;i<aux;i++){
		instruccion= list_get(instrucciones,i);
		size+= sizeof(uint8_t); //id
		size+=sizeof(uint32_t)*list_size(instruccion->parametros); //un int por parametro
	}
	return size;
}

void llenar_stream(t_list* instrucciones,void* stream){
	int desplazamiento=0;
	Instruccion* instruccion = malloc(sizeof(instruccion));
	uint32_t aux=list_size(instrucciones);
	memcpy(stream+desplazamiento,&aux,sizeof(uint32_t)); // CANT DE INSTRUCCIONES
	desplazamiento+= sizeof(uint32_t);
	int aux2;
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


//_-----------------------------------Funcionalidad------------------------------------------

int main() {

	//Conecta como cliente al Kernel
	/*int sockfd = conectar_kernel();
	 saludo_inicial_kernel(sockfd);*/

//	char* tamanio_proceso = argv[2];
//	char* filename = argv[1];

	FILE* input_file = fopen("/home/utnso/tp-2022-1c-9-12/consola/instrucciones.txt", "r"); //harcodeado

	if(input_file==NULL){
		perror("error al leer el archivo");
		return -1;
	}

	if (!input_file)
		exit(EXIT_FAILURE);

	char *contents = NULL;
	size_t len = 0;

	t_list* instrucciones = list_create();
	uint32_t* parametro;
	while (getline(&contents, &len, input_file) != -1) { //contents = contenido de la linea

		char** linea = string_split(contents, " ");
		char* nombre = linea[0];
		int id = definirCodigo(nombre);

		Instruccion* instruccion = malloc(sizeof(Instruccion));
		instruccion->id = id;
		instruccion->parametros = list_create();

		for (int i = 1;linea[i]!=NULL; i++) {
			parametro = malloc(sizeof(uint32_t));
			*parametro = atoi(linea[i]);
			list_add(instruccion->parametros, parametro);
		}
		list_add(instrucciones, instruccion);

	}

	mostrar_instrucciones(instrucciones);

	fclose(input_file);
	free(contents);
	 // ---------------------------------------------------------------------------- SERIALIZACION ----------------------------------------------------------------------------------------//

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = calcular_espacio_instrucciones(instrucciones);
	void* stream = malloc(buffer->size);

	llenar_stream(instrucciones,stream);
	buffer->stream=stream;

	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion= 0;
	paquete->buffer= buffer;

	void* a_enviar = malloc(buffer->size + sizeof(uint8_t) + sizeof(uint32_t));
	int offset = 0;

	memcpy(a_enviar + offset, &(paquete->codigo_operacion), sizeof(uint8_t));
	offset += sizeof(uint8_t);
	memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);

	 // ------------------------------------------------------------------------------ CONEXION ----------------------------------------------------------------------------------------//
	t_config* config = config_create("kernel.config");
	char* ip= config_get_string_value(config,"IP_KERNEL");
	char* puerto= config_get_string_value(config,"PUERTO_KERNEL");
	int conexion= crear_conexion(ip,puerto);
	send(conexion,a_enviar,buffer->size+sizeof(uint8_t)+sizeof(uint32_t),0);
	close(conexion);
	return EXIT_SUCCESS;
}
