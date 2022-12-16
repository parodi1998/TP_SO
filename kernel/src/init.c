#include "../include/init.h"

static bool iniciar_logger() {
    logger = log_create(LOGGER_FILE_AUX,LOGGER_NAME_AUX,true,LOG_LEVEL_DEBUG);
    logger_kernel_obligatorio = log_create(LOGGER_FILE,LOGGER_NAME,true,LOG_LEVEL_DEBUG);
    return logger != NULL;
}

static t_list* array_to_list(char** arr) {
    t_list* list = list_create();
    int index;
    for(index = 0; arr[index] != NULL; index++) {
        list_add(list, arr[index]);
    }
    return list;
}

static bool cargar_config(t_config_kernel* config_kernel) {
    t_config* config = config_create(CONFIG_FILE);
    if(config != NULL) {
        config_kernel->ip_memoria = string_new();
        config_kernel->puerto_memoria = string_new();
        config_kernel->ip_cpu = string_new();
        config_kernel->puerto_cpu_dispatch = string_new();
        config_kernel->puerto_cpu_interrupt = string_new();
        config_kernel->ip_escucha = string_new();
        config_kernel->puerto_escucha = string_new();
        config_kernel->nombre_escucha = string_new();
        config_kernel->algoritmo_planificacion = string_new();
        config_kernel->grado_max_multiprogramacion = string_new();
        config_kernel->quantum_RR = string_new();
        string_append(&config_kernel->ip_memoria, config_get_string_value(config, "IP_MEMORIA"));
        string_append(&config_kernel->puerto_memoria, config_get_string_value(config, "PUERTO_MEMORIA"));
        string_append(&config_kernel->ip_cpu, config_get_string_value(config, "IP_CPU"));
        string_append(&config_kernel->puerto_cpu_dispatch, config_get_string_value(config, "PUERTO_CPU_DISPATCH"));
        string_append(&config_kernel->puerto_cpu_interrupt, config_get_string_value(config, "PUERTO_CPU_INTERRUPT"));
        string_append(&config_kernel->puerto_escucha, config_get_string_value(config, "PUERTO_ESCUCHA"));
        string_append(&config_kernel->nombre_escucha, config_get_string_value(config, "NOMBRE_ESCUCHA"));
        string_append(&config_kernel->algoritmo_planificacion, config_get_string_value(config, "ALGORITMO_PLANIFICACION"));
        string_append(&config_kernel->grado_max_multiprogramacion, config_get_string_value(config, "GRADO_MAX_MULTIPROGRAMACION"));
        string_append(&config_kernel->quantum_RR, config_get_string_value(config, "QUANTUM_RR"));
        string_append(&config_kernel->ip_escucha, "127.0.0.1");
        config_kernel->dispositivos_IO = array_to_list(config_get_array_value(config, "DISPOSITIVOS_IO"));
        config_kernel->tiempos_IO = array_to_list(config_get_array_value(config, "TIEMPOS_IO"));
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

static void free_config_kernel() {
    free(config_kernel->ip_memoria);
    free(config_kernel->puerto_memoria);
    free(config_kernel->ip_cpu);
    free(config_kernel->puerto_cpu_dispatch);
    free(config_kernel->puerto_cpu_interrupt);
    free(config_kernel->ip_escucha);
    free(config_kernel->puerto_escucha);
    free(config_kernel->nombre_escucha);
    free(config_kernel->algoritmo_planificacion);
    free(config_kernel->grado_max_multiprogramacion);
    free(config_kernel->quantum_RR);
    list_destroy_and_destroy_elements(config_kernel->dispositivos_IO, free);
    list_destroy_and_destroy_elements(config_kernel->tiempos_IO, free);
    free(config_kernel);
}

void terminar_programa() {
    log_destroy(logger);
    log_destroy(logger_kernel_obligatorio);
    free_config_kernel();
}