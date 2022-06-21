
#include "swap.h"

void iniciarSwap(){

	//Verifico si el archivo existe, de lo contrario lo creo


}

int existe_archivo(char* path){

	int ret = 0;

	int f = open(path, "O_RDONLY");

	if (f != -1){ //si f no es -1, el archivo existe
		ret = 1;
		close(f);
	}

	return ret;
}

void crear_archivo_swap(int pid){ //archivo del tamanio del proceso

	//Copio el nomrbe del proceso y el string .swap
	char* nombreArchivo = strcpy(pid, ".swap");
	char* pathArchivo = strcpy(config->path_swap, strcpy("/", nombreArchivo));
	//Verifico si el archivo no existe

	if(!existe_archivo(pathArchivo)){


		FILE* archivo = fopen(nombreArchivo, "wt");

		log_info(logger, "Se creo el archivo %s", nombreArchivo);
		log_info(logger, " del proceso %d", pid);

		//completar con info de memoria
	}
}

void eliminar_archivo_swap(int pid, t_log* logger){

	char* nombreArchivo = strcpy(pid, ".swap");
	char* pathArchivo = strcpy(config->path_swap, strcpy("/", nombreArchivo));

	if(existe_archivo(pathArchivo)){
		if(remove(nombreArchivo)){
			log_info(logger, "Se eliminó el archivo %s", nombreArchivo);
		}
		else
			log_info(logger, "Ocurrio un error al eliminar el archivo %s", nombreArchivo);
	}

	else
		log_info(logger, "No se encontro el archivo %s", nombreArchivo);
}
/*
int retardo_swap(){

	return sleep(config->swap_time_delay);
}
*/
