#ifndef PROTOCOLO_H_
#define PROTOCOLO_H_

#include "shared_commons.h"

typedef enum {
	PCB_NEW,
    PCB_READY,
    PCB_EXIT
} t_estado_pcb;

typedef struct {
    size_t id_proceso;
    t_list* instrucciones;
    size_t program_counter;
    size_t registro_AX;
    size_t registro_BX;
    size_t registro_CX;
    size_t registro_DX;
    t_list* tabla_segmentos;
    t_estado_pcb estado_anterior;
    t_estado_pcb estado_actual;
} t_pcb;

typedef struct {
    size_t tamanio_segmento;
    size_t id_tabla_paginas;
} t_pcb_segmentos;

/**
 * Funciones utiles
 * */
void log_list_of_chars(t_log* logger, t_list* list);
void log_pcb(t_log* logger, t_pcb* pcb);
char* convertir_estado_pcb_a_string(t_estado_pcb estado);

/**
 * Logs obligatiorios
 */

void log_proceso_en_new(t_log* logger, t_pcb* proceso);
void log_proceso_cambio_de_estado(t_log* logger, t_pcb* proceso);

/**
 * Funciones para comunicacion entre consola y kernel
 * */
bool send_instrucciones_y_segmentos(int fd, t_list* instrucciones, t_list* segmentos);
bool recv_instrucciones_y_segmentos(int fd, t_list** instrucciones, t_list** segmentos);

/**
 * Funciones para enviar y recibir un PCB
 * */
bool send_pcb(int fd, t_pcb* pcb);
bool recv_pcb(int fd, t_pcb** pcb);

#endif