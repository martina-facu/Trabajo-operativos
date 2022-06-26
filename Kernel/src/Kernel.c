
#include "kernel.h"



int main(void)
{
	//	Levanto todo el archivo de configuracion
	establecer_configuracion();
	//	Inicializo todos los logs
	inicializar_loggers();

	//	Se establece la conexion con la Memoria
	levantar_conexion_memoria(configuracion->IP_MEMORIA,configuracion->PUERTO_MEMORIA, logP);

	//	Se establece conexion con el Dispatch de la CPU
//	levantar_conexion_cpu_dispatch(configuracion->IP_CPU, configuracion->PUERTO_CPU_DISPATCH , logP);

	//	Se establece conexion con el Interrupt de la CPU
//	levantar_conexion_cpu_interrupt(configuracion->IP_CPU, configuracion->PUERTO_CPU_INTERRUPT , log);

	//	Me levanto como server y establesco todas las conexiones como cliente
	//	Inicio el servidor Kernel para atencion de consolas
	server_fd = iniciar_servidor(configuracion->IP_KERNEL,configuracion->PUERTO_ESCUCHA, PLP);


//	iniciar_conexion_cpu_interrupt();
//	iniciar_conexion_memoria();

	//	Inicializo las listas de los planificadores
	inicializar_listas();
	//	Inicializo los semaros usados por los planificadores
	inicializar_semaforos();

	//	Defino las variables de identificacion de los threads de cada planificador
//	pthread_t planificador_corto_plazo;
//	pthread_t planificador_mediano_plazo;
	pthread_t planificador_largo_plazo;


	if(strcmp(configuracion->algoritmo,"FIFO")==0)
	{
		log_trace(logP,"MODO PLANIFICADOR CORTO PLAZO: FIFO\n");
//		pthread_create(&planificador_corto_plazo,NULL,fifo,NULL);
	}
	else if(strcmp(configuracion->algoritmo,"SJF\n")==0)
	{
		log_trace(logP,"MODO PLANIFICADOR CORTO PLAZO: SJF");
	}
	else{
		printf("error en el algoritmo, finalizando kernel\n");
		return -1;
	}
	log_trace(logP,"EJECUTANDO: PLANIFICADOR LARGO PLAZO\n");
	pthread_create(&planificador_largo_plazo,NULL,administrador_largo_plazo,NULL);
	log_trace(logP,"EJECUTANDO: PLANIFICADOR MEDIANO PLAZO");
//	pthread_create(&planificador_mediano_plazo,NULL,administrador_mediano_plazo,NULL);


	pthread_join(planificador_largo_plazo,NULL);
//	pthread_join(planificador_corto_plazo,NULL);
//	pthread_join(planificador_mediano_plazo,NULL);

	return EXIT_SUCCESS;
}
void establecer_configuracion()
{
	t_config* kernel_config= config_create("kernel.config");

	if(kernel_config==NULL)
	{
		perror("\nNo pude abrir el archivo de configuracion\n");
		exit(EXIT_FAILURE);
	}

	configuracion = malloc(sizeof(t_config_kernel));

	configuracion->grado_multiprogramacion =	config_get_int_value(kernel_config, "GRADO_MULTIPROGRAMACION");
	configuracion->TIEMPO_BLOCK_MAX		=	config_get_double_value(kernel_config, "TIEMPO_MAXIMO_BLOQUEADO");
	configuracion->algoritmo 				= 	config_get_string_value(kernel_config,"ALGORITMO_PLANIFICACION");
	configuracion->estimacion_inicial		= 	config_get_double_value(kernel_config,"ESTIMACION_INICIAL");
	configuracion->alpha 					=	config_get_double_value(kernel_config,"ALFA");


	configuracion->IP_CPU					=	config_get_string_value(kernel_config,"IP_CPU");
	configuracion->IP_MEMORIA				= 	config_get_string_value(kernel_config,"IP_MEMORIA");
	configuracion->PUERTO_ESCUCHA			= 	config_get_string_value(kernel_config,"PUERTO_ESCUCHA");
	configuracion->PUERTO_MEMORIA			= 	config_get_string_value(kernel_config,"PUERTO_MEMORIA");
	configuracion->PUERTO_CPU_DISPATCH		= 	config_get_string_value(kernel_config,"PUERTO_CPU_DISPATCH");
	configuracion->PUERTO_CPU_INTERRUPT	=	config_get_string_value(kernel_config,"PUERTO_CPU_INTERRUPT");
	configuracion->IP_KERNEL	=	config_get_string_value(kernel_config,"IP_KERNEL");
}



void inicializar_listas(){
	comunicaciones_l= 		list_create();
	ready_l= 				list_create();
	new_l= 					list_create();
	susp_ready_l=			list_create();
	block_l=				list_create();
	unblock_l= 				list_create();
	susp_block_l= 			list_create();
	exit_l=					list_create();
	interrumpidos_l=		list_create();
	finalizado_l =			list_create();
	susp_block_buffer_l=    list_create();
	newM_l=					list_create();
	susp_readyM_l=			list_create();
}

void inicializar_semaforos(){
	sem_init(&s_proceso_ejecutando,0,1);
	sem_init(&s_proceso_ready,0,0);
	sem_init(&s_cpu,0,0);
	sem_init(&s_proceso_finalizado,0,0);
	sem_init(&s_proceso_new,0,0);
	sem_init(&s_grado_multiprogramacion,0,grado_multiprogramacion);// contador
	sem_init(&s_proceso_susp,0,0);
	sem_init(&s_interrupcion,0,0);
	sem_init(&s_susp,0,0);
}


void inicializar_loggers(){
	logP = log_create("kernel.log","KERNEL",0,0);
	PCP = log_create("planificador_corto_plazo.log","planificador_corto_plazo",0,0);
	PMP = log_create("planificador_mediano_plazo.log","planificador_mediano_plazo",0,0);
	PLP = log_create("planificador_largo_plazo.log","planificador_largo_plazo",0,0);
}
