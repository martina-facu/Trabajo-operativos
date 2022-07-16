#ifndef CPU
#define CPU

#include <stdio.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <conexion.h>
#include <pcb.h>
#include <paquete.h>
#include <logs.h>
#include <pthread.h>
#include <errno.h>
#include <paquete.h>
#include <mmu.h>

#define SERVER_DISPATCH "SERVER DISPATCH"
#define SERVER_INTERRUPT "SERVER INTERRUPT"
#define CPU_IP "IP_CPU"

#define THREAD_STACK_SIZE 65536 //64KB
#define CONCURRENT_CONNECTION 1 //conexiones concurrentes maximas

/*
 * Listado de estructuras
*/

#define ENTRADAS_TLB "ENTRADAS_TLB"
#define ALG_TLB "REEMPLAZO_TLB"
#define RETARDO_NOOP "RETARDO_NOOP"
#define IP_MEMORIA "IP_MEMORIA"
#define PUERTO_MEMORIA "PUERTO_MEMORIA"
#define PUERTO_DISPATCH "PUERTO_ESCUCHA_DISPATCH"
#define PUERTO_INTERRUPT "PUERTO_ESCUCHA_INTERRUPT"
#define IP_CPU "IP_CPU"

	typedef struct
	{

		int entradasTLB;
		char* algoritmoReemplazoTLB;
		int retardoNoOp;
		char* IPMemoria;
		char* IPCPU;
		char* puertoMemoria;
		char* puertoDispatch;
		char* puertoInterrupt;
	}t_config_cpu;


/*
 * Listado de variables globales
*/
	t_log* logger;
	int fdDispatch;
	// thread identifier
	pthread_t thread_id;
	struct timeval 	tiempoSelect;
	fd_set read_fd_set;			//	conjunto temporal de fd para select
	pcb_t* pcb;
	fd_set master_fd_set; 		// 	conjunto maestro de descriptores de fichero
	bool devolver_pcb;			//	Si es True significa que recibi una interrupcion
	bool recibiPCB;				//	Si es FALSE significa que todavia no recibi ningun PCB si es TRUE ya recibi PCB a procesar desde el Kernel
	//	bool* hubo_interrupcion;
	t_list* tlb;

	uint32_t Ntamano_pagina;
	uint32_t Ncantidad_entradas;
	uint32_t Nid_tabla_paginas1;
	uint32_t Nentrada_tabla_primer_nivel;
	uint32_t Nentrada_tabla_segundo_nivel;
	uint32_t Nmarco; //TODO: lo declaro global pq me pudri

/*
 * Prototipo de funciones
*/

	int levantar_conexion_memoria(char* ipServer, char* portServer, uint32_t* cantidad_entradas,uint32_t* tamano_pagina);
	int levantar_server(char* ipServer, char* portServer, char* sTipo);
	t_paquete* recibir_mensaje_memoria(int conexion_memoria);
	pcb_t* obtener_pcb(int cliente);

	void ejecutar_ciclo_instrucciones(pcb_t* pcb, bool* devolver_pcb, int retardoNoOp, int cantidad_entradas, int conexion_memoria, int tamano_pagina, bool* hubo_interrupcion);
	bool execute(Instruccion* instruccion,int dormir, Datos_calculo_direccion* datos, pcb_t* pcb);
	uint32_t leer(uint32_t direccion_logica, Datos_calculo_direccion* datos,t_log* logger);
	uint32_t* escribir(int direccion_logica, uint32_t* valor_a_escribir, Datos_calculo_direccion* datos);
	bool validar_codigo(t_paquete* paquete, uint8_t operacion);
	void mandar_lecto_escritura(uint32_t direccion, uint32_t* valor, uint8_t operacion, int conexion);


#endif
