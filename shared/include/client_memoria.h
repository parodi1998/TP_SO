/*
 * client_memoria.h
 *
 *  Created on: 26 nov. 2022
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
#include<commons/log.h>
#include<semaphore.h>
#include "../include/protocolo.h"
#include "../include/sockets.h"

typedef enum
{
		CONFIG_CPU = 0,
		INICIAR_PROCESO = 1,
		TRADUCIR = 2,
		ESCRIBIR = 3,
		LEER = 4,
		SUSPENDER_PROCESO = 5,
		FINALIZAR_PROCESO = 6,
		TRADUCCION_EXITOSA = 7,
		PAGE_FAULT_MEMORIA = 8,
		SWAP_PAGE_MEMORIA = 9,
		OK = 10,
		ERROR = 11,
		ACTUALIZAR_TLB = 12
} op_code_memoria;


typedef struct
{
	op_code_memoria codigo_operacion;
	t_buffer* buffer;
} t_paquete_memoria;

int crear_conexion_memoria(char* ip, char* port);
bool generar_conexion_kernel_a_memoria(t_log* logger, char* ip, char* puerto, int* fd_memoria);
bool generar_conexion_kernel_a_cpu_dispatch(t_log* logger, char* ip, char* puerto, int* fd_cpu_dispatch);
bool generar_conexion_kernel_a_cpu_interrupt(t_log* logger, char* ip, char* puerto, int* fd_cpu_interrupt);
void* serializar_paquete_memoria(t_paquete_memoria* paquete, int bytes);
void enviar_mensaje_memoria(char* mensaje, int socket_cliente,op_code_memoria codigo_operacion);
char* recibir_mensaje_memoria(int socket_cliente);
void eliminar_paquete_memoria(t_paquete_memoria* paquete);
void liberar_conexion_memoria(int socket_cliente);

char* page_fault_memoria(int socket_cliente,t_log* logger,uint32_t pid,uint32_t segment, uint32_t page);
char* finalizar_proceso_memoria(int socket_cliente, sem_t* sincro, t_log* logger,uint32_t pid);
char* iniciar_segmento_memoria(int socket_cliente, sem_t* sincro, t_log* logger,uint32_t pid,uint32_t segmento, uint32_t tamanio);

char* traducir_memoria(int socket_cliente,t_log* logger,uint32_t pid,uint32_t segment, uint32_t page,uint32_t es_escritura);
char* leer_memoria(int socket_cliente,t_log* logger,uint32_t pid,uint32_t dir_fisica, uint32_t tamanio);
char* escribir_memoria(int socket_cliente,t_log* logger,uint32_t pid,uint32_t dir_fisica,uint32_t tamanio, char* contenido);
char* recibir_config_para_mmu(int socket_cliente,t_log* logger);


#endif /* CLIENT_MEMORIA_H_ */
