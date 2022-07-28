
#include "funciones_kernel.h"

/*
 *  Funcion: 	funciones_kernel
 *  Entradas: 	void
 *  Salidas: 	void* (es la funcion de un hilo)
 *  Razon:		Evaluar los mensajes recibidos desde el Kernel y enviar a procesar los mismos.
 *  Author: Grupo 99
 */

void* funciones_kernel(){
	while(1){
		log_info(logger,"MEMORIA-KERNEL || TE ESTAMOS ESPERANDO KERNELITO");
		uint8_t operacion;
		recv(socket_kernel,&operacion,sizeof(uint8_t),0);
		log_trace(logger,"MEMORIA-KERNEL || RECIBI UN CODIGO DE OPERACION: %d", (int) operacion);
		switch(operacion){
		case INICIALIZAR_PROCESO:
			inicializar_proceso();
			break;
		case SUSPENDER_PROCESO:
			suspender_proceso();
			break;
		case FINALIZAR_PROCESO:
			finalizar_proceso();
			break;
		default:
			log_error(logger, "MEMORIA-KERNEL || El mensaje recibido %d no corresponde a uno de los conocidos.", operacion);
			log_error(logger, "MEMORIA-KERNEL || Se procede a cerrar la conexion");
			close(socket_kernel);
			break;
		}
	}
	return NULL;
}

/*
 *  Funcion: 	inicializar_proceso
 *  Entradas: 	void
 *  Salidas: 	void
 *  Razon:		Genero las estructuras para el ingreso de un nuevo proceso a la memoria.
 *  Author: Grupo 99
 */


void inicializar_proceso(){
	uint32_t pid;
	uint32_t tam_proceso;
	recv(socket_kernel,&pid,sizeof(uint32_t),0);
	log_trace(logger,"MEMORIA-KERNEL || PID RECIBIDO: %d",pid);
	recv(socket_kernel,&tam_proceso,sizeof(uint32_t),0);
	log_trace(logger,"MEMORIA-KERNEL || TAM PROCESO RECIBIDO: %d",tam_proceso);
	t_proceso *proceso = crear_proceso(pid,tam_proceso);
	list_add(procesos,proceso);
	uint32_t cant_pag = division_entera(tam_proceso,TAM_PAGINA);
	crear_archivo_swap(pid,cant_pag);
	log_info(logger,"MEMORIA-KERNEL || Proceso cargado en Memoria, se envia al Kernel el numero de tabla de primer nivel asignado: %d",proceso->entrada_1);
	send(socket_kernel,&proceso->entrada_1,sizeof(uint32_t),0);

	mostrar_tablas(proceso);
	mostrar_bitarray();

}

/*
 *  Funcion: 	paginas_modificadas
 *  Entradas: 	void* entrada_
 *  Salidas: 	void
 *  Razon:
 *  Author: Grupo 99
 */

bool paginas_modificadas(void* entrada_){
	t_memory_pag* pagina= entrada_;
	return pagina->entrada->bMod ==1;
}

/*
 *  Funcion: 	mostrar_paginas
 *  Entradas: 	t_list* paginas		Lista de paginas a mostrar por logs
 *  Salidas: 	void
 *  Razon:		Guarda en los logs las paginas de un proceso
 *  Author: Grupo 99
 */

void mostrar_paginas(t_list* paginas){
	for(int i =0; i<list_size(paginas);i++){
		t_memory_pag* pagina = list_get(paginas,i);
		int num_pagina = pagina->n_tabla_2*ENTRADAS_POR_TABLA + pagina->n_entrada_2;
		log_info(logger,"MEMORIA-KERNEL || PAGINA: %d, FRAME: %d ",num_pagina,pagina->entrada->frame);
	}
}

/*
 *  Funcion: 	suspender_proceso
 *  Entradas: 	void
 *  Salidas: 	void
 *  Razon:		Suspender el proceso de acuerdo a lo solicitado por el Kernel
 *  Author: Grupo 99
 */

void suspender_proceso(){
	uint32_t pid;
	recv(socket_kernel,&pid,sizeof(uint32_t),0);
	log_trace(logger,"MEMORIA-SWAP || Se solicita suspender el proceso con PID: %d",pid);
	t_proceso* proceso = list_get(procesos,pid);
	t_list* paginas_modificadas_proceso =list_filter(proceso->pagMem,paginas_modificadas);
	t_swap* paginas_a_swappear = malloc(sizeof(t_swap));
	paginas_a_swappear->pid = pid;
	paginas_a_swappear->memorias_a_swappear=  paginas_modificadas_proceso;
	list_add(pedidos_swap_l,paginas_a_swappear);
	log_trace(logger,"MEMORIA-SWAP || Se da aviso al Thread de SWAP para que envie proceso a SWAP");
	sem_post(&s_swap);
	usleep(RETARDO_SWAP);
	while(!list_is_empty(proceso->pagMem)){
		t_memory_pag* pagina = list_remove(proceso->pagMem,0);
		pagina->entrada->bPres=0;
		bitarray_clean_bit(bitMem, pagina->entrada->frame);
	}
	mostrar_bitarray();
}

