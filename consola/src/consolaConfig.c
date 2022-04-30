#include "consolaConfig.h"

void setConfigurationParameters() {
	t_config* config = config_create("config/config.cfg");
	if (!config) {
		printf("No encuentro el archivo config\n");
		exit(1);
	}

	IP_CONFIG_KERNEL = malloc(sizeof(char) * 100);
	strcpy(IP_CONFIG_KERNEL,config_get_string_value(config, "IP_CONFIG_KERNEL"));

	IP_MINE = malloc(sizeof(char) * 100);
	strcpy(IP_MINE,config_get_string_value(config, "IP_MINE"));

	PORT_CONFIG_KERNEL = config_get_int_value(config,"PORT_CONFIG_KERNEL");
	LISTENING_PORT = config_get_int_value(config,"LISTENING_PORT");

	config_destroy(config);
}

void configure_logger() {
	LOGGER = log_create("log-PLANIFICADOR.log","tp",0,LOG_LEVEL_INFO);
	log_info(LOGGER, "Empezamos.....");
}

void logger_mensaje(char* mensaje) {
	pthread_mutex_lock(&MUTEX_LOGGER);
	log_info(LOGGER,mensaje);
	pthread_mutex_unlock(&MUTEX_LOGGER);
}

void logger_mensaje_error(char* mensaje) {
	pthread_mutex_lock(&MUTEX_LOGGER);
	log_error(LOGGER,mensaje);
	pthread_mutex_unlock(&MUTEX_LOGGER);
}
