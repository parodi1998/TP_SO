#ifndef KERNEL_COMMONS_H_
#define KERNEL_COMMONS_H_

#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>

#define LOGGER_FILE "kernel.log"
#define LOGGER_NAME "KERNEL"

#define CONFIG_FILE "kernel.config"

typedef struct {
    char* ip_memoria;
    char* puerto_memoria;
    char* ip_cpu;
    char* puerto_cpu_dispatch;
    char* puerto_cpu_interrupt;
    char* puerto_escucha;
    char* algoritmo_planificacion;
    char* grado_max_multiprogramacion;
    char* dispositivos_IO;
    char* tiempos_IO;
    char* quantum_RR;
} t_config_kernel;

t_log* logger;
t_config_kernel* config_kernel;

#endif