/*
 *  Funcion: 	finalizar_proceso
 *  Entradas: 	void
 *  Salidas: 	void
 *  Razon:		Finalizar el proceso de acuerdo a lo solicitado por el Kernel.
 *  			Libero todas las estructuras ocupadas
 *  Author: Grupo 99
 */

void finalizar_proceso(){

	uint32_t pid;
	recv(socket_kernel,&pid,sizeof(uint32_t),0);
	log_trace(logger, "MEMORIA-KERNEL || Se recibe un pid %d para finalizar", pid);
	t_proceso* proceso = list_get(procesos,pid);
	log_trace(logger,"MEMORIA-KERNEL || AGARRAMOS EL PROCESO DE LA LISTA DE PROCESOS, PID: %d",proceso->pid);
	for(int i = 0; i < list_size(proceso->pagMem); i++){
		t_memory_pag* pagina = list_get(proceso->pagMem,i);
		log_trace(logger,"MEMORIA-KERNEL || VAMOS A Liberar UNA PAGINA DEL FRAME: %d",pagina->entrada->frame);
		memset(memoria + pagina->entrada->frame*TAM_PAGINA, '\0', TAM_PAGINA);
		bitarray_clean_bit(bitMem, pagina->entrada->frame);
		log_trace(logger, "MEMORIA-KERNEL || MUESTRO EL BITARRAY, LUEGO DE LIMPIAR ESE FRAME");
		mostrar_bitarray();
	}
	eliminar_archivo_swap(pid);
	log_trace(logger, "MEMORIA-KERNEL || Elimino el archivo de SWAP del proceso con PID: %d",pid);

//TODO: Esto se me ocurrio para liberar memoria

//	log_trace(logger, "Eliminamos la lista de paginas presentes");
//	list_destroy(paginas_presentes_proceso);
//	log_trace(logger, "Eliminamos la lista de paginas en memoria y las liberamos");
//	list_destroy_and_destroy_elements(proceso->pagMem, free);
//	log_trace(logger, "Liberamos el proceso de la lista de procesos");
//	list_remove_and_destroy_element(procesos, proceso->pid, free);

}

/*
 *  Funcion: 	crear_proceso
 *  Entradas: 	uint32_t pid			PID del proceso a crear
 *  			uint32_t tam_proceso	Tamaño del proceso a crear
 *  Salidas: 	t_proceso*		Estructura con los datos de un proceso necesarios desde la memoria
 *  Razon:		Crear un proceso de acuerdo a lo solicitado por el Kernel
 *  Author: Grupo 99
 */

t_proceso* crear_proceso(uint32_t pid, uint32_t tam_proceso){
	log_info(logger,"MEMORIA-KERNEL || CREANDO PROCESO");
	uint32_t cant_pag = division_entera(tam_proceso,TAM_PAGINA);
	log_info(logger,"MEMORIA-KERNEL || CANTIDAD PAGINAS: %d", cant_pag);
	uint32_t cant_entradas_1 = division_entera(cant_pag,ENTRADAS_POR_TABLA);
	log_info(logger,"MEMORIA-KERNEL || CANTIDAD ENTRADAS: %d", cant_entradas_1);

	if(cant_entradas_1>ENTRADAS_POR_TABLA){
		log_error(logger,"MEMORIA-KERNEL || NO SE DEBERIA PODER, QUE HACEMOS?");
	}

	t_tabla_1* tabla = malloc(sizeof(t_tabla_1));
	inicializar_tabla_1(tabla,pid);
	asignar_tabla_2(tabla,cant_entradas_1);
	list_add(tabla_1_l,tabla);

	uint32_t entrada = list_size(tabla_1_l)-1;
	log_info(logger,"MEMORIA-KERNEL || ENTRADA: %d || PID: %d",entrada,pid);

	t_proceso * proceso= malloc(sizeof(t_proceso));

	proceso->pid= pid;
	proceso->entrada_1= entrada;
	proceso->puntero=0;
	proceso->tam_proceso= tam_proceso;
	proceso->pagMem= list_create();
	proceso->contador=0;

	return proceso;
}

/*
 *  Funcion: 	inicializar_tabla_1
 *  Entradas: 	t_tabla_1* tabla	Puntero a la tabla de primer nivel a inicializar
 *  			uint32_t pid		PID del proceso que se le inicializara la tabla de primer nivel
 *  Salidas: 	void
 *  Razon:		Inicializar tabla de primer nivel al generar un proceso nuevo.
 *  Author: Grupo 99
 */

void inicializar_tabla_1(t_tabla_1* tabla,uint32_t pid){
	tabla->pid = pid;
	tabla->entradas = malloc(ENTRADAS_POR_TABLA* sizeof(int));
	for(int i=0; i<ENTRADAS_POR_TABLA;i++){
		*(tabla->entradas + i)= -1;
	}
	log_trace(logger,"MEMORIA-KERNEL || SE IINICIALIZO LA TABLA PRIMER NIVEL || PID: %d", pid);
}

