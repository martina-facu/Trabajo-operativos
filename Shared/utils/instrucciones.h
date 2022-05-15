#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/collections/list.h>

typedef struct {
	t_list* parametros;
	uint8_t id; //NO_OP, I/O, READ, WRITE, COPY, EXIT
} Instruccion;

typedef struct {
	uint32_t size;
	void* stream;
} t_buffer;

typedef struct {
	uint8_t codigo_operacion;
	t_buffer* buffer;
} t_paquete;
