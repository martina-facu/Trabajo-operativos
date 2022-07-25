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

	pcb_t* pcb = pcb_deserializar(buffer, logger);

	return pcb;
}


int levantar_conexion_memoria_CPU(char* ipServer, char* portServer, uint32_t* cantidad_entradas,uint32_t* tamano_pagina)
{
	int conexion_memoria = crear_conexion(ipServer, portServer, logger);
	log_trace(logger, "CPU-COMUNICACION-MEMORIA Se intenta conectarse con la memoria en la IP %s y puerto %s con el descriptor:  %d", ipServer, portServer, conexion_memoria);


	uint8_t handshake_memoria = INICIAR_CONEXION_CPU;
	send(conexion_memoria, &handshake_memoria, sizeof(uint8_t), 0);
	log_trace(logger, "CPU-COMUNICACION-MEMORIA Se envia Handshake a la memoria");

	uint8_t respuesta_memoria = 0;
	recv(conexion_memoria, &respuesta_memoria, sizeof(uint8_t), 0);
	log_trace(logger, "CPU-COMUNICACION-MEMORIA Mensaje recibido de la memoria:  %d", respuesta_memoria);

	if(respuesta_memoria == ACEPTAR_CONEXION_CPU)
		log_trace(logger, "CPU-COMUNICACION-MEMORIA Conexion establecida con la memoria");
	else
	{
		log_error(logger, "CPU-COMUNICACION-MEMORIA No se puede establecer conexion con la memoria. El server contactado responde con un mensaje no predeterminado: %d.", respuesta_memoria);
		exit(EXIT_FAILURE);
	}
		uint8_t mensaje = SOLICITAR_ENTRADA_Y_TAMANO;
		send(conexion_memoria, &mensaje, sizeof(uint8_t), 0);

		recv(conexion_memoria, &cant, sizeof(uint32_t), 0);
		recv(conexion_memoria, &tam, sizeof(uint32_t), 0);

		log_trace(logger, "CPU-COMUNICACION-MEMORIA Cantidad de entradas %d", cant);
		log_trace(logger, "CPU-COMUNICACION-MEMORIA Tamano_pagina %d", tam);

	return conexion_memoria;
}


int levantar_server(char* ipServer, char* portServer, char* sTipo)
{
	int socket;

	//	Inicio el servidor en la IP y puertos leidos desde el archivo de configuracion
	socket = iniciar_servidor(ipServer, portServer, logger);
	log_trace(logger, "CPU-COMUNICACION-KERNEL Socket en el que se levanta el server %s: %d en la IP: %s y Puerto: %s",sTipo, socket, ipServer, portServer);

	return socket;
}

t_paquete* recibir_mensaje_memoria(int conexion_memoria)
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

bool validar_codigo(t_paquete* paquete, uint8_t operacion)
{
	if(paquete->codigo_operacion == operacion)
	{
		log_error(logger, "CPU-EXECUTE Codigo de operacion incorrecto, se esperaba %d, se recibio %d",operacion,paquete->codigo_operacion);
		return true;
	}else{
		return false;
	}
}

uint32_t mandar_lecto_escritura(uint32_t direccion, uint32_t* valor, uint8_t operacion, int conexion)
{
	uint32_t valorRecibido;


	uint8_t lectura = SOLICITAR_LECTURA;
	uint8_t escritura = SOLICITAR_ESCRITURA;


	if(operacion == SOLICITAR_LECTURA)
	{
		//	Envio operacion de lectura
		send(cliente_memoria,&lectura,sizeof(uint8_t),0);
		send(cliente_memoria, &direccion, sizeof(uint32_t),0);
		log_trace(logger, "CPU-MEMORIA Envie solicitud de Lectura");

		//	Recibo valor leido de la memoria
		recv(cliente_memoria,&valorRecibido,sizeof(uint32_t),0);
		log_info(logger, "CPU-MEMORIA Lei la posicion de memoria: %d", valorRecibido);
	}
	else if(operacion == SOLICITAR_ESCRITURA)
	{
		//	Envio operacion de Escritura
		send(cliente_memoria,&escritura,sizeof(uint8_t),0);
		send(cliente_memoria,&direccion, sizeof(uint32_t),0);
		send(cliente_memoria, valor, sizeof(uint32_t),0);
		//	Recibo resultado de la escritura
		recv(cliente_memoria,&valorRecibido,sizeof(uint32_t),0);
		log_info(logger, "CPU-MEMORIA Se recibio el resultado de la operacion de escritura");
	}

	return valorRecibido;
}

