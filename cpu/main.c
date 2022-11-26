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
#include "include/client_memoria.h"
#include "include/conexiones_cpu.h"
#include "include/tlb.h"
#include "include/mmu.h"

int main(void) {
	puts("ENCENDIDOS");
	iniciar_config_cpu();
	init_tlb();
	iniciar_mmu();
	start();

}
