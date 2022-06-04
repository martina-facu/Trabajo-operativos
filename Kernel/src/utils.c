#include "utils.h"

t_list* deserializar_mensaje(int socket_serv, int cliente, uint32_t* tamano_proceso) {

	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->buffer = malloc(sizeof(t_buffer));
	t_buffer* buffer = paquete->buffer;

	//recibimos el codigo del tipo de mensaje que nos llega
	recv(cliente, &(paquete->codigo_operacion), sizeof(uint8_t), 0);

	//recibo el tamaño del paquete
	recv(cliente, &(buffer->size), sizeof(uint32_t), 0);

	//recibo el buffer con las instrucciones
	buffer->stream = malloc(buffer->size);
	recv(cliente, buffer->stream, buffer->size, 0);

	t_list* instrucciones = list_create();
	deserializar_instrucciones(buffer, instrucciones);

	recv(cliente, tamano_proceso, sizeof(uint32_t), 0);

	return instrucciones;
}

void avisar_proceso_finalizado(int cliente) {
	uint8_t recibido = 1;
	send(cliente, &recibido, sizeof(uint8_t), 0);
}

Pcb* crear_pcb(t_list* instrucciones, uint32_t tamano_proceso, t_config* config, uint32_t id_proceso) {
	//TODO: get_tabla_paginas

	double estimacion_inicial = config_get_double_value(config, "ESTIMACION_INICIAL");

	Tabla_paginas *tabla_paginas = malloc(sizeof(Tabla_paginas));
	tabla_paginas->desplazamiento = 0;
	tabla_paginas->entrada_tabla_primer_nivel = 0;
	tabla_paginas->entrada_tabla_segundo_nivel = 0;
	tabla_paginas->numero_pagina = 0;

	Pcb* pcb = pcb_create(id_proceso, tamano_proceso,
			tabla_paginas, estimacion_inicial, instrucciones);
	return pcb;
}

int levantar_conexion_memoria(t_config* config){
	char * ip_memoria = malloc(sizeof(char) * 30);
		strcpy(ip_memoria, config_get_string_value(config, "IP_MEMORIA"));
		printf("\nIp de la memoria: %s", ip_memoria);

		char* puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
		int conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);

		uint8_t handshake_memoria = 3;
		send(conexion_memoria, &handshake_memoria, sizeof(uint8_t), 0);

		uint8_t respuesta_memoria = 0;
		recv(conexion_memoria, &respuesta_memoria, sizeof(uint8_t), 0);
		printf("\nMensaje recibido de la memoria: %d", respuesta_memoria);

		return conexion_memoria;
}

int levantar_conexion_dispacher(t_config* config){
	char * ip_cpu = malloc(sizeof(char) * 30);
	strcpy(ip_cpu, config_get_string_value(config, "IP_CPU"));
	printf("\nIp cpu: %s", ip_cpu);

//	Dispatch
	char* puerto_dispatch = config_get_string_value(config,"PUERTO_CPU_DISPATCH");
	printf("\nPuerto dispatch: %s", puerto_dispatch);

	int cpu_dispatch = crear_conexion(ip_cpu, puerto_dispatch);
	printf("\nConexion dispatch: %d", cpu_dispatch);

	uint8_t handshake = 2;

	send(cpu_dispatch, &handshake, sizeof(uint8_t), 0);

	uint8_t respuesta = 0;
	recv(cpu_dispatch, &respuesta, sizeof(uint8_t), 0);
	printf("\nMensaje recibido dispatch: %d", respuesta);

	return cpu_dispatch;
}

int levantar_conexion_interrupt(t_config* config){
	char * ip_cpu = malloc(sizeof(char) * 30);
	strcpy(ip_cpu, config_get_string_value(config, "IP_CPU"));
	printf("\nIp cpu: %s", ip_cpu);

//	Interrupt
	char* puerto_interrupt = config_get_string_value(config,
			"PUERTO_CPU_INTERRUPT");
	int cpu_interrupt = crear_conexion(ip_cpu, puerto_interrupt);
	uint8_t handshake1 = 3;
	send(cpu_interrupt, &handshake1, sizeof(uint8_t), 0);

	uint8_t respuesta1 = 0;
	recv(cpu_interrupt, &respuesta1, sizeof(uint8_t), 0);
	printf("\nMensaje recibido interrupt: %d", respuesta1);

	return cpu_interrupt;
}
