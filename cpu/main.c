/*
 ============================================================================
 Name        : c.c
 Author      : joaquin
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "include/instrucciones.h"
#include "include/config_cpu.h"
#include "include/conexiones_cpu.h"
#include "include/tlb.h"
#include "include/mmu.h"

int fd_server_cpu_dispatch;
int fd_server_cpu_interrupt;

int main(void) {
	puts("ENCENDIDOS");
	iniciar_config_cpu();
	iniciar_mmu();

	fd_server_cpu_dispatch = 0;
	if(!iniciar_server_cpu_dispatch(&fd_server_cpu_dispatch)) {
        log_error(get_log(),"No se iniciar el server cpu escucha dispatch");
        // terminar_programa(); deberia llamarse a una funcion que libere toda la memoria de loggers, configs, listas, giladas
        return EXIT_FAILURE;
    }
	
	
	fd_server_cpu_interrupt = 0;
	if(!iniciar_server_cpu_interrupt(&fd_server_cpu_interrupt)) {
        log_error(get_log(),"No se iniciar el server cpu escucha interrupt");
        // terminar_programa(); deberia llamarse a una funcion que libere toda la memoria de loggers, configs, listas, giladas
        return EXIT_FAILURE;
    }
	
	log_info(get_log(),"INICIANDO CPU");
	start();
}
