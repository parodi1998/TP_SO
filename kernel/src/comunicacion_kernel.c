#include "../include/comunicacion_kernel.h"

static void carga_tabla_segmentos_pcb( t_list** lista_a_cargar, t_list* segmentos) {
    t_list* tabla_segmentos = list_create();
    uint32_t index;
    uint32_t size = list_size(segmentos);
    for(index = 0; index < size; index++) {
        char* segmento = list_get(segmentos,index);
        t_pcb_segmentos* segmento_pcb = malloc(sizeof(t_pcb_segmentos));
        segmento_pcb->id_tabla_paginas = 0;
        segmento_pcb->tamanio_segmento = atoi(segmento);
        list_add(tabla_segmentos, segmento_pcb);
    }
    *lista_a_cargar = tabla_segmentos;
}

static void procesar_conexion(void* void_args) {
    t_procesar_conexion_args* args = (t_procesar_conexion_args*) void_args;
    t_log* logger = args->log;
    uint32_t cliente_fd = args->fd;
    char* server_name = args->server_name;
    free(args);

    op_code codigo;
    t_list* instrucciones;
    t_list* segmentos;
    t_pcb* pcb_proceso;

    while (cliente_fd != -1) {

        if (recv(cliente_fd, &codigo, sizeof(op_code), 0) != sizeof(op_code)) {
            // Una vez que recibimos las instrucciones y los segmentos, creamos el PCB y lo ponemos en la cola de ready
            generador_pcb_id++;
            pcb_proceso = malloc(sizeof(t_pcb));
            pcb_proceso->id_proceso = generador_pcb_id;
            pcb_proceso->program_counter = 0;
            carga_tabla_segmentos_pcb(&pcb_proceso->tabla_segmentos, segmentos);
            pcb_proceso->registro_AX = 0;
            pcb_proceso->registro_BX = 0;
            pcb_proceso->registro_CX = 0;
            pcb_proceso->registro_DX = 0;
            pcb_proceso->instrucciones = instrucciones;
            pcb_proceso->estado_anterior = PCB_NEW;
            pcb_proceso->estado_actual = PCB_NEW;
            pcb_proceso->consola_fd = cliente_fd;
            pcb_proceso->debe_ser_finalizado = false;
            pcb_proceso->debe_ser_bloqueado = false;
            pcb_proceso->puede_ser_interrumpido = false;
            pcb_proceso->fue_interrumpido = false;
            pcb_proceso->registro_para_bloqueo = 0;
            pcb_proceso->unidades_de_trabajo = 0;
            pcb_proceso->dispositivo_bloqueo = string_new();
            pcb_proceso->page_fault_segmento = 0;
            pcb_proceso->page_fault_pagina = 0;

            meter_proceso_en_new(pcb_proceso);
            break;
        }

        switch (codigo) {
            case INSTRUCCIONES_Y_SEGMENTOS:
                if(!recv_instrucciones_y_segmentos(logger, cliente_fd, &instrucciones, &segmentos)) {
                    log_error(logger,"Hubo un error al recuperar la lista de instrucciones y segmentos");
                }
                break;
            case -1:
                log_error(logger, "el cliente se desconecto. Terminando servidor");
                return;
            default:
                log_error(logger, "Algo anduvo mal en el server de %s", server_name);
                log_info(logger, "Cop: %d", codigo);
                return;
        }
    }

    log_warning(logger, "El cliente se desconecto de %s server", server_name);
    return;
}

int server_escuchar(t_log* logger, char* server_name, int server_socket) {
    uint32_t cliente_socket = esperar_cliente(logger, server_name, server_socket);

    if (cliente_socket != -1) {
        pthread_t hilo;
        t_procesar_conexion_args* args = malloc(sizeof(t_procesar_conexion_args));
        args->log = logger;
        args->fd = cliente_socket;
        args->server_name = server_name;
        pthread_create(&hilo, NULL, (void*) procesar_conexion, (void*) args);
        pthread_detach(hilo);
        return 1;
    }
    return 0;
}