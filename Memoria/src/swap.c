#include "swap.h"

//-----------------------------SWAP-----------------------------

void iniciarSwap(){

 //TODO: Ver si tengo que iniciarlo

}

void retardo_swap(){

	sleep(config->swap_time_delay/1000);
}

char* obtener_nombre_archivo(int pid){

	log_info(logger, "SWAP: proceso %d", pid);
	//para guardar el pid
	char buffer[5];

	//lo paso a entero
	sprintf(buffer, "%d",pid);
	log_info(logger, "SWAP: Voy a agregarle .swap al pid");
	//hago la concatenación
	char* nombreArchivo = strcpy(buffer, ".swap");
	log_info(logger, "SWAP: Nombre: %s", nombreArchivo);

	return nombreArchivo;
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
		ret = 1;
		close(f);
	}

	return ret;
}



void crear_archivo_swap(int pid){ //archivo del tamanio del proceso
	log_info(logger, "%d", pid);
	char* pathSwap = config->path_swap;
	char* path = "";

	struct stat st = {0};

	log_info(logger, "MEMORIA: Se accede a swap..");
	retardo_swap();

	if (stat(pathSwap, &st) == -1) {
		log_info(logger, "SWAP: Se crea el archivo de swap en %s", pathSwap);
	    mkdir(pathSwap, 0700);
	}
	else
		log_info(logger, "SWAP: El directorio swap ya existe.");

	log_info(logger, "SWAP: Se obtiene el nombre del archivo");
	char* nombreArchivo = obtener_nombre_archivo(pid);

	strcpy(path, pathSwap);
	strcpy(path, "/");
	strcpy(path, nombreArchivo);

	log_info(logger, "MEMORIA path: %s", path);


	log_info(logger, "MEMORIA: Verifico si existe el archivo");
	if(!existe_archivo(path)){

		//FILE* archivo = fopen(nombreArchivo, "wt"); borrar si funciona lo de abajo
		log_info(logger, "Se crea el archivo con el nombre %s", nombreArchivo);
		int archivo = open(nombreArchivo, O_RDWR);

		int tamanioArchivo = config->table_input * config->page_size;
		log_info(logger, "Se setea con \0");
		memset(&archivo,'\0', tamanioArchivo);

		log_info(logger, "SWAP: Se creo el archivo %s", nombreArchivo);
		log_info(logger, " del proceso %d", pid);
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
	int cantidadDeProcesos = list_size(procesos);
	int b = 0, i = 0;

	retardo_swap();

	while(b == 0){


		proceso = list_get(procesos, i);
		i++;
		if (proceso->pid == pidRecibido){

			tamanoProceso = proceso->tamanoProceso;

			char* nombreArchivo = obtener_nombre_archivo(pidRecibido);
			char* pathArchivo = strcpy(config->path_swap, strcpy("/", nombreArchivo));

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

    close(nombreArchivo);

	return 1;
}
