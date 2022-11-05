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


/**
 * Funciones para comunicacion entre conso y kernel
 * */
bool send_instrucciones(int fd, t_list* instrucciones);
bool recv_instrucciones(int fd, t_list** instrucciones);

/**
 * Funciones para enviar y recibir un PCB
 * */
// bool send_pcb(int fd, t_pcb* pcb);
// bool recv_pcb(int fd, t_pcb** pcb);

#endif