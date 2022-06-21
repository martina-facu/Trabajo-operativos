#ifndef SWAP_H_
#define SWAP_H_

#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/bitarray.h>


//prototipos funciones
int existe_archivo(char* path);
void iniciarSwap();
void crear_archivo_swap(int pid);
void eliminar_archivo_swap(int pid, t_log* logger);
int retardo_swap();

#endif /* SWAP_H_ */