/*
 *  Funcion: 	asignar_tabla_2
 *  Entradas: 	t_tabla_1* tabla			Puntero a la tabla de primer nivel a inicializar
 *  			uint32_t cant_entradas_1
 *  Salidas: 	void
 *  Razon:
 *  Author: Grupo 99
 */


void asignar_tabla_2(t_tabla_1* tabla,uint32_t cant_entradas_1){
	for(int i=0;i<cant_entradas_1;i++){
		t_tabla_2* tabla2= malloc(sizeof(t_tabla_2));
		tabla2->entradas=list_create();

		inicializar_tabla_2(tabla2->entradas);
		list_add(tabla_2_l,tabla2);

		*(tabla->entradas+i)=list_size(tabla_2_l)-1;

	}
	log_trace(logger,"MEMORIA-KERNEL || Se asigno la tabla 2 con %d entradas", cant_entradas_1);
}

/*
 *  Funcion: 	inicializar_tabla_2
 *  Entradas: 	t_list* entradas_tabla2	Puntero a la tabla de segundo nivel a inicializar
 *  Salidas: 	void
 *  Razon:		Inicializar tabla de segundo nivel al generar un proceso nuevo.
 *  Author: Grupo 99
 */

void inicializar_tabla_2(t_list* entradas_tabla2){
	for(int i=0;i<ENTRADAS_POR_TABLA;i++){
		t_entrada_2* entrada = malloc(sizeof(t_entrada_2));
		entrada->frame=-1;
		entrada->bUso=0;
		entrada->bMod=0;
		entrada->bPres=0;
		list_add(entradas_tabla2,entrada);

	}
	log_trace(logger,"MEMORIA-KERNEL || SE IINICIALIZO LA TABLA DE SEGUNDO NIVEL");
}

/*
 *  Funcion: 	division_entera
 *  Entradas: 	double numerador	Numerador de la division
 *  Salidas: 	double denominador	Denominador de la division
 *  Razon:		Obtener la parte entera redondeada hacia arriba si la parte decimal de la misma
 *  			es distinta de cero.
 *  Author: Grupo 99
 */

int division_entera(double numerador,  double denominador){

	double resultado,fractPart,intpart;
	resultado = (double) numerador / denominador;
	fractPart = modf(resultado, &intpart);
	if (fractPart > 0){
		return (int)intpart + 1;
	}
	else
		return (int)intpart;
}

/*
 *  Funcion: 	mostrar_bitarray
 *  Entradas: 	void
 *  Salidas: 	void
 *  Razon:		Mostrar el estado del bitarray
 *  Author: Grupo 99
 */

void mostrar_bitarray()
{
	char barray[1024];
	char buffer[1024];

	memset(barray, '\0', sizeof(char)*1024);
	memset(buffer, '\0', sizeof(char) * 1024);

	for(int i=0;i< bitarray_get_max_bit(bitMem);i++){

		sprintf(buffer, "%d", (int)bitarray_test_bit(bitMem,i));
		strcat(barray, buffer);
		}

	log_info(logger, "%s", barray);
}

/*
 *  Funcion: 	mostrar_tablas
 *  Entradas: 	t_proceso* proceso	Estructura del proceso en memoria
 *  Salidas: 	void
 *  Razon:		Mostrar las tablas del proceso solicitado.
 *  Author: Grupo 99
 */

void mostrar_tablas(t_proceso* proceso_){
	t_tabla_1* tabla1 = list_get(tabla_1_l,proceso_->entrada_1);
	log_info(logger,"MEMORIA-ALGORITMO || ------------------ MOSTRANDO TABLA 1 Proceso: %d ------------------", proceso_->pid);
	log_info(logger,"MEMORIA-ALGORITMO || ENTRADA DE TABLA 1 GLOBAL: %d", proceso_->entrada_1);
	for(int i=0; i < ENTRADAS_POR_TABLA && *(tabla1->entradas+i)!=-1 ;i++){
		uint32_t indice = *(tabla1->entradas+i);
		log_info(logger,"MEMORIA-ALGORITMO || --------- MOSTRANDO TABLA 2 Proceso: %d --------- ", proceso_->pid);
		log_info(logger,"MEMORIA-ALGORITMO || INDICE DE TABLA 2 GLOBAL: %d ", indice);
		t_tabla_2* tabla2= list_get(tabla_2_l,indice);
		for(int j =0; j<ENTRADAS_POR_TABLA;j++){
			t_entrada_2* entrada2 = list_get(tabla2->entradas,j);
			log_info(logger,"MEMORIA-ALGORITMO ||\tENTRADA\t||U: %d\t||M: %d\t||P: %d\t||FRAME: %d", entrada2->bUso,entrada2->bMod,entrada2->bPres,entrada2->frame);
		}
	}
}
