#include "swap.h"

//-----------------------------SWAP-----------------------------

void retardo_swap(){

	sleep(swapDelay/1000);
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

	if(!existe_archivo(path)){

		int archivo = open(path, O_CREAT | O_RDWR , 0770);

		log_info(logger, "SWAP: Se crea el archivo con el nombre %s", nombreArchivo);

		truncate(path, cantidadPaginas);

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

void guardar_archivo_en_swap(int pid, int tamanoProceso,int nroFrame,int cantPag,int tamanoPagina, char* memoriaP){

	log_info(logger, "MEMORIA: Ingresando a SWAP..(SUSPENSION)");

	retardo_swap();


	char nombreArchivo[1024];
	char pathArchivo[1024];

	sprintf(nombreArchivo, "%d.swap", pid);
	sprintf(pathArchivo, "%s/%s", pSwap, nombreArchivo);

	log_info(logger, "%s", pathArchivo);
	int fd = open(pathArchivo, O_RDWR);

	if (fd == -1){

		log_error(logger, "SWAP: El archivo no existe.");
		exit(1);
	}

	if (tamanoProceso == -1){
		log_error(logger, "SWAP: Ocurrió un error al escribir en swap.");
		exit(1);
	}


	void* archivo = mmap((void*) 0, cantPag, PROT_WRITE, MAP_PRIVATE, fd, 0);

	close(fd);

	memcpy(&archivo, (memoriaP + (tamanoPagina*nroFrame)), cantPag);

	msync(archivo, cantPag, MS_SYNC);

	munmap(archivo, cantPag);

	log_info(logger, "SWAP: Ingresando a memoria..");
}
