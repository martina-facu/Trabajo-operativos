#include "utils_CPU.h"

void inicializar_utils(t_config* config, int conexion_m, bool* interrupcion){
	conexion_memoria  = conexion_m;
	cantidad_entradas = 0;
	tamano_pagina = 0;
	hubo_interrupcion = interrupcion;
	config_cpu = config;
}

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

t_paquete* recibir_mensaje_memoria(){
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->buffer = malloc(sizeof(t_buffer));
	t_buffer* buffer = paquete->buffer;

	//recibimos el codigo del tipo de mensaje que nos llega
	recv(conexion_memoria, &(paquete->codigo_operacion), sizeof(uint8_t), 0);

	//recibo el tamaño del paquete
	recv(conexion_memoria, &(buffer->size), sizeof(uint32_t), 0);

	//recibo el buffer con el pcb
	buffer->stream = malloc(buffer->size);
	recv(conexion_memoria, buffer->stream, buffer->size, 0);

	return paquete;
}

bool validar_codigo(t_paquete* paquete, uint8_t operacion){
	if(paquete->codigo_operacion == operacion){
		printf("\nCodigo de operacion incorrecto, se esperaba %d, se recibio %d",operacion,paquete->codigo_operacion);
		return true;
	}else{
		return false;
	}
}

void mandar_lecto_escritura(uint32_t direccion, uint32_t* valor, uint8_t operacion, int conexion){
	void* stream;
	int tamano_mensaje = 0;

	if(operacion == 9){
		tamano_mensaje = sizeof(uint32_t);
		stream = malloc(tamano_mensaje);
		memcpy(stream, &direccion, sizeof(uint32_t));
	}else if(operacion == 11){
		tamano_mensaje = sizeof(uint32_t)*2;
		stream = malloc(tamano_mensaje);
		memcpy(stream, &direccion, sizeof(uint32_t));
		memcpy(stream + sizeof(uint32_t), valor, sizeof(uint32_t));
	}

	t_buffer* buffer = armar_buffer(tamano_mensaje, stream);
	t_paquete* paquete = empaquetar_buffer(buffer,9);

	void* a_enviar = malloc(paquete->size);
	a_enviar = serializar_paquete(paquete, a_enviar);

	send(conexion, a_enviar, paquete->size, 0);
}

uint32_t* leer(int direccion_logica, Datos_calculo_direccion* datos){
	calcular_datos_direccion(datos, direccion_logica);
	Pagina_direccion* resultado = traducir_direccion(datos);

	mandar_lecto_escritura(resultado->direccion_fisica, 0, 9, datos->conexion_memoria);

	t_paquete* respuesta = recibir_mensaje_memoria(datos->conexion_memoria);
	uint32_t* valor_leido = malloc(sizeof(uint32_t));

	memcpy(valor_leido, respuesta->buffer->stream, sizeof(uint32_t));

	if(validar_codigo(respuesta,10)){
		printf("El valor leido: %d", *valor_leido);
		return valor_leido;
	}else{
		return (uint32_t*)-1;
	}
}

uint32_t* escribir(int direccion_logica, uint32_t* valor_a_escribir, Datos_calculo_direccion* datos){
	calcular_datos_direccion(datos, direccion_logica);
	Pagina_direccion* resultado = traducir_direccion(datos);
	printf("El valor de la direccion fisica es: %d", resultado->direccion_fisica);

	mandar_lecto_escritura(resultado->direccion_fisica, valor_a_escribir, 11, datos->conexion_memoria);

	t_paquete* respuesta = recibir_mensaje_memoria(datos->conexion_memoria);
	uint32_t* resultado_escritura = malloc(sizeof(uint32_t));

	memcpy(resultado_escritura, respuesta->buffer->stream, sizeof(uint32_t));

	if(validar_codigo(respuesta,10)){
		printf("El resultado de escribir fue: %d", *resultado_escritura);
		return resultado_escritura;
	}else{
		return (uint32_t*)-1;
	}
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
	uint32_t* parametro1;
	uint32_t* parametro2;

	if(catidad_parametros == 1){
		parametro1 = list_get(parametros,0);
	}else if(catidad_parametros >= 1){
		parametro1 = list_get(parametros,0);
		parametro2 = list_get(parametros,1);
	}

	int dormir = config_get_int_value(config,"RETARDO_NOOP");
	uint32_t* resultado;
	uint32_t* valor_leido;

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
	case WRITE:
		resultado = escribir(*parametro1, parametro2, datos);
		*resultado == -1? printf("Fallo la escritura") : printf("Escritura exitosa");
//		free(resultado);
		return false;
		break;
	case COPY: // COPY(destino, origen)
		valor_leido = leer(*parametro2,datos);
		*parametro2 = *valor_leido;
//		free(valor_leido);
		resultado = escribir(*parametro1, parametro2, datos);
		*resultado == -1? printf("Fallo la escritura") : printf("Escritura exitosa");
		return false;
		break;
	case READ:
		valor_leido = leer(*parametro1,datos);
//		free(valor_leido);
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

void ejecutar_ciclo_instrucciones(Pcb* pcb,t_config* config, bool* devolver_pcb) {
	t_list* instrucciones = pcb->instrucciones;
	uint32_t program_counter = pcb->program_counter;

	//fetch
	Instruccion* instruccion = list_get(instrucciones,program_counter);
	program_counter++;
	pcb->program_counter = program_counter;

	//decode
	bool requiere_fetch_operands = false;
	if (instruccion->id == COPY) {
		requiere_fetch_operands = true;
	}

	//fetch_operands
	if (requiere_fetch_operands) {
		// TODO: Ir a buscar los fetch operands, seria solo en la instruccion copy, va a ser una llamada a memoria
		printf("\nES UNA INSTRUCCION COPY");
		printf("\n");
	}

	//execute
	*devolver_pcb = execute(instruccion,config,pcb);

	//check interrupt
	if (*devolver_pcb == false) {
		*devolver_pcb = *hubo_interrupcion;
	}
}
