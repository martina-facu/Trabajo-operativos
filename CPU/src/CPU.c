#include <stdio.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/config.h>
#include <conexion.h>
#include <pcb.h>
#include <paquete.h>

bool interrupcion = false;

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

int execute(Instruccion* instruccion,t_config* config) { // TODO : encapsular la logica de las instrucciones mas complicadas
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

	int dormir = config_get_int_value(config,"RETARDO_NOOP");

	switch (id) {
	case 1:
		dormir = dormir/1000;
		sleep(dormir);
		return 0;
		break;
	case 2:
		return *parametro1;
		break;
	case 3: // TODO : Implementar la instruccion WRITE
		return 0;
		break;
	case 4: // TODO : Implementar la instruccion COPY
		return 0;
		break;
	case 5: // TODO : Implementar la instruccion READ
		return 0;
		break;
	case 6:
		return 1;
		break;
	default:
		printf("\nHUBO UN FALLO EN LA EJECUCION DE LAS INSTRUCCIONES");
		return -1;
		break;
	}
}

int check_interrupt(){
	return interrupcion;
}

void ejecutar_ciclo_instrucciones(Pcb* pcb,t_config* config, uint32_t* flag) {
	t_list* instrucciones = pcb->instrucciones;
	uint32_t program_counter = pcb->program_counter;

	//fetch
	Instruccion* instruccion = list_get(instrucciones,program_counter);
	program_counter++;
	pcb->program_counter = program_counter;

	//decode
	bool requiere_fetch_operands = false;
	if (instruccion->id == 4) {
		requiere_fetch_operands = true;
	}

	//fetch_operands
	if (requiere_fetch_operands) {
		// TODO: Ir a buscar los fetch operands, seria solo en la instruccion copy, va a ser una llamada a memoria
		printf("\nES UNA INSTRUCCION COPY");
		printf("\n");
	}

	*flag = execute(instruccion,config);

	if (*flag == 0) {
		*flag = check_interrupt();
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

	Pcb* pcb = obtener_pcb(socket_dispatch, kernel_dispatch);
	pcb_mostrar(pcb);

//	Ejecutar ciclo de instrucciones
	uint32_t flag = 0;
	while (flag == 0) {
		ejecutar_ciclo_instrucciones(pcb,config,&flag);
	}

//	DEVOLVER PCB AL KERNEL
	uint32_t* tamano_mensaje = malloc(sizeof(uint32_t));

	if(flag == 1){
		void* a_enviar = pcb_serializar(pcb,tamano_mensaje,1);
		send(kernel_dispatch, a_enviar, *tamano_mensaje, 0);

	}else{
		uint32_t* tiempo_bloqueo = malloc(sizeof(uint32_t));
		*tiempo_bloqueo = flag;

		void* a_enviar = pcb_serializar(pcb,tamano_mensaje,2);
		memcpy(a_enviar + *tamano_mensaje, tiempo_bloqueo, sizeof(uint32_t));

		*tamano_mensaje = *tamano_mensaje + sizeof(uint32_t);

		send(kernel_dispatch, a_enviar, *tamano_mensaje, 0);
	}


	close(socket_dispatch);
//	close(socket_interrupt);
	close(conexion_memoria);
	return EXIT_SUCCESS;
}
