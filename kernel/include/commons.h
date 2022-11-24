#ifndef KERNEL_COMMONS_H_
#define KERNEL_COMMONS_H_

#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/collections/dictionary.h>
#include "../../shared/include/sockets.h"
#include "../../shared/include/protocolo.h"
#include "../../shared/include/server_utils.h"
#include "../../shared/include/shared_commons.h"

#define LOGGER_FILE "kernel.log"
#define LOGGER_NAME "KERNEL"

#define LOGGER_FILE_AUX "kernel_aux.log"
#define LOGGER_NAME_AUX "KERNEL_AUX"

#define CONFIG_FILE "kernel.config"

typedef struct {
    char* ip_memoria;
    char* puerto_memoria;
    char* ip_cpu;
    char* puerto_cpu_dispatch;
    char* puerto_cpu_interrupt;
    char* ip_escucha;
    char* puerto_escucha;
    char* nombre_escucha;
    char* algoritmo_planificacion;
    char* grado_max_multiprogramacion;
    t_list* dispositivos_IO;
    t_list* tiempos_IO;
    char* quantum_RR;
} t_config_kernel;

typedef struct {
    t_log* log;
    int fd;
    char* server_name;
} t_procesar_conexion_args;

t_log* logger;
t_log* logger_kernel_obligatorio;
t_config_kernel* config_kernel;

#endif