#ifndef PROTOCOLO_H_
#define PROTOCOLO_H_

#include "shared_commons.h"

typedef enum {
	PCB_NEW,
    PCB_READY,
    PCB_EXECUTE,
    PCB_BLOCK,
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
    size_t consola_fd;
    bool debe_ser_finalizado;
    bool debe_ser_bloqueado;
    bool puede_ser_interrumpido;
    bool fue_interrumpido;
    char* dispositivo_bloqueo;
    size_t registro_para_bloqueo;
    size_t unidades_de_trabajo;
} t_pcb;

/*
    
    char* dispositivo_bloqueo; // CONSOLA, TECLADO, DISCO
    size_t unidades_de_trabajo_o_registro_de_bloqueo;
*/

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
void liberar_pcb(t_pcb* proceso);

/**
 * Logs obligatiorios
 */

void log_proceso_en_new(t_log* logger, t_pcb* proceso);
void log_procesos_en_ready(t_log* logger, t_list* procesos_fifo, t_list* procesos_rr, char* algoritmo);
void log_proceso_cambio_de_estado(t_log* logger, t_pcb* proceso);
void actualizar_estado_proceso(t_log* logger, t_pcb* proceso, t_estado_pcb nuevo_estado);
void log_motivo_de_bloqueo(t_log* logger, t_pcb* proceso, char* dispositivo);

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