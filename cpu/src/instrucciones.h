/*
 * instrucciones.h
 *
 *  Created on: Oct 16, 2022
 *      Author: utnso
 */

#ifndef INSTRUCCIONES_H_
#define INSTRUCCIONES_H_
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<semaphore.h>
#include "server.h"
#include "client.h"

#include "utils.h"

enum tipo_instruccion{
	SET,
	ADD,
	MOV_IN,
	MOV_OUT,
	IO,
	EXIT

};

enum tipo_devolucion{
	OPTIMO,
	BLOQUEO,
	INTERRUPCION,
	FINALIZADO
};

typedef struct operando{
	char* nombre;
	int apunta;
} operando;

typedef struct registros{
	int ax;
	int bx;
	int cx;
	int dx;
} registros;

typedef struct lista_instrucciones{
	int id;
	char* instruccion;
	struct lista_instrucciones* lista_siguiente;
} Lista_Instrucciones;

typedef struct tabla_de_segmentos{
	int nro;
	int tam;
	int nro_indice_pagina;
	struct tabla_de_segmentos* siguiente;
} Tabla_De_Segmentos;

typedef struct pcb{
	int id;
	Lista_Instrucciones* instrucciones;
	int program_counter;
	registros reg_general;
	Tabla_De_Segmentos* tabla_segmentos;
} pcb;

typedef struct lista_operaciones{
	int t_instruccion;
	char* nombre_instruccion;
} lista_operaciones;

typedef struct instruccion{
	int operacion;
	operando operando_1;
	operando operando_2;
} instruccion;

void* funcion_hilo(void*);
void* seguir_instrucciones(pcb*);
char* fetch(pcb*);
lista_operaciones* todas_operaciones(void);
int buscarValorOperacion (char*);
int esABCD(char*, int*);
int llamaMemoria(operando*);
void accederMemoria(operando*);
instruccion decodificar(char*);
int ejecutar(instruccion*);
int ins_set(instruccion*);
int ins_add(instruccion*);
void ins_mov_in(instruccion);
int ins_io(instruccion*);
int  ins_exit(instruccion*);
int check_interrupt(int);
int ciclo_instrucciones(pcb*);

#endif /* INSTRUCCIONES_H_ */
