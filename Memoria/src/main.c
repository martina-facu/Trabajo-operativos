
#include "main.h"
#include "funciones_kernel.h"

void iniciar_config();
void iniciar_servidor_memoria();
void iniciar_conexiones_mem();
void inicializar();

int main(){
	iniciar_config();
	inicializar();
	iniciar_servidor_memoria();
	iniciar_conexiones_mem();

	void* memoria = malloc(TAM_MEMORIA);

	pthread_t kernel;

	pthread_create(&kernel,NULL,funciones_kernel,NULL);
	pthread_join(kernel,NULL);


	return 0;
}

void inicializar(){

	CANT_MARCOS= TAM_MEMORIA/TAM_PAGINA;

	log_trace(logger,"CANTIDAD DE MARCOS: %d", CANT_MARCOS);

	int cant_marcos_bytes = division_entera(CANT_MARCOS,8);

	log_info(logger, "%d", cant_marcos_bytes);

	void* bitarray_memory = malloc(cant_marcos_bytes);

	memset(bitarray_memory,0,cant_marcos_bytes);

	bitMem = bitarray_create_with_mode(bitarray_memory,cant_marcos_bytes,MSB_FIRST);

	for (int i =0; i < cant_marcos_bytes;i++){
			printf("%d", (int)bitarray_test_bit(bitMem,i));
	}
	tabla_1_l = list_create();
	tabla_2_l = list_create();
	procesos = list_create();
}

void iniciar_config(){
	t_config* config = config_create("memoria.config");
	logger = log_create("logger.log","memoria",1,LOG_LEVEL_TRACE);

	PUERTO_ESCUCHA = config_get_string_value(config,"PUERTO_ESCUCHA");
	TAM_MEMORIA = (uint32_t ) config_get_int_value(config,"TAM_MEMORIA");
	log_trace(logger,"TAMANO MEMORIA: %d",TAM_MEMORIA);
	TAM_PAGINA = (uint32_t ) config_get_int_value(config,"TAM_PAGINA");
	log_trace(logger,"TAMANO PAGINA: %d",TAM_PAGINA);
	ENTRADAS_POR_TABLA= (uint32_t ) config_get_int_value(config,"ENTRADAS_POR_TABLA");
	log_trace(logger,"ENTRADAS POR TABLA %d",ENTRADAS_POR_TABLA);
	RETARDO_MEMORIA = config_get_int_value(config,"RETARDO_MEMORIA");
	ALGORITMO_REEMPLAZO = config_get_string_value(config,"ALGORITMO_REEMPLAZO");
	log_trace(logger,"ALGORITMO: %s",ALGORITMO_REEMPLAZO);
	MARCOS_POR_PROCESO= (uint32_t) config_get_int_value(config,"MARCOS_POR_PROCESO");
	log_trace(logger,"MARCOS POR PROCESO: %d",MARCOS_POR_PROCESO);
	RETARDO_SWAP = config_get_int_value(config,"RETARDO_SWAP");
	PATH_SWAP = config_get_string_value(config,"PATH_SWAP");
}
// 1 para cpu
// 41 para kernel
void iniciar_servidor_memoria(){
	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo("127.0.0.1", PUERTO_ESCUCHA, &hints, &servinfo);

	// Creamos el socket de escucha del servidor
	socket_server=socket(servinfo->ai_family,servinfo->ai_socktype,servinfo->ai_protocol);
	//	Marcamos el socket como reutilizable
		if (setsockopt(socket_server, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
		    perror("setsockopt(SO_REUSEADDR) failed");
	// Asociamos el socket a un puerto
	if(bind(socket_server,servinfo->ai_addr,servinfo->ai_addrlen)!=0){
		log_error(logger, "Fallo el bind al generar server");
		return exit(-2);
	}
	// Escuchamos las conexiones entrantes
	if(listen(socket_server,SOMAXCONN)==-1)
	{
		log_error(logger, "Fallo el listen al generar server");
		return exit(-1);
	}
	log_info(logger, "Server escuchando");

	freeaddrinfo(servinfo);
}

void iniciar_conexiones_mem(){
	for(int i=0; i < 2;i++){
		int socket_cliente = accept(socket_server,NULL,NULL);
		if(socket_cliente <0){
			log_error(logger,"error en el accept");
		}
		uint8_t codigo;
		recv(socket_cliente,&codigo,sizeof(uint8_t),0);
		if( codigo == INICIAR_CONEXION_KERNEL){
			socket_kernel = socket_cliente;
			uint8_t handshake = ACEPTAR_CONEXION_KERNEL;
			send(socket_kernel,&handshake,sizeof(uint8_t),0);
			log_trace(logger,"SE ACEPTO EL KERNEL");
		} else if( codigo == INICIAR_CONEXION_CPU){
			socket_cpu = socket_cliente;
			uint8_t handshake = ACEPTAR_CONEXION_CPU;
			send(socket_cpu,&handshake,sizeof(uint8_t),0);
			log_trace(logger,"SE ACEPTO EL CPU");
			uint8_t mensaje;
			recv(socket_cpu,&mensaje,sizeof(uint8_t),0);

			send(socket_cliente,&ENTRADAS_POR_TABLA,sizeof(uint32_t),0);
			send(socket_cliente,&TAM_PAGINA,sizeof(uint32_t),0);

		} else{
			log_error(logger,"ERROR EN EL MENSAJE DE HANDSHAKE");
		}
	}
}