uint32_t leer(uint32_t direccion_logica, Datos_calculo_direccion* datos)
{
	uint32_t valorRecibido;

	log_info(logger, "CPU-MEMORIA Voy a leer valor de memoria");
//	uint32_t valor_leido_respuesta;
	log_trace(logger, "CPU-MEMORIA ----- Direccion logica %d", direccion_logica);
	// Voy a calcular los datos de la direccion
	calcular_datos_direccion(datos, direccion_logica);
	log_trace(logger, "CPU-MEMORIA ----- Calcule datos de direccion");
	Pagina_direccion* resultado = traducir_direccion(datos);
	log_trace(logger, "CPU-MEMORIA ----- Traduje los datos");
	log_info(logger, "CPU-MEMORIA ----- DIRECCION FISICA: %d ----- ", resultado->direccion_fisica);


	valorRecibido = mandar_lecto_escritura(resultado->direccion_fisica, 0, SOLICITAR_LECTURA, datos->conexion_memoria);
	log_info(logger, "CPU-MEMORIA Envie pedido de lectura a la Memoria");
	log_info(logger, "CPU-MEMORIA -------- LECTURA: %d -------- ", valorRecibido);
	return valorRecibido;
}

uint32_t escribir(int direccion_logica, uint32_t* valor_a_escribir, Datos_calculo_direccion* datos)
{
	calcular_datos_direccion(datos, direccion_logica);
	log_trace(logger, "CPU-MEMORIA-EXECUTE Calculo direccion logica");
	Pagina_direccion* resultado = traducir_direccion(datos);
	log_trace(logger, "CPU-MEMORIA-EXECUTE Realizo la traduccion de direccion logica a fisica");

	log_info(logger, "CPU-MEMORIA ----- DIRECCION FISICA: %d ----- ", resultado->direccion_fisica);

	mandar_lecto_escritura(resultado->direccion_fisica, valor_a_escribir, SOLICITAR_ESCRITURA, datos->conexion_memoria);

	return 0;
}

//bool execute(Instruccion* instruccion,t_config* config, pcb_t* pcb, t_log* logger)
bool execute(Instruccion* instruccion,int dormir, Datos_calculo_direccion* datos, pcb_t* pcb)
{
//	mostrar_datos(datos);
	uint8_t id = instruccion->id;
	t_list* parametros = instruccion->parametros;

	int catidad_parametros = getCantidadParametros(id);
	uint32_t* parametro1;
	uint32_t* parametro2;


	if(catidad_parametros == 1){
		parametro1 = list_get(parametros,0);
//		log_trace(logger, "CPU-EXECUTE Obtengo lista de parametros de la instruccion: Parametro 1: %d\tParametro 2: %d", *parametro1);
	}else if(catidad_parametros >= 1){
		parametro1 = list_get(parametros,0);
		parametro2 = list_get(parametros,1);
//		log_trace(logger, "CPU-EXECUTE Obtengo lista de parametros de la instruccion: Parametro 1: %d\tParametro 2: %d", *parametro1,*parametro2);
	}



	uint32_t resultado = -1;
	uint32_t valor_leido= -1;

	switch (id)
	{
		case NO_OP:
			log_info(logger, "CPU-EXECUTE ------------------- OPERACION NO_OP PID: %d -------------------", dormir/1000, pcb->pid);
			dormir = dormir/1000;
			sleep(dormir);

			return false;
			break;
		case I_O:
			log_info(logger, "CPU-EXECUTE ------------------- OPERACION I/0 PID: %d -------------------", pcb->pid);
			pcb->estado = BLOQUEADO;
			pcb->tiempo_block = *parametro1;

//			sleep(1);
			return true;
			break;
		case WRITE:
			log_info(logger, "CPU-EXECUTE ------------------- OPERACION WRITE PID: %d -------------------", pcb->pid);
			resultado = escribir(*parametro1, parametro2, datos);
//			resultado == -1? printf("Fallo la escritura") : printf("Escritura exitosa");

//			sleep(1);
			return false;
			break;
		case COPY: // COPY(destino, origen)
			log_info(logger, "CPU-EXECUTE ------------------- OPERACION COPY PID: %d -------------------", pcb->pid);
			valor_leido= leer(*parametro1,datos);
			resultado = escribir(*parametro1, parametro2, datos);
//			resultado == -1? printf("Fallo la escritura") : printf("Escritura exitosa");

//			sleep(1);
			return false;
			break;
		case READ:
			log_info(logger, "CPU-EXECUTE ------------------- OPERACION READ PID: %d -------------------", pcb->pid);
			valor_leido = leer(*parametro1,datos);

//			sleep(1);
			return false;
			break;
		case EXIT:
			log_info(logger, "CPU-EXECUTE ------------------- OPERACION EXIT PID: %d -------------------", pcb->pid);
			//	Esto es para hacer mas lenta la ejecucion y poder seguirlo por log
			pcb->estado = FINALIZADO;
//			log_info(logger, "Duermo 5 segundos antes de la siguiente operacion");
//			sleep(1);
			return true;
			break;
		default:
			log_info(logger, "CPU-EXECUTE HUBO UN FALLO EN LA EJECUCION DE LAS INSTRUCCIONES PID: %d", pcb->pid);
			//	Esto es para hacer mas lenta la ejecucion y poder seguirlo por log
//			log_info(logger, "Duermo 5 segundos antes de la siguiente operacion");
//			sleep(2);
			return true;
			break;
	}
}

