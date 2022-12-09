#include "../include/kernel_logs.h"

/**
 * Funciones utiles
 */

static char* convertir_estado_pcb_a_string(t_estado_pcb estado) {
    char* estado_string = string_new();
    switch(estado) {
        case PCB_NEW:
            string_append(&estado_string, "NEW");
            break;
        case PCB_READY:
            string_append(&estado_string, "READY");
            break;
        case PCB_EXECUTE:
            string_append(&estado_string, "EXECUTE");
            break;
        case PCB_BLOCK:
            string_append(&estado_string, "BLOCK");
            break;
        case PCB_EXIT:
            string_append(&estado_string, "EXIT");
            break;     
        default:
            string_append(&estado_string, "UNKNOWN");
            break;  
    }
    return estado_string;
}

static char* lista_de_pids(t_list* procesos) {
    
    void* pid_as_string(void* proceso) {
        t_pcb* p = proceso;
        return (void*) string_itoa(p->id_proceso);
    }

    t_list* pids_list = list_map(procesos,pid_as_string);
    char* pids_string = string_new();

    for(uint32_t index = 0; index < list_size(pids_list); index++) {
            char* pid = list_get(pids_list,index);
            if(index == list_size(pids_list) - 1) {
                string_append_with_format(&pids_string, "%s",pid);
            } else {
                string_append_with_format(&pids_string, "%s,",pid);
            }   
        }

    list_destroy_and_destroy_elements(pids_list, free);
    return pids_string;
}

/**
 * Logs obligatiorios
 */

void log_proceso_en_new(t_log* logger, t_pcb* proceso) {
    log_info(logger, "Se crea el proceso <%d> en NEW", proceso->id_proceso);
}

void log_procesos_en_ready(t_log* logger, t_list* procesos_fifo, t_list* procesos_rr, char* algoritmo) {
    char* pids_rr = lista_de_pids(procesos_rr);
    char* pids_fifo = lista_de_pids(procesos_fifo);
    if(string_equals_ignore_case(algoritmo,"FEEDBACK")) {
		log_info(logger, "Cola Ready <%s>: [%s] [%s]", algoritmo, pids_rr, pids_fifo);
	} else if(string_equals_ignore_case(algoritmo,"RR")) {
        log_info(logger, "Cola Ready <%s>: [%s]", algoritmo, pids_rr);
	} else {
        log_info(logger, "Cola Ready <%s>: [%s]", algoritmo, pids_fifo);
    }
    free(pids_fifo);
    free(pids_rr);
}

void log_proceso_cambio_de_estado(t_log* logger, t_pcb* proceso) {
    int id = proceso->id_proceso;
    char* estado_anterior = convertir_estado_pcb_a_string(proceso->estado_anterior);
    char* estado_actual = convertir_estado_pcb_a_string(proceso->estado_actual);
    log_info(logger, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <%s>", id, estado_anterior, estado_actual);
    free(estado_anterior);
    free(estado_actual);
}

void actualizar_estado_proceso(t_log* logger, t_pcb* proceso, t_estado_pcb nuevo_estado) {
	t_estado_pcb estado_aux;
	estado_aux = proceso->estado_actual;
	proceso->estado_actual = nuevo_estado;
	proceso->estado_anterior = estado_aux;
	log_proceso_cambio_de_estado(logger, proceso);
}

void log_motivo_de_bloqueo(t_log* logger, t_pcb* proceso, char* dispositivo) {
    uint32_t segmento = proceso->page_fault_segmento;
    uint32_t pagina = proceso->page_fault_pagina;
    if(string_equals_ignore_case(dispositivo,"PAGE_FAULT")) {
        log_info(logger, "Page Fault PID: <%d> - Segmento: <%d> - Pagina: <%d>", proceso->id_proceso, segmento, pagina);
    } else {
        log_info(logger, "PID: <%d> - Bloqueado por: <%s>", proceso->id_proceso, dispositivo);
    }
}

void log_proceso_desalojado_por_quantum(t_log* logger, t_pcb* proceso) {
    log_info(logger, "PID: <%d> - Desalojado por fin de Quantum", proceso->id_proceso);
}