#include "../include/init.h"

static bool iniciar_logger() {
    logger = log_create(LOGGER_FILE,LOGGER_NAME,true,LOG_LEVEL_DEBUG);
    return logger != NULL;
}

static bool cargar_config(t_config_kernel* config_kernel) {
    t_config* config = config_create(CONFIG_FILE);
    if(config != NULL) {
        config_kernel->ip_memoria = string_new();
        config_kernel->puerto_memoria = string_new();
        config_kernel->ip_cpu = string_new();
        config_kernel->puerto_cpu_dispatch = string_new();
        config_kernel->puerto_cpu_interrupt = string_new();
        config_kernel->puerto_escucha = string_new();
        config_kernel->algoritmo_planificacion = string_new();
        config_kernel->grado_max_multiprogramacion = string_new();
        config_kernel->dispositivos_IO = string_new();
        config_kernel->tiempos_IO = string_new();
        config_kernel->quantum_RR = string_new();
        string_append(&config_kernel->ip_memoria, config_get_string_value(config, "IP_MEMORIA"));
        string_append(&config_kernel->puerto_memoria, config_get_string_value(config, "PUERTO_MEMORIA"));
        string_append(&config_kernel->ip_cpu, config_get_string_value(config, "IP_CPU"));
        string_append(&config_kernel->puerto_cpu_dispatch, config_get_string_value(config, "PUERTO_CPU_DISPATCH"));
        string_append(&config_kernel->puerto_cpu_interrupt, config_get_string_value(config, "PUERTO_CPU_INTERRUPT"));
        string_append(&config_kernel->puerto_escucha, config_get_string_value(config, "PUERTO_ESCUCHA"));
        string_append(&config_kernel->algoritmo_planificacion, config_get_string_value(config, "ALGORITMO_PLANIFICACION"));
        string_append(&config_kernel->grado_max_multiprogramacion, config_get_string_value(config, "GRADO_MAX_MULTIPROGRAMACION"));
        string_append(&config_kernel->dispositivos_IO, config_get_string_value(config, "DISPOSITIVOS_IO"));
        string_append(&config_kernel->tiempos_IO, config_get_string_value(config, "TIEMPOS_IO"));
        string_append(&config_kernel->quantum_RR, config_get_string_value(config, "QUANTUM_RR"));
        config_destroy(config);
        return true;
    } else {
        return false;
    }
}

static bool iniciar_config() {
    config_kernel = malloc(sizeof(t_config_kernel));
    if(!cargar_config(config_kernel)) {
        log_error(logger,"No se pudo abrir el archivo de configuracion kernel.config");
        return false;
    }
    return true;
}

bool iniciar_programa() {
    if(!iniciar_logger()) {
        return false;
    } else {
        if(!iniciar_config()) {
            return false;
        }
    }
    return true;
}

void terminar_programa() {
    log_destroy(logger);
    free(config_kernel);
}