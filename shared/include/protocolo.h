#ifndef PROTOCOLO_H_
#define PROTOCOLO_H_

#include "client_utils.h"
#include "server_utils.h"

typedef enum {
	PCB_NEW,
    PCB_READY,
    PCB_EXEC,
    PCB_EXIT,
    PCB_BLOCKED,
    PCB_SUSPENDED_BLOCKED,
    PCB_SUSPENDED_READY,
} t_estado_pcb;

typedef struct {
    size_t id_proceso;
    t_list* instrucciones;
    size_t program_counter;
    size_t registro_AX;
    size_t registro_BX;
    size_t registro_CX;
    size_t registro_DX;
    size_t tabla_segmentos;
    t_estado_pcb estado;
} t_pcb;

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
bool send_pcb(int fd, t_pcb* pcb);
bool recv_pcb(int fd, t_pcb** pcb);

#endif