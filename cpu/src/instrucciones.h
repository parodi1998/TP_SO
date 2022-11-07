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
#include <commons/collections/list.h>
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
	int* apunta;
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

typedef struct{
	int nro;
	int tam;
	int nro_indice_tabla_paginas;
} t_segmento;

typedef struct {
	t_list* instrucciones;
	int program_counter;
	registros reg_general;
	char * io_dispositivo;
	int io_unidades;
	int estado;
	t_list* tabla_segmentos;
} t_contexto_ejecucion;

typedef struct t_pcb{
	uint32_t id_proceso;
	t_list* instrucciones;
	uint32_t program_counter;
	uint32_t registro_AX;
	uint32_t registro_BX;
	uint32_t registro_CX;
	uint32_t registro_DX;
	uint32_t tabla_segmentos;
	char* estado;
} t_pcb;

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
void* seguir_instrucciones(t_contexto_ejecucion*);
//char* fetch(t_contexto_ejecucion*);
void* todas_operaciones(void);


int decodificaOperando(operando*);
//void accederMemoria(operando*);
void accederMemoria(int*);
//int llamaMemoria(int*, char*);
int* operandoBusca(char*);
//instruccion decodificar(char*);
void decodificar(char*);
//int ejecutar(instruccion*);
int ejecutar(void);
//int ins_set(instruccion*);
int ins_set(void);
int ins_add(void);
void ins_mov_in(instruccion);
int ins_io(void);
int  ins_exit(void);
int check_interrupt(int);
int ciclo_instrucciones(t_contexto_ejecucion*);

#endif /* INSTRUCCIONES_H_ */
