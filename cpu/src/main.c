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
#include "conexiones.h"
#include "instrucciones.h"
#include "config_cpu.h"

int main(void) {
	puts("ENCENDIDOS");
	iniciar_config_cpu();
	//int valor = start(2);
	int valor = 0;
	start_prueba();
	return valor;
}
