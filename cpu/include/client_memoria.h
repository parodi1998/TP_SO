/*
 * client_memoria.h
 *
 *  Created on: 4 nov. 2022
 *      Author: utnso
 */

#ifndef CLIENT_MEMORIA_H_
#define CLIENT_MEMORIA_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/string.h>
#include "../../shared/include/protocolo.h"

typedef enum
{
	CONFIG_CPU,
	INICIAR_PROCESO,
	TRADUCIR,
	ESCRIBIR,
	LEER,
	SUSPENDER_PROCESO,
	RESTAURAR_PROCESO,
	FINALIZAR_PROCESO,
	TRADUCCION_EXITOSA,
	PAGE_FAULT,
	OK,
	ERROR
}op_code_memoria;

//typedef struct
//{
//	int size;
//	void* stream;
//} t_buffer;

typedef struct
{
	op_code_memoria codigo_operacion;
	t_buffer* buffer;
} t_paquete_memoria;

int crear_conexion_memoria_cpu();
void enviar_mensaje_memoria_cpu(char* mensaje, int socket_cliente,op_code_memoria codigo_operacion);
char* recibir_mensaje_memoria_cpu(int socket_cliente);
void liberar_conexion_memoria_cpu(int socket_cliente);
char* traducir_memoria(uint32_t pid,uint32_t segment, uint32_t page,uint32_t es_escritura);
char* leer_memoria(uint32_t pid,uint32_t dir_fisica, uint32_t tamanio);
char* escribir_memoria(uint32_t pid,uint32_t dir_fisica,uint32_t tamanio, char* contenido);
char* finalizar_proceso(uint32_t pid,uint32_t segment);
char* recibir_config_para_mmu();


#endif /* CLIENT_MEMORIA_H_ */
