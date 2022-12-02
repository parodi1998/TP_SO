/*
 * server.h
 *
 *  Created on: 26 ago. 2022
 *      Author: utnso
 */

#ifndef SERVER_H_
#define SERVER_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<string.h>
#include<pthread.h>
#include<commons/string.h>

typedef enum
{
	CONFIG_CPU,
	INICIAR_PROCESO,
	TRADUCIR,
	ESCRIBIR,
	LEER,
	SUSPENDER_PROCESO,
	FINALIZAR_PROCESO,
	TRADUCCION_EXITOSA,
	PAGE_FAULT,
	SWAP_PAGE,
	OK,
	ERROR
}op_code;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;


void* recibir_buffer_memory(int*, int);

void iniciar_servidor_memory(void);
bool iniciar_server_memoria(int* fd);
int iniciar_servidor(t_log* logger, const char* name, char* ip, char* puerto);
void esperar_cliente_memory(int);
void* recibir_mensaje_memory(int socket_cliente, int* size);
int recibir_operacion_memory(int);
void process_request_memory(int cod_op, int cliente_fd);
void serve_client_memory(int *socket);
void* serializar_paquete_memory(t_paquete* paquete, int bytes);
void devolver_mensaje_memory(void* payload, int size, int socket_cliente,int op_code);


char* process_numeric_response(int32_t response);
void* procesar_mensaje_iniciar_proceso(char* string,int* size, int* op_code);
void* procesar_finalizar_proceso(char* string,int* size, int* op_code);
void* procesar_suspender_proceso(char* string,int* size, int* op_code);
void* procesar_mensaje_escribir(char* string,int* size, int* op_code);
void* procesar_mensaje_leer(char* string,int* size, int* op_code);
void* procesar_traducir_direccion(char* string,int* size, int* op_code);
void* procesar_swapping(char* string, int* size,int* op_code);

#endif /* SERVER_H_ */
