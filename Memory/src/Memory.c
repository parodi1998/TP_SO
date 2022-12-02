/*
 ============================================================================
 Name        : Memory.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "../include/memory_configuration_manager.h"
#include "../include/memory_file_management.h"
#include "../include/server.h"

int main(void) {
	initialize_memory_configuration_manager();
	initialize_memory_file_management();
	log_info(get_logger(),"INICIANDO SERVIDOR..");
	iniciar_servidor_memory();
	end_memory_module();

	return EXIT_SUCCESS;
}
