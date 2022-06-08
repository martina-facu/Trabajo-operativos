#include <stdio.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/config.h>
#include <conexion.h>
#include <pcb.h>
#include <paquete.h>

Pcb* obtener_pcb(int socket_serv, int cliente) {

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

	Pcb* pcb = pcb_deserializar(buffer);

	return pcb;
}

int levantar_conexion_memoria(t_config* config) {
	char * ip_memoria = malloc(sizeof(char) * 30);
	strcpy(ip_memoria, config_get_string_value(config, "IP_MEMORIA"));
	printf("\nIp de la memoria: %s", ip_memoria);

	char* puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
	int conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);

	uint8_t handshake_memoria = 8;
	send(conexion_memoria, &handshake_memoria, sizeof(uint8_t), 0);

	uint8_t respuesta_memoria = 0;
	recv(conexion_memoria, &respuesta_memoria, sizeof(uint8_t), 0);
	printf("\nMensaje recibido de la memoria: %d", respuesta_memoria);

	return conexion_memoria;
}

int levantar_canal_dispatch(t_config* config, int* socket_dispatch) {
	char* puerto_dispatch = config_get_string_value(config,
			"PUERTO_ESCUCHA_DISPATCH");
	printf("\nPuerto escucha %s", puerto_dispatch);

	*socket_dispatch = iniciar_servidor("127.0.0.1", puerto_dispatch);
	printf("\nSocket %d", *socket_dispatch);

	int cliente1 = esperar_cliente(*socket_dispatch);
	printf("\nCliente: %d", cliente1);
	printf("\n");

	uint8_t mensaje = 0;
	recv(cliente1, &mensaje, sizeof(uint8_t), 0);
	printf("Mensaje recibido dispatch: %d", mensaje);
	printf("\n");

	uint8_t handshake = 4;

	send(cliente1, &handshake, sizeof(uint8_t), 0);

	return cliente1;
}

int levantar_puerto_interrupt(t_config* config, int* socket_interrupt) {
	char* puerto_interrupt = config_get_string_value(config,
			"PUERTO_ESCUCHA_INTERRUPT");
	printf("\nPuerto interrupt %s", puerto_interrupt);

	*socket_interrupt = iniciar_servidor("127.0.0.1", puerto_interrupt);
	printf("\nSocket %d", *socket_interrupt);

	int cliente2 = esperar_cliente(*socket_interrupt);
	printf("\nCliente: %d", cliente2);

	uint8_t mensaje1 = 0;

	recv(cliente2, &mensaje1, sizeof(uint8_t), 0);
	printf("\nMensaje recibido interrupt: %d", mensaje1);
	printf("\n");

	uint8_t handshake1 = 5;
	send(cliente2, &handshake1, sizeof(uint8_t), 0);

	return cliente2;
}

//Intruccion fetch(){
//
//}

int execute(Instruccion* instruccion) {
	uint8_t id = instruccion->id;
	t_list* parametros = instruccion->parametros;

	switch (id) {
	case 1:
		return 0;
		break;
	case 2:
		return 1;
		break;
	case 3:
		return 0;
		break;
	case 4:
		return 0;
		break;
	case 5:
		return 0;
		break;
	case 6:
		return 1;
		break;
	default:
		printf("\n Se ingreso una operacion incorrecta");
		return -1;
		break;
	}
}

int check_interrupt(){
	return 0;
}

uint8_t ejecutar_ciclo_instrucciones(Pcb* pcb) {
	t_list* instrucciones = pcb->instrucciones;
	uint32_t program_counter = pcb->program_counter;

	//fetch
	Instruccion* instruccion = list_get(instrucciones,program_counter);
	program_counter++;
	pcb->program_counter = program_counter;

	//decode
	bool requiere_fetch_operands = false;
	if (instruccion->id == 3) {
		requiere_fetch_operands = true;
	}

	if (requiere_fetch_operands) {
		// TODO: Ir a buscar los fetch operands, seria solo en la instruccion copy, va a ser una llamada a memoria
		printf("\nES UNA INSTRUCCION COPY");
		printf("\n");
	}

	int flag = execute(instruccion);

	if (flag != 0) {
		return 1;
	} else {
		return check_interrupt();
	}
}

int main(void) {
	t_config* config = config_create("cpu.config");
//	int socket_dispatch = 0;
//	int socket_interrupt = 0;

	int conexion_memoria = levantar_conexion_memoria(config);
//	int kernel_dispatch = levantar_canal_dispatch(config, &socket_dispatch);
//	int kernel_interrupt = levantar_canal_dispatch(config, &socket_interrupt);

//	Dispatch
	char* puerto_dispatch = config_get_string_value(config,
			"PUERTO_ESCUCHA_DISPATCH");
	printf("\nPuerto escucha %s", puerto_dispatch);

	int socket_dispatch = iniciar_servidor("127.0.0.1", puerto_dispatch);
	printf("\nSocket %d", socket_dispatch);

	int kernel_dispatch = esperar_cliente(socket_dispatch);
	printf("\nCliente: %d", kernel_dispatch);
	printf("\n");

	uint8_t mensaje = 0;
	recv(kernel_dispatch, &mensaje, sizeof(uint8_t), 0);
	printf("Mensaje recibido dispatch: %d", mensaje);
	printf("\n");

	uint8_t handshake = 4;

	send(kernel_dispatch, &handshake, sizeof(uint8_t), 0);

//	Interrupt
//	char* puerto_interrupt = config_get_string_value(config,
//			"PUERTO_ESCUCHA_INTERRUPT");
//	printf("\nPuerto interrupt %s", puerto_interrupt);
//
//	int socket_interrupt = iniciar_servidor("127.0.0.1", puerto_interrupt);
//	printf("\nSocket %d", socket_interrupt);
//
//	int cliente2 = esperar_cliente(socket_interrupt);
//	printf("\nCliente: %d", cliente2);
//
//	uint8_t mensaje1 = 0;
//
//	recv(cliente2, &mensaje1, sizeof(uint8_t), 0);
//	printf("\nMensaje recibido interrupt: %d", mensaje1);
//	printf("\n");
//
//	uint8_t handshake1 = 5;
//	send(cliente2, &handshake1, sizeof(uint8_t), 0);

//Recibir pcb de kernel

//	Pcb* pcb = obtener_pcb(socket_dispatch, kernel_dispatch);
	Pcb* pcb = obtener_pcb(socket_dispatch, kernel_dispatch);
	pcb_mostrar(pcb);

	uint8_t flag = 0;
	while (flag == 0) {
		flag = ejecutar_ciclo_instrucciones(pcb);
	}

	//devolver_pcb_kernel
	printf("HAY QUE DEVOLVER EL PCB AL KERNEL");
	printf("\n");

//	void* stream_pcb = pcb_armar_stream(pcb);
//	uint32_t tamano_pcb = pcb_calcular_espacio(pcb);
//
//	t_buffer* buffer = armar_buffer(tamano_pcb, stream_pcb);
//	t_paquete* paquete = empaquetar_buffer(buffer,0);
//
//	void* a_enviar = malloc(paquete->size);
//	a_enviar = serializar_paquete(paquete, a_enviar);
//
////	Enviar PCB
//	send(kernel_dispatch, a_enviar, paquete->size, 0);


	close(socket_dispatch);
//	close(socket_interrupt);
	close(conexion_memoria);
	return EXIT_SUCCESS;
}
