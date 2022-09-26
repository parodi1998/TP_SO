#include "../include/init.h"

void iniciar_programa() {
    iniciar_logger();
}

void iniciar_logger() {
    logger = log_create(LOGGER_FILE,LOGGER_NAME,true,LOG_LEVEL_DEBUG);
}

bool cargar_config(t_config_consola* config_consola, char* path) {
    t_config* config = config_create(path);
    if(config != NULL) {
        config_consola->ip_kernel = string_new();
        config_consola->puerto_kernel = string_new();
        config_consola->segmentos = string_new();
        config_consola->tiempo_pantalla = string_new();
        string_append(&config_consola->ip_kernel, config_get_string_value(config, "IP_KERNEL"));
        string_append(&config_consola->puerto_kernel, config_get_string_value(config, "PUERTO_KERNEL"));
        string_append(&config_consola->segmentos, config_get_string_value(config, "SEGMENTOS"));
        string_append(&config_consola->tiempo_pantalla, config_get_string_value(config, "TIEMPO_PANTALLA"));
        config_destroy(config);
        return true;
    } else {
        return false;
    }
}

void terminar_programa() {
    log_destroy(logger);
    free(config_consola);
}