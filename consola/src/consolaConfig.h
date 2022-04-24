#ifndef CONSOLA_CONFIGURATION_H
#define CONSOLA_CONFIGURATION_H

#include <commons/collections/list.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <commons/config.h>
#include <commons/string.h>
#include <pthread.h>
#include <commons/log.h>

t_log * LOGGER;
pthread_mutex_t MUTEX_LOGGER;

char* IP_CONFIG_KERNEL;
int32_t PORT_CONFIG_KERNEL;
char* IP_MINE;
int32_t LISTENING_PORT;

void setConfigurationParameters();

void configure_logger();

void logger_mensaje(char* mensaje);

void logger_mensaje_error(char* mensaje);

#endif
