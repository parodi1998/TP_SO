#include "../include/init.h"

void iniciar_programa() {
    iniciar_logger();
}

void iniciar_logger() {
    logger = log_create(LOGGER_FILE,LOGGER_NAME,true,LOG_LEVEL_DEBUG);
}

static t_list* array_to_list(char** arr) {
    t_list* list = list_create();
    int index;
    for(index = 0; arr[index] != NULL; index++) {
        list_add(list, arr[index]);
    }
    return list;
}

bool cargar_config(t_config_consola* config_consola, char* path) {
    t_config* config = config_create(path);
    if(config != NULL) {
        config_consola->ip_kernel = string_new();
        config_consola->puerto_kernel = string_new();
        config_consola->tiempo_pantalla = string_new();
        string_append(&config_consola->ip_kernel, config_get_string_value(config, "IP_KERNEL"));
        string_append(&config_consola->puerto_kernel, config_get_string_value(config, "PUERTO_KERNEL"));
        string_append(&config_consola->tiempo_pantalla, config_get_string_value(config, "TIEMPO_PANTALLA"));
        config_consola->segmentos = array_to_list(config_get_array_value(config, "SEGMENTOS"));
        config_destroy(config);
        return true;
    } else {
        return false;
    }
}

static void free_config_consola() {
    free(config_consola->ip_kernel);
    free(config_consola->puerto_kernel);
    free(config_consola->tiempo_pantalla);
    list_destroy_and_destroy_elements(config_consola->segmentos, free);
}

void terminar_programa() {
    log_destroy(logger);
    free_config_consola();
}