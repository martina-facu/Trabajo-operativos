#ifndef PROTOCOLO
#define PROTOCOLO

#include <stdio.h>
#include <string.h>
#include <stdint.h>
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

void llenar_stream(t_list* instrucciones,void* stream);

void deserializar_instrucciones(t_buffer* buffer,t_list* instrucciones);

#endif
