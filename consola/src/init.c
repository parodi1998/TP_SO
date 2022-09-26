#include "../include/init.h"

void iniciar_programa() {
    iniciar_logger();
}

void iniciar_logger() {
    logger = log_create(LOGGER_FILE,LOGGER_NAME,true,LOG_LEVEL_DEBUG);
}

bool cargar_config(t_config_consola* config_consola) {
    t_config* config = config_create(CONFIG_FILE);
    if(config != NULL) {
        config_consola->nombre = string_new();
        config_consola->ip = string_new();
        config_consola->puerto = string_new();
        string_append(&config_consola->ip, config_get_string_value(config, "IP_KERNEL"));
        string_append(&config_consola->puerto, config_get_string_value(config, "PUERTO_KERNEL"));
        string_append(&config_consola->nombre, config_get_string_value(config, "NOMBRE"));
        config_destroy(config);
        return true;
    } else {
        return false;
    }
}

void terminar_programa() {
    log_destroy(logger);
    
    //free(config_consola);
}