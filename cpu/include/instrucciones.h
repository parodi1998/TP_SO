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
#include<unistd.h>
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
#include "mmu.h"
#include "config_cpu.h"

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
	PAGE_DEFAULT,
	FINALIZADO
};

typedef struct operando{
	char* nombre;
	int* apunta;
} operando;

typedef struct t_registros{
	int ax;
	int bx;
	int cx;
	int dx;
} t_registros;

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
	int program_counter;
	t_registros reg_general;
	char * io_dispositivo;
	int io_unidades;
	int estado;
	char* io_registro;
	t_list* tabla_segmentos;
} t_contexto_ejecucion;


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
void* seguir_instrucciones(t_contexto_ejecucion*, t_list*,  int);
void fetch(int, t_list*, char**);


int decodificaOperando(operando*);
void accederMemoria(int, t_list*);
int* operandoBusca(char*);
void decodificar(char*, t_list*, int);
int ejecutar(void);
int ins_set(void);
int ins_add(void);
int ins_mov_in(void);
int ins_mov_out(void);
int ins_io(void);
int  ins_exit(void);
int check_interrupt(int);
int ciclo_instrucciones(t_contexto_ejecucion*,  t_list*, int);
void* inicializar(void);
int unidades_en_registro(int*);
void interrumpe(void);
int* buscaOperando(char*);

#endif /* INSTRUCCIONES_H_ */
