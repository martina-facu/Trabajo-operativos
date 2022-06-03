#ifndef PROTOCOLO
#define PROTOCOLO

#include <stdio.h>
#include <string.h>
#include "./paquete.h"
#include <ctype.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/config.h>
#include "instrucciones.h"

void llenar_stream_instruccion(t_list* instrucciones,void* stream);

t_paquete* empaquetar_instrucciones(t_list* instrucciones);

void* serializar_paquete(t_paquete* paquete);

t_paquete *empaquetar_buffer(t_buffer* buffer);

#endif
