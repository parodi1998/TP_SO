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
#include "config_manager/memory_configuration_manager.h"
#include "memory_file_management.h"
#include "connections/server.h"

int main(void) {
	initialize_memory_configuration_manager();
	initialize_memory_file_management();
	iniciar_servidor();
	return EXIT_SUCCESS;
}
