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

typedef enum
{
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

int crear_conexion_memoria();
void enviar_mensaje(char* mensaje, int socket_cliente,op_code codigo_operacion);
char* recibir_mensaje(int socket_cliente);
void eliminar_paquete(t_paquete* paquete);
void liberar_conexion(int socket_cliente);



#endif /* CLIENT_MEMORIA_H_ */
