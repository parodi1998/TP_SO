#ifndef UTILS_H_
#define UTILS_H_
/*
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>

typedef enum {
    MENSAJE,
    PAQUETE,
    DEBUG = 100,
} op_code;

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


typedef struct cpu_config{
  int entrada;
  char* reemplazo;
  int retardo;
  char* ip_memoria;
  int puerto_memoria;
  int puerto_escucha_dispatch;
  int puerto_escucha_interrupt;

}cpu_config ;


int crear_conexion(char* ip, char* puerto);
void enviar_mensaje(char* mensaje, int socket_cliente,op_code codigo_operacion);
t_paquete* crear_paquete(void);
t_paquete* crear_super_paquete(void);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void liberar_conexion(int socket_cliente);
void eliminar_paquete(t_paquete* paquete);
char* recibir_mensaje(int socket_cliente);*/

#endif /* UTILS_H_ */
