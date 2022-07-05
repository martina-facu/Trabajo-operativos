#include "utils.h"

pcb_t* obtener_pcb(int cliente)
{

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

	pcb_t* pcb = pcb_deserializar(buffer);





	return pcb;
}


int levantar_conexion_memoria(char* ipServer, char* portServer, t_log* logger, uint32_t* cantidad_entradas,uint32_t* tamano_pagina)
{
	int conexion_memoria = crear_conexion(ipServer, portServer, logger);
	log_info(logger, "Se intenta conectarse con la memoria en la IP %s y puerto %s con el descriptor:  %d", ipServer, portServer, conexion_memoria);


	uint8_t handshake_memoria = INICIAR_CONEXION_CPU;
	send(conexion_memoria, &handshake_memoria, sizeof(uint8_t), 0);
	log_info(logger, "Se envia Handshake a la memoria");

	uint8_t respuesta_memoria = 0;
	recv(conexion_memoria, &respuesta_memoria, sizeof(uint8_t), 0);
	log_info(logger, "Mensaje recibido de la memoria:  %d", respuesta_memoria);

	if(respuesta_memoria == ACEPTAR_CONEXION_CPU)
		log_info(logger, "Conexion establecida con la memoria");
	else
	{
		log_error(logger, "No se puede establecer conexion con la memoria. El server contactado responde con un mensaje no predeterminado: %d.", respuesta_memoria);
		exit(EXIT_FAILURE);
	}
	//	TODO: descomentar estas lineas cuando se implemente la parte de la memoria
	//	t_paquete* respuesta = recibir_mensaje_memoria(conexion_memoria);
	//	void* stream = respuesta->buffer->stream;
	//
	//	memcpy(cantidad_entradas, stream, sizeof(uint32_t));
	//	stream += sizeof(uint32_t);
	//	memcpy(tamano_pagina, stream, sizeof(uint32_t));

	return conexion_memoria;
}


int levantar_server(char* ipServer, char* portServer, t_log* logger, char* sTipo)
{
	int socket;

	//	Inicio el servidor en la IP y puertos leidos desde el archivo de configuracion
	socket = iniciar_servidor(ipServer, portServer, logger);
	log_info(logger, "Socket en el que se levanta el server %s: %d en la IP: %s y Puerto: %s",sTipo, socket, ipServer, portServer);

	return socket;
}

