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

typedef struct lista_operaciones{
	int t_instruccion;
	char* nombre_instruccion;
} lista_operaciones;

typedef struct instruccion{
	int operacion;
	char* operando_1;
	char* operando_2;
} instruccion;


#endif /* INSTRUCCIONES_H_ */
