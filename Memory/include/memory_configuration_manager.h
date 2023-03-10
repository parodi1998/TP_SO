
#ifndef MEMORY_CONFIGURATION_MANAGER_H_
#define MEMORY_CONFIGURATION_MANAGER_H_

#include <stdint.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>

void initialize_memory_configuration_manager();

char* port_getter();
uint32_t memory_size_getter();
uint32_t page_size_getter();
uint32_t entries_per_table_getter();
uint32_t memory_time();
char* algorithm();
uint32_t swap_time();
uint32_t frames_per_table_getter();
char* swap_path();
void initialize_logger();
t_log* get_logger();
uint32_t swap_size_getter();
char* get_ip();
void destroy_config();
void destroy_logger();

#endif /*MEMORY_CONFIGURATION_MANAGER_H_*/
