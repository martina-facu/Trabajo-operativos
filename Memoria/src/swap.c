#include "swap.h"

//-----------------------------SWAP-----------------------------

void retardo_swap(){

	sleep(config->swap_time_delay/1000);
}

char* obtener_nombre_archivo(int pid){

	char archivo[1024];
	memset(&archivo, '\0',1024);

	sprintf(archivo, "%d.swap", pid);

	return  archivo;
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



void crear_archivo_swap(int pid, int tamanioProceso){


	char* pathSwap = config->path_swap;
	char path[1024];
	char nombreArchivo[1024];

	memset(&path, '\0',1024);
	memset(&nombreArchivo, '\0',1024);
	struct stat st = {0};

	retardo_swap();

	if (stat(pathSwap, &st) == -1) {
		log_info(logger, "SWAP: Se crea el archivo de swap en %s", pathSwap);
	    mkdir(pathSwap, 0700);
	}
	else
		log_info(logger, "SWAP: El directorio swap ya existe.");

	//nombreArchivo = obtener_nombre_archivo(pid);
	strcpy(nombreArchivo, obtener_nombre_archivo(pid));


	log_info(logger, "SWAP: %s",nombreArchivo);

	sprintf(path, "%s/%s", pathSwap, nombreArchivo);

	if(!existe_archivo(path)){

		int archivo = open(path, O_CREAT | O_RDWR , 0777);

		log_info(logger, "SWAP: Se crea el archivo con el nombre %s", nombreArchivo);

		truncate(path, tamanioProceso);

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

	t_proceso* proceso = malloc(sizeof(t_proceso));
	int tamanoProceso;
	//int cantidadDeProcesos = list_size(procesos);
	char* pathArchivo = malloc(sizeof(char));
	char* nombreArchivo = malloc(sizeof(char));
	char* pathSwap = config->path_swap;
	int b = 0, i = 0;

	retardo_swap();

	while(b == 0){


		proceso = list_get(procesos, i);
		i++;
		if (proceso->pid == pidRecibido){

			tamanoProceso = proceso->tamanoProceso;

			nombreArchivo = obtener_nombre_archivo(pidRecibido);

			sprintf(pathArchivo, "%s/%s", pathSwap, nombreArchivo);

			if(existe_archivo(pathArchivo)){
				if(cerrar_archivo(nombreArchivo, tamanoProceso)){
						log_info(logger, "SWAP: Se eliminó el archivo %s", nombreArchivo);
						b = 1;
			}
				else
						log_info(logger, "SWAP: Ocurrio un error al eliminar el archivo %s", nombreArchivo);
				}

				else
					log_info(logger, "SWAP: No se encontro el archivo %s", nombreArchivo);
			}

		}
}


/*
 *  Funcion: crear_archivo_swap
 *  Entradas: 	void No recibe ningun parametro
 *  Salidas: void La devolucion de esta funcion es un entero, que emula un delay
 *  Razon: 	Cuando se hacen operaciones entre memoria y el swap suele haber un tiempo
 *  de espera, con esta función lo emulamos.
 */

void guardar_archivo_en_swap(int pid, int tamanoProceso){

	retardo_swap();

	char* nombreArchivo = obtener_nombre_archivo(pid);

	int fd = open(nombreArchivo, O_RDWR);

	if (fd == -1){

		log_error(logger, "SWAP: El archivo no existe.");
		exit(1);
	}

	if (tamanoProceso == -1){
		log_error(logger, "SWAP: Ocurrió un error al escribir en swap.");
		exit(1);
	}

	fd = mmap((void*) 0, tamanoProceso, PROT_WRITE, MAP_PRIVATE, fd, 0);

	if (fd == MAP_FAILED){
		log_error(logger, "SWAP: Ocurrio un error en el memory map.");
		exit(1);
	}

	close(fd);
}

int cerrar_archivo(char* nombreArchivo, int tamanoProceso){

	retardo_swap();

    munmap(nombreArchivo, tamanoProceso);

//    close(nombreArchivo);

	return 1;
}
