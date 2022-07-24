
#include "main.h"
#include "funciones_kernel.h"
#include "funciones_cpu.h"

void iniciar_config();
void iniciar_servidor_memoria();
void iniciar_conexiones_mem();
void inicializar();

int main(){
	iniciar_config();
	inicializar();
	iniciar_servidor_memoria();
	iniciar_conexiones_mem();

	memoria = malloc(TAM_MEMORIA);
	memset(memoria,'\0',TAM_MEMORIA);

	pthread_t kernel;
	pthread_t swap;
	pthread_t cpu;
	
	pthread_create(&swap,NULL,swap_,NULL);
	pthread_create(&kernel,NULL,funciones_kernel,NULL);
	pthread_create(&cpu, NULL, funciones_cpu, NULL);
	
	pthread_join(kernel,NULL);
	pthread_join(cpu, NULL);
	pthread_join(swap,NULL);


	return 0;
}

void inicializar(){

	CANT_MARCOS= TAM_MEMORIA/TAM_PAGINA;

	log_info(logger,"MEMORIA-CONFIGURACION || CANTIDAD DE MARCOS: %d", CANT_MARCOS);

	int cant_marcos_bytes = division_entera(CANT_MARCOS,8);

	void* bitarray_memory = malloc(cant_marcos_bytes);
	bitMem = bitarray_create_with_mode(bitarray_memory,cant_marcos_bytes,MSB_FIRST);

	for(int i=0;i< bitarray_get_max_bit(bitMem);i++){
		bitarray_clean_bit(bitMem, i);
	}

	mostrar_bitarray(bitMem);

	pedidos_swap_l = list_create();
	tabla_1_l = list_create();
	tabla_2_l = list_create();
	procesos = list_create();

	sem_init(&s_swap,0,0);

}

void iniciar_config(){
	t_config* config = config_create("memoria.config");

	logger = setearLogLevel(config,"memoria.log", "MEMORIA");

	PUERTO_ESCUCHA = config_get_string_value(config,"PUERTO_ESCUCHA");
	TAM_MEMORIA = (uint32_t ) config_get_int_value(config,"TAM_MEMORIA");
	log_trace(logger,"MEMORIA-CONFIGURACION: TAMANO MEMORIA: %d",TAM_MEMORIA);
	TAM_PAGINA = (uint32_t ) config_get_int_value(config,"TAM_PAGINA");
	log_trace(logger,"MEMORIA-CONFIGURACION: TAMANO PAGINA: %d",TAM_PAGINA);
	ENTRADAS_POR_TABLA= (uint32_t ) config_get_int_value(config,"ENTRADAS_POR_TABLA");
	log_trace(logger,"MEMORIA-CONFIGURACION: ENTRADAS POR TABLA %d",ENTRADAS_POR_TABLA);
	RETARDO_MEMORIA = config_get_int_value(config,"RETARDO_MEMORIA");
	ALGORITMO_REEMPLAZO = config_get_string_value(config,"ALGORITMO_REEMPLAZO");
	log_trace(logger,"MEMORIA-CONFIGURACION: ALGORITMO: %s",ALGORITMO_REEMPLAZO);
	MARCOS_POR_PROCESO= (uint32_t) config_get_int_value(config,"MARCOS_POR_PROCESO");
	log_trace(logger,"MEMORIA-CONFIGURACION: MARCOS POR PROCESO: %d",MARCOS_POR_PROCESO);
	RETARDO_SWAP = config_get_int_value(config,"RETARDO_SWAP");
	log_trace(logger,"MEMORIA-CONFIGURACION: RETARDO SWAP: %d",RETARDO_SWAP);
	pSwap = config_get_string_value(config,"PATH_SWAP");
	log_trace(logger,"MEMORIA-CONFIGURACION: PATH SWAP: %s",pSwap);
	IPMEMORIA = config_get_string_value(config, "IPMEMORIA");
	if(IPMEMORIA == NULL)
	{
		IPMEMORIA = malloc(10*sizeof(char));
		strcpy(IPMEMORIA,"127.0.0.1");
	}

}
// 1 para cpu
// 41 para kernel
void iniciar_servidor_memoria(){
	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(IPMEMORIA, PUERTO_ESCUCHA, &hints, &servinfo);

	// Creamos el socket de escucha del servidor
	socket_server=socket(servinfo->ai_family,servinfo->ai_socktype,servinfo->ai_protocol);
	//	Marcamos el socket como reutilizable
		if (setsockopt(socket_server, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
			log_error(logger, "MEMORIA-CONEXION || setsockopt(SO_REUSEADDR) failed");
//		    perror("CONEXION: setsockopt(SO_REUSEADDR) failed");
	// Asociamos el socket a un puerto
	if(bind(socket_server,servinfo->ai_addr,servinfo->ai_addrlen)!=0){
		log_error(logger, "MEMORIA-CONEXION || Fallo el bind al generar server");
		return exit(-2);
	}
	// Escuchamos las conexiones entrantes
	if(listen(socket_server,SOMAXCONN)==-1)
	{
		log_error(logger, "MEMORIA-CONEXION || Fallo el listen al generar server");
		return exit(-1);
	}
	log_info(logger, "MEMORIA-CONEXION || Server escuchando");

	freeaddrinfo(servinfo);
}

void iniciar_conexiones_mem(){
	for(int i=0; i < 2;i++){
		int socket_cliente = accept(socket_server,NULL,NULL);
		if(socket_cliente <0){
			log_error(logger,"MEMORIA-CONEXION || Error en el accept");
		}
		uint8_t codigo;
		recv(socket_cliente,&codigo,sizeof(uint8_t),0);
		if( codigo == INICIAR_CONEXION_KERNEL){
			socket_kernel = socket_cliente;
			uint8_t handshake = ACEPTAR_CONEXION_KERNEL;
			send(socket_kernel,&handshake,sizeof(uint8_t),0);
			log_info(logger,"MEMORIA-CONEXION || SE ACEPTO EL KERNEL");
		} else if( codigo == INICIAR_CONEXION_CPU){
			socket_cpu = socket_cliente;
			uint8_t handshake = ACEPTAR_CONEXION_CPU;
			send(socket_cpu,&handshake,sizeof(uint8_t),0);
			log_info(logger,"MEMORIA-CONEXION || SE ACEPTO EL CPU");
			uint8_t mensaje;
			recv(socket_cpu,&mensaje,sizeof(uint8_t),0);

			send(socket_cliente,&ENTRADAS_POR_TABLA,sizeof(uint32_t),0);
			send(socket_cliente,&TAM_PAGINA,sizeof(uint32_t),0);

		} else{
			log_error(logger,"MEMORIA-CONEXION || ERROR EN EL MENSAJE DE HANDSHAKE");
		}
	}
}


