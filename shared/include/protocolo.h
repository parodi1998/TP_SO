#ifndef PROTOCOLO_H_
#define PROTOCOLO_H_

#include "client_utils.h"
#include "server_utils.h"

typedef struct{
	int id;
	char* nombre;
	uint32_t parametro1;
	uint32_t parametro2;
}instruccion;

typedef struct{
	int id;
	char* nombre;
	char* dispositivo;
	uint32_t parametro;
}instruccion_IO;


bool send_instrucciones(int fd, t_list* instrucciones);
bool recv_instrucciones(int fd, t_list** instrucciones); 

#endif