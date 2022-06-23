#include "utils_CPU.h"

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

int levantar_conexion_memoria(t_config* config, uint32_t* cantidad_entradas,uint32_t* tamano_pagina) {
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

//	TODO: descomentar estas lineas cuando se implemente la parte de la memoria
//	recv(conexion_memoria, cantidad_entradas, sizeof(uint32_t), 0);
//	recv(conexion_memoria, tamano_pagina, sizeof(uint32_t), 0);

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

int leer(int direccion_logica, Datos_calculo_direccion* datos){
	calcular_datos_direccion(datos, direccion_logica);

	return 0;
}

int escribir(int direccion_logica){
	return 0;
}

bool execute(Instruccion* instruccion,t_config* config, Pcb* pcb) {
	uint8_t id = instruccion->id;
	t_list* parametros = instruccion->parametros;

	Datos_calculo_direccion* datos = malloc(sizeof(Datos_calculo_direccion));
	datos->id_tabla_paginas1 = pcb->pid;
	datos->conexion_memoria = 0; //va a ser variable global
	datos->entradas_por_tabla = 4; //va a ser variable global
	datos->tamano_pagina = 100; //va a ser variable global

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
	case NO_OP:
		dormir = dormir/1000;
		sleep(dormir);
		return false;
		break;
	case I_O:
		pcb->estado = BLOQUEADO;
		pcb->tiempo_bloqueo = *parametro1;
		return true;
		break;
	case WRITE: // TODO : Implementar la instruccion WRITE
//		int resultado = write(instruccion);
		return false;
		break;
	case COPY: // TODO : Implementar la instruccion COPY
		return false;
		break;
	case READ: // TODO : Implementar la instruccion READ
//		int valor_leido = read(instruccion);

		return false;
		break;
	case EXIT:
		return true;
		break;
	default:
		printf("\nHUBO UN FALLO EN LA EJECUCION DE LAS INSTRUCCIONES");
		return true;
		break;
	}
}

