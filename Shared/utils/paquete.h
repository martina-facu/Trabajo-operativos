#ifndef PAQUETE
#define PAQUETE

#include <stdint.h>

typedef struct {
	uint32_t size;
	void* stream;
} t_buffer;

typedef struct {
	uint8_t codigo_operacion;
	t_buffer* buffer;
	uint32_t size;
} t_paquete;


#endif