void ejecutar_ciclo_instrucciones(pcb_t* pcb, bool* devolver_pcb, int retardoNoOp, int cantidad_entradas, int conexion_memoria, int tamano_pagina, bool* hubo_interrupcion)
{
	t_list* instrucciones = pcb->instrucciones;
	uint32_t program_counter = pcb->program_counter;
	cliente_memoria= conexion_memoria;
	Datos_calculo_direccion* datos = malloc(sizeof(Datos_calculo_direccion));
	datos->id_tabla_paginas1 = pcb->pid;
	datos->conexion_memoria = conexion_memoria;
//	datos->entradas_por_tabla = cantidad_entradas;
//	datos->tamano_pagina = tamano_pagina;
	datos->entradas_por_tabla = cant;
	datos->tamano_pagina = tam;

	//	Inicializo los datos para el calculo de direcciones
	datos->numero_pagina = -1;
	datos->entrada_tabla_primer_nivel = -1;
	datos->entrada_tabla_segundo_nivel = -1;
	datos->desplazamiento = -1;

	//fetch
	Instruccion* instruccion = list_get(instrucciones,program_counter);
	program_counter++;
	pcb->program_counter = program_counter;

	log_info(logger, "CPU-EXECUTE Aumento el program counter del PCB PID %d a: %d", pcb->pid, pcb->program_counter);

	//decode
	bool requiere_fetch_operands = false;
	if (instruccion->id == COPY)
		requiere_fetch_operands = true;

	//fetch_operands
//	if (requiere_fetch_operands)
//	{
//		uint32_t* origen_dir = list_get(instruccion->parametros,1);
//		*origen_dir = leer(*origen_dir,datos);
//	}

	//	Ejecuto la instruccion
	*devolver_pcb = execute(instruccion, retardoNoOp, datos, pcb);
//	log_trace(logger, "CPU-EXECUTE Valor de devolver PCBt %s", *devolver_pcb?"true":"false");
	//check interrupt
	pthread_mutex_lock(&mutex_interrupt);
	if (!*devolver_pcb){
		if(*hubo_interrupcion)
		{
			log_info(logger, "CPU-EXECUTE Hubo una interrupcion se cambia el estado del PCB a INTERRUMPIDO");
			pcb->estado = INTERRUMPIDO;
		}
		*devolver_pcb = *hubo_interrupcion;
	}
	pthread_mutex_unlock(&mutex_interrupt);
}

t_config_cpu* crearConfigCPU(void)
{
	t_config_cpu* config = malloc(sizeof(t_config_cpu));
	return config;
}

t_config_cpu* cargarConfiguracion(char* configPath)
{
	t_config* configFile = config_create(configPath);
	t_config_cpu* configTemp = crearConfigCPU();

	logger = setearLogLevel(configFile,"cpu.log", "CPU");

	configTemp->entradasTLB = config_get_int_value(configFile, ENTRADAS_TLB);
		log_trace(logger, "CPU-CONFIGURACION Se leyo la variable ENTRADAS_TLB: %d", configTemp->entradasTLB);
	configTemp->algoritmoReemplazoTLB = config_get_string_value(configFile, ALG_TLB);
		log_trace(logger, "CPU-CONFIGURACION Se leyo la variable REEMPLAZO_TLB: %s", configTemp->algoritmoReemplazoTLB);
	configTemp->retardoNoOp = config_get_int_value(configFile, RETARDO_NOOP);
		log_trace(logger, "CPU-CONFIGURACION Se leyo la variable RETARDO_NOOP: %d", configTemp->retardoNoOp);
	configTemp->IPMemoria = config_get_string_value(configFile, IP_MEMORIA);
		log_trace(logger, "CPU-CONFIGURACION Se leyo la variable IP_MEMORIA: %s", configTemp->IPMemoria);
	configTemp->puertoMemoria = config_get_string_value(configFile, PUERTO_MEMORIA);
		log_trace(logger, "CPU-CONFIGURACION Se leyo la variable PUERTO_MEMORIA: %s", configTemp->puertoMemoria);
	configTemp->puertoDispatch = config_get_string_value(configFile, PUERTO_DISPATCH);
		log_trace(logger, "CPU-CONFIGURACION Se leyo la variable PUERTO_DISPATCH: %s", configTemp->puertoDispatch);
	configTemp->puertoInterrupt = config_get_string_value(configFile, PUERTO_INTERRUPT);
		log_trace(logger, "CPU-CONFIGURACION Se leyo la variable PUERTO_INTERRUPT: %s", configTemp->puertoInterrupt);
	configTemp->IPCPU = config_get_string_value(configFile, IP_CPU);
	if(configTemp->IPCPU == NULL)
	{
		configTemp->IPCPU = malloc(10*sizeof(char));
		strcpy(configTemp->IPCPU,"127.0.0.1");
	}
	log_trace(logger, "CPU-CONFIGURACION Se leyo la variable IP_CPU: %s", configTemp->IPCPU);


	return configTemp;
}
