#include "consola.h"




int main(int argc, char *argv[]) {

	//	Se levanta el log de la consola.
	t_log* logger;
	logger = initLogger("consola.log", "CONSOLA", LOG_LEVEL_INFO);

	//	Seteo el primer argumento como la ruta al archivo de instruccion del programa a ejecutar.
	char* filename = argv[1];

	log_info(logger, "El archivo de instrucciones para esta consola es: %s", filename);
	//	Leo el segundo arguento y lo guardo como uint32_t. El mismo posee el tamaño del proceso.
	uint32_t* tamano_proceso = malloc(sizeof(uint32_t));
	*tamano_proceso = atoi(argv[2]);
	log_info(logger, "El tamaño del proceso para esta consola es: %d", *tamano_proceso);

	//	Levamto y leo del archivo de configuracion los valores de conexion al Kernel
	t_config* config = config_create("consola.config");
	char* ip = config_get_string_value(config, "IP_KERNEL");
	log_info(logger, "La IP de conexion al Kernel es: %s", ip);
	char* puerto = config_get_string_value(config, "PUERTO_KERNEL");
	log_info(logger, "El puerto de conexion al Kernel es: %s", puerto);

	//	Abro el archivo donde estan las instrucciones del programa
	FILE* input_file = fopen(filename, "r");

	if (input_file == NULL)
	{
		perror("error al leer el archivo");
		log_error(logger, "Error al leer el archivo de instrucciones del programa");
		exit(EXIT_FAILURE);
	}

	//	Proceso el archivo del programa y obtengo una lista de instrucciones
	t_list* instrucciones = obtener_intrucciones(input_file);

	//	Muestro las instrucciones obtenidas tanto en pantalla como en log.
	mostrar_instrucciones(instrucciones, logger);

	// ---------------------------------------------------------------------------- SERIALIZACION ----------------------------------------------------------------------------------------//
	//	Serializo el mensaje a enviar al Kernel
	uint32_t* tamano_mensaje = malloc(sizeof(uint32_t));
	void* a_enviar = serializar_mensaje(instrucciones,tamano_proceso,tamano_mensaje);


	// ---------------------------------------------------------------------------- CONEXION ----------------------------------------------------------------------------------------//
	//	Levanto conexion con el Kernel
	int conexion = levantarConexionKernel(ip, puerto, logger);

	//	Envio mensaje al Kernel con el PCB
	send(conexion, a_enviar,*tamano_mensaje, 0);
	log_info(logger, "Se envio el mensaje %s al KERNEL de tamaño %d", (char*)a_enviar,*tamano_mensaje);

	//	Espero respuesta del Kernel que se termino de procesar
	uint8_t respuesta = 0;
	recv(conexion, &respuesta, sizeof(uint8_t), 0);
	log_info(logger, "Se recibio la respuesta %d del KERNEL", respuesta);

	if (respuesta == PROCESO_FINALIZADO)
		log_info(logger, "Se recibio la respuesta de finalizacion exitosa del KERNEL: %d", respuesta);
	else
		log_info(logger, "No pudo finalizarse en forma exitosa el proceso");

	close(conexion);
	return EXIT_SUCCESS;
}

/*
 *  Funcion: levantarConexionKernel
 *  Entradas: 	char* ip
 *  			char* puerto
 *  			t_log* logger
 *  Salidas: int	Descriptor de conexion en el que esta atendiendo el Kernel
 *  Razon: 	Genero la conexion con el Kernel
 *  Autor:
 */
