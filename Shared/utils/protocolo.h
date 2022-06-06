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


t_buffer* armar_buffer (uint32_t tamano_estructura,void* stream);

void* serializar_paquete(t_paquete* paquete, void* a_enviar);

t_paquete *empaquetar_buffer(t_buffer* buffer);

#endif
