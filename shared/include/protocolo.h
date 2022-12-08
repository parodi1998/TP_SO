#ifndef PROTOCOLO_H_
#define PROTOCOLO_H_

#include "shared_commons.h"
#include "sockets.h"
#include "client_memoria.h"
#include "client_utils.h"
#include "server_utils.h"

typedef enum {
	PCB_NEW,
    PCB_READY,
    PCB_EXECUTE,
    PCB_BLOCK,
    PCB_EXIT
} t_estado_pcb;

typedef struct {
    uint32_t id_proceso;
    t_list* instrucciones;
    uint32_t program_counter;
    uint32_t registro_AX;
    uint32_t registro_BX;
    uint32_t registro_CX;
    uint32_t registro_DX;
    t_list* tabla_segmentos;
    t_estado_pcb estado_anterior;
    t_estado_pcb estado_actual;
    uint32_t consola_fd;
    bool debe_ser_finalizado;
    bool debe_ser_bloqueado;
    bool puede_ser_interrumpido;
    bool fue_interrumpido;
    char* dispositivo_bloqueo;
    uint32_t registro_para_bloqueo;
    uint32_t unidades_de_trabajo;
    uint32_t page_fault_segmento;
    uint32_t page_fault_pagina;
} t_pcb;

typedef struct {
    uint32_t tamanio_segmento;
    uint32_t id_tabla_paginas;
} t_pcb_segmentos;

/**
 * Funciones utiles
 * */
void liberar_pcb(t_pcb* proceso);
void log_pcb(t_log* logger, t_pcb* proceso);

/**
 * Funciones para enviar y recibir un PCB
 * */
bool send_pcb(t_log* logger,int fd, t_pcb* pcb);
bool recv_pcb(t_log* logger,int fd, t_pcb** pcb);

char* finalizar_proceso_consola(int socket_cliente, t_log* logger);

bool send_op_code(t_log* logger, uint32_t fd, op_code cod_op);
bool recv_op_code(t_log* logger, uint32_t fd, op_code* cod_op);

#endif