int levantarConexionKernel(char* ip, char* puerto, t_log* logger)
{
	//	Me conecto al kernel con los valores recibidos por archivo de configuracion
	int conexion = crear_conexion(ip, puerto, logger);
	if (conexion < 0)
	{
		log_info(logger, "No pude conectarme al Kernel");
		exit(EXIT_FAILURE);
	}
	else
	{
		log_info(logger, "Se establece una conexion voy a comenzar con el envio de Handshake para validar la misma");

		//	Seteo el valor del Handshake para el inicio de Consola hacia Kernel
		uint8_t handshake = INICIAR_CONEXION_CONSOLA;
		//	Envio Handshake
		send(conexion, &handshake, sizeof(uint8_t), 0);
		log_info(logger, "Se envia Handshake al Kernel");

		//	Recibo la respuesta del Kernel
		uint8_t respuesta_kernel = 0;
		recv(conexion, &respuesta_kernel, sizeof(uint8_t), 0);
		log_info(logger, "Mensaje recibido del Kernel:  %d", respuesta_kernel);

		if(respuesta_kernel == ACEPTAR_CONEXION_CONSOLA)
		{
			log_info(logger, "Conexion establecida con el Kernel");
			return conexion;
		}
		else
		{
			log_error(logger, "No se puede establecer conexion con el Kernel. El server contactado responde con un mensaje no predeterminado: %d.", respuesta_kernel);
			exit(EXIT_FAILURE);
		}

	}
	log_info(logger, "Me conecte al KERNEL en el descriptor: %d", conexion);


}


/*
 *  Funcion: serializar_mensaje
 *  Entradas: 	t_list* instrucciones		Lista con todas las instrucciones del programa que
 *  										va a enviar la consola a procesar al Kernel
 *  			uint32_t* tamano_proceso	Tamaño del proceso recibido por argumento
 *  			uint32_t* tamano_mensaje	Tamaño del mensaje serializado
 *  Salidas: void*	Puntero al mensaje serializado.
 *  Razon: 	Se recibe la lista de intrucciones y se serializa la misma para su envio al Kernel.
 *  Autor:
 */

void* serializar_mensaje(t_list* instrucciones, uint32_t* tamano_proceso,uint32_t* tamano_mensaje)
{
	void* stream_instrucciones = armar_stream_instruccion(instrucciones);
	int tamano_instrucciones = calcular_espacio_instrucciones(instrucciones);

	t_buffer* buffer = armar_buffer(tamano_instrucciones, stream_instrucciones);
	t_paquete* paquete = empaquetar_buffer(buffer,0);

	void* a_enviar = malloc(paquete->size + sizeof(uint32_t));
	a_enviar = serializar_paquete(paquete, a_enviar);

	memcpy(a_enviar + paquete->size, tamano_proceso, sizeof(uint32_t));

	*tamano_mensaje = paquete->size + sizeof(uint32_t);

	return a_enviar;
}

/*
 *  Funcion: obtener_intrucciones
 *  Entradas: 	FILE* input_file		Puntero al archivo a procesar
 *  Salidas: t_list*	Puntero a la lista de instrucciones
 *  Razon: 	Recibo el puntero al archivo del cual se procesara fila por fila para conseguir
 *  		el listado de instrucciones.
 *  		Con cada nueva linea leida la transformo en formato de instruccion y las ingreso
 *  		en una lista de instruccion ordenada por numero de instruccion.
 *  Autor:
 */

t_list* obtener_intrucciones(FILE* input_file)
{
	char *contents = NULL;
	size_t len = 0;

	t_list* instrucciones = list_create();
	uint32_t* parametro;
	uint32_t* cant = malloc(sizeof(uint32_t));

	while (getline(&contents, &len, input_file) != -1) {
		char** linea = string_split(contents, " ");
		char* nombre = linea[0];
		uint8_t id = definirCodigo(nombre);

		Instruccion* instruccion = malloc(sizeof(Instruccion));
		instruccion->id = id;
		instruccion->parametros = list_create();

		if(id == 1){
			*cant = atoi(linea[1]);

			for (int i = 1; i<=*cant; i++) {
				list_add(instrucciones, instruccion);
			}
		}else{
			for (int i = 1; linea[i] != NULL; i++) {
				parametro = malloc(sizeof(uint32_t));
				*parametro = atoi(linea[i]);
				list_add(instruccion->parametros, parametro);
			}
			list_add(instrucciones, instruccion);
		}
	}

	fclose(input_file);
	free(contents);
	free(cant);

	return instrucciones;
}
