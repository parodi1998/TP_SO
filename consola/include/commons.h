#ifndef CONSOLA_COMMONS_H_
#define CONSOLA_COMMONS_H_

#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>

#define LOGGER_FILE "consola.log"
#define LOGGER_NAME "CONSOLA"

#define CONFIG_FILE "consola.config"

typedef struct {
    char* ip_kernel;
    char* puerto_kernel;
    char* segmentos;
    char* tiempo_pantalla;
} t_config_consola;

t_log* logger;
t_config_consola* config_consola;

#endif