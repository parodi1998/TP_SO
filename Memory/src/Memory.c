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
#include <signal.h>

int main(void) {
	signal(SIGINT,end_memory_module);
	initialize_memory_configuration_manager();
	initialize_memory_file_management();
	log_info(get_logger(),"INICIANDO SERVIDOR..");
	iniciar_servidor_memory();

	return EXIT_SUCCESS;
}
