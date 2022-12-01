#include "../include/memory_configuration_manager.h"
#include <stdint.h>

char* server_port;
uint32_t memory_size;
uint32_t page_size;
uint32_t entries_per_table;
uint32_t memory_delay_time;
char* replacement_algorithm;
uint32_t frames_per_process;
uint32_t swap_delay_time;
char* swap_file_path;
uint32_t swap_size;
char* IP;

t_config* CONFIG;

t_log* logger;

void initialize_memory_configuration_manager(){
	CONFIG = config_create("../Memory/src/memory.config");

    server_port = config_get_string_value(CONFIG,"PUERTO_ESCUCHA");
    memory_size = (volatile uint32_t) config_get_int_value(CONFIG,"TAM_MEMORIA");
    page_size = (volatile uint32_t) config_get_int_value(CONFIG,"TAM_PAGINA");
    entries_per_table = (volatile uint32_t) config_get_int_value(CONFIG,"ENTRADAS_POR_TABLA");
    memory_delay_time = (volatile uint32_t) config_get_int_value(CONFIG,"RETARDO_MEMORIA");
    replacement_algorithm = config_get_string_value(CONFIG,"ALGORITMO_REEMPLAZO");
    frames_per_process = (volatile uint32_t) config_get_int_value(CONFIG,"MARCOS_POR_PROCESO");
    swap_delay_time = (volatile uint32_t) config_get_int_value(CONFIG,"RETARDO_SWAP");
    swap_file_path = config_get_string_value(CONFIG,"PATH_SWAP");
    swap_size = (volatile uint32_t) config_get_int_value(CONFIG,"TAMANIO_SWAP");
    IP = config_get_string_value(CONFIG,"IP_MEMORIA");
    initialize_logger();
}
char* port_getter(){
    return server_port;
}
uint32_t memory_size_getter(){
    return memory_size;
}
uint32_t page_size_getter(){
    return page_size;
}
uint32_t entries_per_table_getter(){
    return entries_per_table;
}
uint32_t memory_time(){
    return memory_delay_time;
}
char* algorithm(){
    return replacement_algorithm;
}
uint32_t swap_time(){
    return swap_delay_time;
}
uint32_t frames_per_table_getter(){
    return frames_per_process;
}
char* swap_path(){
    return swap_file_path;
}


void free_memory_configuration_manager(){
    config_destroy(CONFIG);
}

void initialize_logger(){
	logger = log_create("Memory.log", "Memory", true, LOG_LEVEL_INFO);
}

t_log* get_logger(){
	return logger;
}

uint32_t swap_size_getter(){
	return swap_size;
}

char* get_ip(){
	return IP;
}
