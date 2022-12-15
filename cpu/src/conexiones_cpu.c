#include "../include/conexiones_cpu.h"
#include "../../shared/include/protocolo.h"


int kernel = 1;
int interrumpir = 0;
sem_t* comunicacion_kernel;
int alfa = 0;

pthread_t hilo_dispatch;
pthread_t hilo_interrupt;

char * concatenar(char* palabra1, char* palabra2)
{
	 char *palabraFinal = string_new();
	string_append(&palabraFinal, palabra1);
	string_append(&palabraFinal, palabra2);
	return palabraFinal;
}

t_contexto_ejecucion iniciar_proceso(t_pcb* pcb1){
	
	t_contexto_ejecucion contexto_ejecucion;
	contexto_ejecucion.estado = OPTIMO;
	contexto_ejecucion.program_counter = pcb1->program_counter;
	contexto_ejecucion.reg_general.ax = pcb1->registro_AX;
	contexto_ejecucion.reg_general.bx = pcb1->registro_BX;
	contexto_ejecucion.reg_general.cx = pcb1->registro_CX;
	contexto_ejecucion.reg_general.dx = pcb1->registro_DX;
	contexto_ejecucion.tabla_segmentos = list_create();

	for(int index = 0; index < list_size(pcb1->tabla_segmentos); index++) {
		t_pcb_segmentos* segmento = list_get(pcb1->tabla_segmentos, index);
		t_segmento* segmento_cpu = malloc(sizeof(t_segmento));
		segmento_cpu->nro = index;
		segmento_cpu->tam = segmento->tamanio_segmento;
		segmento_cpu->nro_indice_tabla_paginas = segmento->id_tabla_paginas;
		list_add(contexto_ejecucion.tabla_segmentos, segmento_cpu);
	}

	seguir_instrucciones(&contexto_ejecucion, pcb1->instrucciones, pcb1->id_proceso);

	return contexto_ejecucion;
}

static uint32_t registro_char_to_int(char* registro) {
	if(string_equals_ignore_case(registro,"AX")) {
		return 0;
	} else if(string_equals_ignore_case(registro,"BX")) {
		return 1;
	} else if(string_equals_ignore_case(registro,"CX")) {
		return 2;
	} else {
		return 3;
	}
}

static void actualizar_pcb(t_contexto_ejecucion contexto, t_pcb* proceso) {
	proceso->program_counter = contexto.program_counter;
	proceso->registro_AX = contexto.reg_general.ax;
	proceso->registro_BX = contexto.reg_general.bx;
	proceso->registro_CX = contexto.reg_general.cx;
	proceso->registro_DX = contexto.reg_general.dx;
	switch (contexto.estado) {
			case BLOQUEO:
				proceso->debe_ser_bloqueado = true;
				proceso->dispositivo_bloqueo = contexto.io_dispositivo;
				proceso->unidades_de_trabajo = contexto.io_unidades;
				if(contexto.io_registro != NULL) {
					proceso->registro_para_bloqueo = registro_char_to_int(contexto.io_registro);
				}
                break;
			case INTERRUPCION:
				proceso->fue_interrumpido = true;
                break;
			case PAGE_DEFAULT:
				log_info(get_log(),"Antes de devolver pcb por page fault - SEGMENTO: %d - PAGINA : %d",segmento_page_fault,pagina_page_fault);
				proceso->debe_ser_bloqueado = true;
				proceso->page_fault_segmento = segmento_page_fault;
				proceso->page_fault_pagina = pagina_page_fault;
				proceso->dispositivo_bloqueo = "PAGE_FAULT";
                break;
			case FINALIZADO:
				proceso->debe_ser_finalizado = true;
                break;
			case ERROR_SEGMENTATION_FAULT:
				proceso->debe_ser_finalizado = true;
				proceso->finaliza_por_segmentation_fault = true;
				break;
			case ERROR_INSTRUCCION:
				proceso->debe_ser_finalizado = true;
				proceso->finaliza_por_error_instruccion = true;
				break;
			default:
				break; 
		}
}

void comunicacion_cpu_kernel_distpach() {
	t_pcb* proceso;
	op_code cod_op;
	t_list* instrucciones;
    t_list* segmentos;
	t_contexto_ejecucion contexto;

	while(fd_client_kernel_dispatch != -1) {
		

		if(recv(fd_client_kernel_dispatch, &cod_op, sizeof(op_code), MSG_WAITALL) == -1)
			cod_op = -1;

		switch (cod_op) {
			case PCB_KERNEL:
				if(!recv_pcb(get_log(), fd_client_kernel_dispatch, &proceso)) {
                    log_error(get_log(),"Hubo un error al recibir el proceso de kernel");
                } else {
					log_info(get_log(),"Se recibio el proceso de kernel con exito");
					contexto = iniciar_proceso(proceso);
					actualizar_pcb(contexto, proceso);
					if(!send_pcb(get_log(), fd_client_kernel_dispatch, proceso)) {
						log_error(get_log(),"Hubo un error enviando el proceso al kernel");
					} else {
						log_info(get_log(),"El proceso fue enviado a kernel");
					}
					liberar_pcb(proceso);
				}
                break;
			default:
				break; 
		}
	}
}

void comunicacion_cpu_kernel_interrupt() {
	op_code cod_op;
	while(fd_client_kernel_interrupt != -1) {
		
		if(recv(fd_client_kernel_interrupt, &cod_op, sizeof(op_code), MSG_WAITALL) == -1)
			cod_op = -1;

		switch (cod_op) {
			case PCB_INTERRUPT:
				interrumpe();
                break;
			default:
				break; 
		}
	}
 }

int start()
{
	pthread_create(&hilo_interrupt, NULL, (void*) comunicacion_cpu_kernel_interrupt, NULL);
    pthread_detach(hilo_interrupt);

	pthread_create(&hilo_dispatch, NULL, (void*) comunicacion_cpu_kernel_distpach, NULL);
    pthread_join(hilo_dispatch, NULL);

	//ciclo_recibir_instruccines();
	//return alfa;
}
