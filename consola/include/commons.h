#ifndef CONSOLA_COMMONS_H_
#define CONSOLA_COMMONS_H_

#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include "../../shared/include/sockets.h"
#include "../../shared/include/protocolo.h"
#include "../../shared/include/shared_commons.h"

#define LOGGER_FILE "consola.log"
#define LOGGER_NAME "CONSOLA"

typedef struct {
    char* ip_kernel;
    char* puerto_kernel;
    t_list* segmentos;
    char* tiempo_pantalla;
} t_config_consola;

t_log* logger;
t_config_consola* config_consola;

#endif