/*
 * server.h
 *
 *  Created on: 26 ago. 2022
 *      Author: utnso
 */

#ifndef CONNECTIONS_SERVER_H_
#define CONNECTIONS_SERVER_H_

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
	INICIAR_PROCESO,
	ESCRIBIR,
	LEER,
	SUSPENDER_PROCESO,
	RESTAURAR_PROCESO,
	FINALIZAR_PROCESO,
	MENSAJE
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

pthread_t thread;

void* recibir_buffer(int*, int);

void iniciar_servidor(void);
void esperar_cliente(int);
void* recibir_mensaje(int socket_cliente, int* size);
int recibir_operacion(int);
void process_request(int cod_op, int cliente_fd);
void serve_client(int *socket);
void* serializar_paquete(t_paquete* paquete, int bytes);
void devolver_mensaje(void* payload, int size, int socket_cliente);


char* process_numeric_response(int32_t response);
void* procesar_mensaje_iniciar_proceso(char* string,int* size);
void* procesar_restaurar_proceso(char* string,int* size);
void* procesar_finalizar_proceso(char* string,int* size);
void* procesar_suspender_proceso(char* string,int* size);
void* procesar_mensaje_escribir(char* string,int* size);
void* procesar_mensaje_leer(char* string,int* size);

#endif /* CONNECTIONS_SERVER_H_ */