t_paquete* recibir_mensaje_memoria(int conexion_memoria, t_log* logger)
{
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

bool validar_codigo(t_paquete* paquete, uint8_t operacion, t_log* logger)
{
	if(paquete->codigo_operacion == operacion){
		printf("\nCodigo de operacion incorrecto, se esperaba %d, se recibio %d",operacion,paquete->codigo_operacion);
		return true;
	}else{
		return false;
	}
}

void mandar_lecto_escritura(uint32_t direccion, uint32_t* valor, uint8_t operacion, int conexion, t_log* logger)
{
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

uint32_t* leer(int direccion_logica, Datos_calculo_direccion* datos, t_log* logger)
{
	calcular_datos_direccion(datos, direccion_logica);
	Pagina_direccion* resultado = traducir_direccion(datos);

	mandar_lecto_escritura(resultado->direccion_fisica, 0, 9, datos->conexion_memoria, logger);

	t_paquete* respuesta = recibir_mensaje_memoria(datos->conexion_memoria, logger);
	uint32_t* valor_leido = malloc(sizeof(uint32_t));

	memcpy(valor_leido, respuesta->buffer->stream, sizeof(uint32_t));

	if(validar_codigo(respuesta,10, logger))
	{
		printf("El valor leido: %d", *valor_leido);
		return valor_leido;
	}else{
		return (uint32_t*)-1;
	}
}

uint32_t* escribir(int direccion_logica, uint32_t* valor_a_escribir, Datos_calculo_direccion* datos, t_log* logger)
{
	calcular_datos_direccion(datos, direccion_logica);
	Pagina_direccion* resultado = traducir_direccion(datos);
	printf("El valor de la direccion fisica es: %d", resultado->direccion_fisica);

	mandar_lecto_escritura(resultado->direccion_fisica, valor_a_escribir, 11, datos->conexion_memoria, logger);

	t_paquete* respuesta = recibir_mensaje_memoria(datos->conexion_memoria, logger);
	uint32_t* resultado_escritura = malloc(sizeof(uint32_t));

	memcpy(resultado_escritura, respuesta->buffer->stream, sizeof(uint32_t));

	if(validar_codigo(respuesta,10, logger)){
		printf("El resultado de escribir fue: %d", *resultado_escritura);
		return resultado_escritura;
	}else{
		return (uint32_t*)-1;
	}
}

//bool execute(Instruccion* instruccion,t_config* config, pcb_t* pcb, t_log* logger)
bool execute(Instruccion* instruccion,int dormir, int cantidad_entradas, int conexion_memoria, int tamano_pagina, pcb_t* pcb, t_log* logger)
{
	uint8_t id = instruccion->id;
	t_list* parametros = instruccion->parametros;

	Datos_calculo_direccion* datos = malloc(sizeof(Datos_calculo_direccion));
	datos->id_tabla_paginas1 = pcb->pid;
	datos->conexion_memoria = conexion_memoria;
	datos->entradas_por_tabla = cantidad_entradas;
	datos->tamano_pagina = tamano_pagina;

	int catidad_parametros = getCantidadParametros(id);
	uint32_t* parametro1;
	uint32_t* parametro2;

	if(catidad_parametros == 1){
		parametro1 = list_get(parametros,0);
	}else if(catidad_parametros >= 1){
		parametro1 = list_get(parametros,0);
		parametro2 = list_get(parametros,1);
	}

	uint32_t* resultado;
	uint32_t* valor_leido;

	switch (id)
	{
		case NO_OP:
			log_info(logger, "CPU-EXECUTE Proceso una operacion de NO_OP me preparo para dormir %d", dormir/1000);
			dormir = dormir/1000;
			sleep(dormir);
			//	Esto es para hacer mas lenta la ejecucion y poder seguirlo por log
			log_info(logger, "Duermo 5 segundos antes de la siguiente operacion");
			sleep(5);
			return false;
			break;
		case I_O:
			log_info(logger, "CPU-EXECUTE Proceso una operacion de I/0");
			pcb->estado = BLOQUEADO;
			pcb->tiempo_block = *parametro1;
			//	Esto es para hacer mas lenta la ejecucion y poder seguirlo por log
			log_info(logger, "Duermo 5 segundos antes de la siguiente operacion");
			sleep(5);
			return true;
			break;
		case WRITE:
			log_info(logger, "CPU-EXECUTE Proceso una operacion de WRITE");
//			resultado = escribir(*parametro1, parametro2, datos, logger);
//			*resultado == -1? printf("Fallo la escritura") : printf("Escritura exitosa");
	//		free(resultado);
			//	Esto es para hacer mas lenta la ejecucion y poder seguirlo por log
			log_info(logger, "Duermo 5 segundos antes de la siguiente operacion");
			sleep(5);
			return false;
			break;
		case COPY: // COPY(destino, origen)
			log_info(logger, "CPU-EXECUTE Proceso una operacion de COPY");
//			valor_leido = leer(*parametro2,datos, logger);
//			*parametro2 = *valor_leido;
	//		free(valor_leido);
//			resultado = escribir(*parametro1, parametro2, datos, logger);
//			*resultado == -1? printf("Fallo la escritura") : printf("Escritura exitosa");
			//	Esto es para hacer mas lenta la ejecucion y poder seguirlo por log
			log_info(logger, "Duermo 5 segundos antes de la siguiente operacion");
			sleep(5);
			return false;
			break;
		case READ:
			log_info(logger, "CPU-EXECUTE Proceso una operacion de READ");
//			valor_leido = leer(*parametro1,datos, logger);
	//		free(valor_leido);
			//	Esto es para hacer mas lenta la ejecucion y poder seguirlo por log
			log_info(logger, "Duermo 5 segundos antes de la siguiente operacion");
			sleep(5);
			return false;
			break;
		case EXIT:
			log_info(logger, "CPU-EXECUTE Proceso una operacion de EXIT");
			//	Esto es para hacer mas lenta la ejecucion y poder seguirlo por log
			pcb->estado = FINALIZADO;
			log_info(logger, "Duermo 5 segundos antes de la siguiente operacion");
			sleep(5);
			return true;
			break;
		default:
			printf("\nCPU-EXECUTE HUBO UN FALLO EN LA EJECUCION DE LAS INSTRUCCIONES");
			//	Esto es para hacer mas lenta la ejecucion y poder seguirlo por log
			log_info(logger, "Duermo 5 segundos antes de la siguiente operacion");
			sleep(5);
			return true;
			break;
	}
//	//	Esto es para hacer mas lenta la ejecucion y poder seguirlo por log
//	log_info(logger, "Duermo 5 segundos antes de la siguiente operacion");
//	sleep(5);
}

void ejecutar_ciclo_instrucciones(pcb_t* pcb, bool* devolver_pcb, int retardoNoOp, int cantidad_entradas, int conexion_memoria, int tamano_pagina, bool* hubo_interrupcion, t_log* logger)
{
	t_list* instrucciones = pcb->instrucciones;
	uint32_t program_counter = pcb->program_counter;

	//fetch
	Instruccion* instruccion = list_get(instrucciones,program_counter);
	program_counter++;
	pcb->program_counter = program_counter;

	log_info(logger, "CPU-EXECUTE Aumento el program counter del PCB a: %d", pcb->program_counter);

	//decode
	bool requiere_fetch_operands = false;
	if (instruccion->id == COPY)
		requiere_fetch_operands = true;

	//fetch_operands
	if (requiere_fetch_operands)
	{
		// 	TODO: Ir a buscar los fetch operands, seria solo en la instruccion copy.
		//	Va a ser una llamada a memoria
//		printf("\nES UNA INSTRUCCION COPY");
		log_info(logger, "Es una INSTRUCCION COPY");
//		printf("\n");
	}

	//	Ejecuto la instruccion
	*devolver_pcb = execute(instruccion, retardoNoOp, cantidad_entradas, conexion_memoria, tamano_pagina, pcb, logger);
	log_info(logger, "Valor de devolver PCBt %s", *devolver_pcb?"true":"false");
	//check interrupt
	if (*devolver_pcb == false)
		*devolver_pcb = *hubo_interrupcion;
}




//bool execute(Instruccion* instruccion,int dormir, pcb_t* pcb)
//{ // TODO : encapsular la logica de las instrucciones mas complicadas
//	uint8_t id = instruccion->id;
//	t_list* parametros = instruccion->parametros;
//
//	int catidad_parametros = getCantidadParametros(id);
//	int* parametro1,parametro2 = 0;
//
//	if(catidad_parametros == 1){
//		parametro1 = list_get(parametros,0);
//	}else if(catidad_parametros >= 1){
//		parametro1 = list_get(parametros,0);
//		parametro2 = list_get(parametros,1);
//	}
//
////	int dormir = config_get_int_value(config,"RETARDO_NOOP");
//
//	switch (id)
//	{
//		case 1:
//			dormir = dormir/1000;
//			sleep(dormir);
//			return false;
//			break;
//		case 2:
//			pcb->estado = BLOQUEADO;
//			pcb->tiempo_block = *parametro1;
//			return true;
//			break;
//		case 3: // TODO : Implementar la instruccion WRITE
//			return false;
//			break;
//		case 4: // TODO : Implementar la instruccion COPY
//			return false;
//			break;
//		case 5: // TODO : Implementar la instruccion READ
//			return false;
//			break;
//		case 6:
//			return true;
//			break;
//		default:
//			printf("\nHUBO UN FALLO EN LA EJECUCION DE LAS INSTRUCCIONES");
//			return true;
//			break;
//	}
//}

