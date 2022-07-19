#include "swap.h"

//-----------------------------SWAP-----------------------------

void retardo_swap(){

	sleep(RETARDO_SWAP/1000);
}

/*
 *  Funcion: existe_archivo
 *  Entradas: 	char* path	le paso un directorio
 *  Salidas: int	ret me devuelve un entero (1) si el archivo existe
 *  Razon: 	Verificar si el archivo existe para no tener que crearlo.
 */

int existe_archivo(char* path){


	int ret = 0;

	int f = open(path, O_RDONLY);

	if (f != -1){ //si f no es -1, el archivo existe
		close(f);
		ret = 1;

	}

	return ret;
}



void crear_archivo_swap(int pid, int cantidadPaginas){


	char path[1024];
	char nombreArchivo[1024];

	memset(&path, '\0',1024);
	memset(&nombreArchivo, '\0',1024);

	struct stat st = {0};

	retardo_swap();

	if (stat(pSwap, &st) == -1) {
		log_info(logger, "SWAP: Se crea el directorio de swap en %s", pSwap);
	    mkdir(pSwap, 0770);
	}
	else
		log_info(logger, "SWAP: El directorio swap ya existe.");

	sprintf(nombreArchivo, "%d.swap", pid);

	log_info(logger, "SWAP: %s",nombreArchivo);

	sprintf(path, "%s/%s", pSwap, nombreArchivo);

	int tamano_proceso = cantidadPaginas*TAM_PAGINA;

	if(!existe_archivo(path)){

		int archivo = open(path, O_CREAT | O_RDWR,0770);

		log_info(logger, "SWAP: Se crea el archivo con el nombre %s", nombreArchivo);

		truncate(path, tamano_proceso);

		void* archivoMAP = (void*) mmap(NULL, tamano_proceso, PROT_READ | PROT_WRITE, MAP_SHARED, archivo, 0);

		char aux = '1';

		memset(archivoMAP,aux,tamano_proceso);


		munmap(archivoMAP,tamano_proceso);

		close(archivo);
		log_info(logger, "SWAP: Se creo el archivo %s del proceso %d", nombreArchivo, pid);

	}
	else{

		log_info(logger, "SWAP: El archivo ya existia, no se realiza ninguna accion");
	}

}


/*
 *  Funcion: eliminar_archivo_swap
 *  Entradas: 	int pid	le paso un pid (id del proceso)
 *  Salidas: void La devolucion de esta funcion es la eliminacion del archivo con
 *  el pid + .swap
 *  Razon: 	Cuando se finaliza un proceso en memoria, hay que eliminar su archivo en swap.
 *  Para eso valido si existe, si es así lo borro, de lo contrario no hago nada.
 */

void eliminar_archivo_swap(int pidRecibido){

	log_info(logger, "SWAP: Estoy en SWAP");

	char pathArchivo[1024];
	char nombreArchivo[1024];

	retardo_swap();

	sprintf(nombreArchivo, "%d.swap", pidRecibido);
	sprintf(pathArchivo, "%s/%s", pSwap, nombreArchivo);

	if(existe_archivo(pathArchivo)){
		remove(pathArchivo);
		log_info(logger, "SWAP: Se eliminó el archivo %s", nombreArchivo);
	}
	else
		log_info(logger, "SWAP: No se encontro el archivo %s", nombreArchivo);

}

/*
 *  Funcion: crear_archivo_swap
 *  Entradas: 	void No recibe ningun parametro
 *  Salidas: void La devolucion de esta funcion es un entero, que emula un delay
 *  Razon: 	Cuando se hacen operaciones entre memoria y el swap suele haber un tiempo
 *  de espera, con esta función lo emulamos.
 */
//void guardar_frame_en_swap(t_list* lista_a_swappear){
//	log_info(logger, "MEMORIA: Ingresando a SWAP..(SUSPENSION)");
//
//	retardo_swap();
//
//
//	char nombreArchivo[1024];
//	char pathArchivo[1024];
//
//	sprintf(nombreArchivo, "%d.swap", pid);
//	sprintf(pathArchivo, "%s/%s", pSwap, nombreArchivo);
//	log_info(logger, "%s", pathArchivo);
//	int fd = open(pathArchivo, O_RDWR);
//
//	if (fd == -1){
//
//		log_error(logger, "SWAP: El archivo no existe.");
//		exit(1);
//	}
//
//
////	void* archivo = mmap((void*) 0, cantidad_de_paginas_del_proceso(proceso->tamanoProceso,tamanoPagina)*tamanoPagina, PROT_WRITE, MAP_PRIVATE, fd, 0);
////
////	memcpy(&archivo, (memoriaPrincipal + (tamanoPagina*frame)), tamanoPagina);
////
////	msync(archivo, cantidad_de_paginas_del_proceso(proceso->tamanoProceso,tamanoPagina)*tamanoPagina, MS_SYNC);
////
////	munmap(archivo, cantidad_de_paginas_del_proceso(proceso->tamanoProceso,tamanoPagina)*tamanoPagina);
//
//	log_info(logger, "SWAP: Ingresando a memoria..");
//	close(fd);
//
//}
void* swap_(){ //TODO: Agregue el void* como está definido en el .h
	while(1){
		sem_wait(&swap);

		t_swap* pedido = list_remove(pedidos_swap_l,0);

		log_info(logger, "MEMORIA: Ingresando a SWAP..(SUSPENSION)");

		retardo_swap();

		uint32_t pid = pedido->pid;

		char nombreArchivo[1024];
		char pathArchivo[1024];
		struct stat sb;

		sprintf(nombreArchivo, "%d.swap", pid);
		sprintf(pathArchivo, "%s/%s", pSwap, nombreArchivo);

		log_info(logger, "%s", pathArchivo);
		int fd = open(pathArchivo, O_RDWR,0770);
		if(fstat(fd,&sb)==-1){
			log_trace(logger,"ERROR EN ASIGNAR EL ESPACIO DEL ARCHIVO");
		}
		if (fd == -1){

			log_error(logger, "SWAP: El archivo no existe.");
			exit(1);
		}
		log_info(logger, "MEMORIA: VAMO A SWAPPEAR");
		void* archivo = (void*) mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

		while(!list_is_empty(pedido->memorias_a_swappear)){

			t_memory_pag* pagina = list_remove(pedido->memorias_a_swappear,0);

			int numero_pagina = pagina->n_tabla_2*ENTRADAS_POR_TABLA + pagina->n_entrada_2;

			memcpy(archivo+numero_pagina*TAM_PAGINA, (memoria + TAM_PAGINA*pagina->entrada->frame), TAM_PAGINA);

			log_info(logger, "MEMORIA: ARCHIVO COPIADO");
		}

		munmap(archivo, TAM_PAGINA);

		log_info(logger, "SWAP: Ingresando a memoria..");
		close(fd);
	}
}

