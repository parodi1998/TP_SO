#include "../include/planificacion.h"

int32_t timer = 0;
bool debe_interrumpir_al_cpu = false;
bool timer_sigue_contando = false;

void meter_proceso_en_new(t_pcb* proceso) {

	pthread_mutex_lock(&mutex_new);
	pthread_mutex_lock(&mutex_dictionary_colas);
	t_queue* cola_new = dictionary_get(colas,"NEW");
	pthread_mutex_unlock(&mutex_dictionary_colas);
	queue_push(cola_new, proceso);
	log_proceso_en_new(logger_kernel_obligatorio, proceso);
	pthread_mutex_unlock(&mutex_new);

	sem_post(&contador_new);
	sem_post(&sem_largo_plazo_new);
}

t_pcb* sacar_proceso_de_new() {
	sem_wait(&contador_new);

	pthread_mutex_lock(&mutex_new);
	pthread_mutex_lock(&mutex_dictionary_colas);
	t_queue* cola_new = dictionary_get(colas,"NEW");
	pthread_mutex_unlock(&mutex_dictionary_colas);
	t_pcb* proceso = queue_pop(cola_new);
	pthread_mutex_unlock(&mutex_new);
	
	return proceso;
}

/**
 * Planificador Largo Plazo
 * */
void hilo_planificador_largo_plazo_new() {

	bool es_apto_para_pasar_a_ready = true;
	
	while(1) {
		
		sem_wait(&sem_largo_plazo_new);
		sem_wait(&sem_grado_multiprogramacion);
		
		t_pcb* proceso = sacar_proceso_de_new();

		es_apto_para_pasar_a_ready = true;

		for(int index = 0; index < list_size(proceso->tabla_segmentos); index++) {

			t_pcb_segmentos* segmento = list_get(proceso->tabla_segmentos, index);
			char* id_tabla_pagina_string = iniciar_segmento_memoria(fd_memoria, &sem_sincro_cargar_segmentos_en_memoria, logger, proceso->id_proceso, index, segmento->tamanio_segmento);
			segmento->id_tabla_paginas = atoi(id_tabla_pagina_string);
			sem_wait(&sem_sincro_cargar_segmentos_en_memoria);
			if(segmento->id_tabla_paginas == -1) {
				log_info(logger, "Error: PID: <%d>, Segmento: <%d>, Pagina: <%d>", proceso->id_proceso, segmento->tamanio_segmento, segmento->id_tabla_paginas);
				es_apto_para_pasar_a_ready = false;
				break;
			}
		}

		if(es_apto_para_pasar_a_ready) {
			meter_proceso_en_ready(proceso);
		} else {
			log_info(logger, "Al menos uno de los ids de las tablas de pagina fue -1, por lo que se procede a finalizar el proceso PID: <%d>", proceso->id_proceso);
			proceso->finaliza_por_error_de_ejecucion = true;
			meter_proceso_en_exit(proceso);
		}
	}
}

void meter_proceso_en_exit(t_pcb* proceso) {
	pthread_mutex_lock(&mutex_exit);
	actualizar_estado_proceso(logger_kernel_obligatorio, proceso, PCB_EXIT);
	pthread_mutex_lock(&mutex_dictionary_colas);
	t_queue* cola_exit = dictionary_get(colas,"EXIT");
	pthread_mutex_unlock(&mutex_dictionary_colas);
	queue_push(cola_exit, proceso);
	pthread_mutex_unlock(&mutex_exit);

	sem_post(&contador_exit);
	sem_post(&sem_largo_plazo_exit);
}

t_pcb* sacar_proceso_de_exit() {
	sem_wait(&contador_exit);			

	pthread_mutex_lock(&mutex_exit);
	pthread_mutex_lock(&mutex_dictionary_colas);
	t_queue* cola_exit = dictionary_get(colas,"EXIT");
	pthread_mutex_unlock(&mutex_dictionary_colas);
	t_pcb* proceso = queue_pop(cola_exit);
	pthread_mutex_unlock(&mutex_exit);
	
	return proceso;
}

void hilo_planificador_largo_plazo_exit() {
	while(1) {
		sem_wait(&sem_largo_plazo_exit);
		
		t_pcb* proceso = sacar_proceso_de_exit();

		char* rta = finalizar_proceso_memoria(fd_memoria, &sem_sincro_finalizar_pcb_en_memoria, logger, proceso->id_proceso);
		sem_wait(&sem_sincro_finalizar_pcb_en_memoria);
		free(rta);

		if(proceso->finaliza_por_segmentation_fault) {
			send_finalizar_consola_error_segmentation_fault_from_kernel(logger, proceso->consola_fd);
		} else if(proceso->finaliza_por_error_instruccion) {
			send_finalizar_consola_error_instruccion_from_kernel(logger, proceso->consola_fd);
		} else if(proceso->finaliza_por_error_de_ejecucion) {
			send_finalizar_consola_error_comunicacion_from_kernel(logger, proceso->consola_fd);
		} else {
			send_finalizar_consola_ok_from_kernel(logger, proceso->consola_fd);
		}

		recv_finalizar_consola_from_consola(logger, proceso->consola_fd);
		sem_wait(&sem_finalizar_proceso);

		liberar_pcb(proceso);
		sem_post(&sem_grado_multiprogramacion);
	}
}


/**
 * Planificador Corto Plazo
 * */

void meter_proceso_en_ready_fifo(t_pcb* proceso) {
	pthread_mutex_lock(&mutex_ready);
	pthread_mutex_lock(&mutex_ready_fifo);
	pthread_mutex_lock(&mutex_dictionary_colas);
	t_queue* cola_ready_fifo = dictionary_get(colas,"READY_FIFO");
	pthread_mutex_unlock(&mutex_dictionary_colas);
	pthread_mutex_lock(&mutex_dictionary_colas);
	t_queue* cola_ready_rr = dictionary_get(colas,"READY_RR");
	pthread_mutex_unlock(&mutex_dictionary_colas);
	actualizar_estado_proceso(logger_kernel_obligatorio, proceso, PCB_READY);
	queue_push(cola_ready_fifo, proceso);
	log_procesos_en_ready(logger_kernel_obligatorio, cola_ready_fifo->elements, cola_ready_rr->elements, config_kernel->algoritmo_planificacion);
	pthread_mutex_unlock(&mutex_ready_fifo);
	pthread_mutex_unlock(&mutex_ready);

	sem_post(&contador_ready_fifo);
	sem_post(&sem_proceso_agregado_a_ready);
}

void meter_proceso_en_ready_rr(t_pcb* proceso) {
	pthread_mutex_lock(&mutex_ready);
	pthread_mutex_lock(&mutex_ready_rr);
	pthread_mutex_lock(&mutex_dictionary_colas);
	t_queue* cola_ready_fifo = dictionary_get(colas,"READY_FIFO");
	pthread_mutex_unlock(&mutex_dictionary_colas);
	pthread_mutex_lock(&mutex_dictionary_colas);
	t_queue* cola_ready_rr = dictionary_get(colas,"READY_RR");
	pthread_mutex_unlock(&mutex_dictionary_colas);
	actualizar_estado_proceso(logger_kernel_obligatorio, proceso, PCB_READY);
	queue_push(cola_ready_rr, proceso);
	log_procesos_en_ready(logger_kernel_obligatorio, cola_ready_fifo->elements, cola_ready_rr->elements, config_kernel->algoritmo_planificacion);
	pthread_mutex_unlock(&mutex_ready_rr);
	pthread_mutex_unlock(&mutex_ready);

	sem_post(&contador_ready_rr);
	sem_post(&sem_proceso_agregado_a_ready);
}

void meter_proceso_en_ready_feedback(t_pcb* proceso) {
	switch(proceso->estado_actual) {
		case PCB_NEW: 
			meter_proceso_en_ready_rr(proceso);
			break;
		case PCB_EXECUTE:
			meter_proceso_en_ready_fifo(proceso);
			break;
		case PCB_BLOCK:
			meter_proceso_en_ready_rr(proceso);
			break;
	}
}

void meter_proceso_en_ready(t_pcb* proceso) {
	if(string_equals_ignore_case(config_kernel->algoritmo_planificacion,"FEEDBACK")) {
		meter_proceso_en_ready_feedback(proceso);
	} else if(string_equals_ignore_case(config_kernel->algoritmo_planificacion,"RR")) {
		meter_proceso_en_ready_rr(proceso);
	} else {
		meter_proceso_en_ready_fifo(proceso);
	}
	sem_wait(&sem_proceso_agregado_a_ready);
	sem_post(&sem_corto_plazo_ready);
}

t_pcb* sacar_proceso_de_ready_fifo() {
	sem_wait(&contador_ready_fifo);			

	pthread_mutex_lock(&mutex_ready);
	pthread_mutex_lock(&mutex_ready_fifo);
	pthread_mutex_lock(&mutex_dictionary_colas);
	t_queue* cola_ready_fifo = dictionary_get(colas,"READY_FIFO");
	pthread_mutex_unlock(&mutex_dictionary_colas);
	t_pcb* proceso = queue_pop(cola_ready_fifo);
	proceso->puede_ser_interrumpido = false;
	pthread_mutex_unlock(&mutex_ready_fifo);
	pthread_mutex_unlock(&mutex_ready);

	sem_post(&sem_proceso_sacado_de_ready);
	return proceso;
}

t_pcb* sacar_proceso_de_ready_rr() {
	sem_wait(&contador_ready_rr);			

	pthread_mutex_lock(&mutex_ready);
	pthread_mutex_lock(&mutex_ready_rr);
	pthread_mutex_lock(&mutex_dictionary_colas);
	t_queue* cola_ready_rr = dictionary_get(colas,"READY_RR");
	pthread_mutex_unlock(&mutex_dictionary_colas);
	t_pcb* proceso = queue_pop(cola_ready_rr);
	proceso->puede_ser_interrumpido = true;
	pthread_mutex_unlock(&mutex_ready_rr);
	pthread_mutex_unlock(&mutex_ready);

	sem_post(&sem_proceso_sacado_de_ready);
	return proceso;
}

t_pcb* sacar_proceso_de_ready_feedback() {
	pthread_mutex_lock(&mutex_dictionary_colas);
	t_queue* cola_ready_rr = dictionary_get(colas,"READY_RR");
	pthread_mutex_unlock(&mutex_dictionary_colas);
	if(!queue_is_empty(cola_ready_rr)) {		// Prioridad 1: RR
		return sacar_proceso_de_ready_rr();
	} else {									// Prioridad 2: FIFO
		return sacar_proceso_de_ready_fifo();
	}
}

t_pcb* sacar_proceso_de_ready() {
	if(string_equals_ignore_case(config_kernel->algoritmo_planificacion,"FEEDBACK")) {
		return sacar_proceso_de_ready_feedback();
	} else if(string_equals_ignore_case(config_kernel->algoritmo_planificacion,"RR")) {
		return sacar_proceso_de_ready_rr();
	} else {
		return sacar_proceso_de_ready_fifo();
	}
}

void hilo_planificador_corto_plazo_ready() {
	while(1) {
		sem_wait(&sem_corto_plazo_ready);
		sem_wait(&sem_cpu_libre);
		t_pcb* proceso = sacar_proceso_de_ready();
		sem_wait(&sem_proceso_sacado_de_ready);
		meter_proceso_en_execute(proceso);
	}
}

void meter_proceso_en_execute(t_pcb* proceso) {
	pthread_mutex_lock(&mutex_execute);
	actualizar_estado_proceso(logger_kernel_obligatorio, proceso, PCB_EXECUTE);
	pthread_mutex_lock(&mutex_dictionary_colas);
	t_queue* cola_execute = dictionary_get(colas,"EXECUTE");
	pthread_mutex_unlock(&mutex_dictionary_colas);
	queue_push(cola_execute, proceso);
	pthread_mutex_unlock(&mutex_execute);
	
	sem_post(&contador_execute);
	sem_post(&sem_corto_plazo_execute);
}

t_pcb* sacar_proceso_de_execute() {
	sem_wait(&contador_execute);

	pthread_mutex_lock(&mutex_execute);
	pthread_mutex_lock(&mutex_dictionary_colas);
	t_queue* cola_execute = dictionary_get(colas,"EXECUTE");
	pthread_mutex_unlock(&mutex_dictionary_colas);
	t_pcb* proceso = queue_pop(cola_execute);
	pthread_mutex_unlock(&mutex_execute);
	sem_post(&sem_sacar_de_execute);
	return proceso;
}

void devolver_proceso_a_ready(t_pcb* proceso) {
	pthread_mutex_lock(&mutex_dictionary_colas);
	t_queue* cola_ready_fifo = dictionary_get(colas,"READY_FIFO");
	pthread_mutex_unlock(&mutex_dictionary_colas);
	pthread_mutex_lock(&mutex_dictionary_colas);
	t_queue* cola_ready_rr = dictionary_get(colas,"READY_RR");
	pthread_mutex_unlock(&mutex_dictionary_colas);
	log_proceso_desalojado_por_quantum(logger_kernel_obligatorio, proceso);
	if(queue_is_empty(cola_ready_fifo) && queue_is_empty(cola_ready_rr)) {
		meter_proceso_en_execute(proceso);
	} else {
		meter_proceso_en_ready(proceso);
		sem_post(&sem_cpu_libre);
	}
}

void hilo_planificador_corto_plazo_execute() {
	
	op_code cod_op;

	while(1) {
		sem_wait(&sem_corto_plazo_execute);

		t_pcb* proceso = sacar_proceso_de_execute();
		sem_wait(&sem_sacar_de_execute);

		if(!send_pcb(logger, fd_cpu_dispatch, proceso)) {
			log_error(logger,"Hubo un error al enviando el proceso de cpu, por lo que se procede a finalizar el mismo.");
			proceso->finaliza_por_error_de_ejecucion = true;
			meter_proceso_en_exit(proceso);
			sem_post(&sem_cpu_libre);
		} else {

			if(proceso->puede_ser_interrumpido) {
				pthread_mutex_lock(&mutex_debe_ser_interrumpido_cpu);
				debe_interrumpir_al_cpu = true;
				pthread_mutex_unlock(&mutex_debe_ser_interrumpido_cpu);
				sem_post(&sem_comienza_timer_quantum);
			}
			
			t_pcb* proceso_recibido;

			recv(fd_cpu_dispatch, &cod_op, sizeof(op_code), MSG_WAITALL);

			if(!recv_pcb(logger, fd_cpu_dispatch, &proceso_recibido) || cod_op != PCB_KERNEL) {
				log_error(logger,"Hubo un error al recibir el proceso de cpu, por lo que se procede a finalizar el mismo.");
				proceso->finaliza_por_error_de_ejecucion = true;
				meter_proceso_en_exit(proceso);
				sem_post(&sem_cpu_libre);
			} else {

				pthread_mutex_lock(&mutex_debe_ser_interrumpido_cpu);
				debe_interrumpir_al_cpu = false;
				pthread_mutex_unlock(&mutex_debe_ser_interrumpido_cpu);

				pthread_mutex_lock(&mutex_timer_quantum);
				timer_sigue_contando = false;
				pthread_mutex_unlock(&mutex_timer_quantum);
				
				liberar_pcb(proceso);

				if(proceso_recibido->debe_ser_finalizado) {
					meter_proceso_en_exit(proceso_recibido);
					sem_post(&sem_cpu_libre);
				} else if(proceso_recibido->debe_ser_bloqueado) {
					if(string_equals_ignore_case(proceso_recibido->dispositivo_bloqueo,"PANTALLA") || string_equals_ignore_case(proceso_recibido->dispositivo_bloqueo,"TECLADO")) {
						meter_proceso_en_block(proceso_recibido, string_from_format("%d-%s",proceso_recibido->id_proceso, proceso_recibido->dispositivo_bloqueo));
					} else {
						meter_proceso_en_block(proceso_recibido, proceso_recibido->dispositivo_bloqueo);
					}
					sem_post(&sem_cpu_libre);
				} else {
					devolver_proceso_a_ready(proceso_recibido);
				}
			}
		}
	}
}

/**
 * Timer de quantum
 */
void hilo_timer_contador_quantum() {
	while(1) {
		sem_wait(&sem_comienza_timer_quantum);

		uint32_t quantum_in_milis = atoi(config_kernel->quantum_RR);
		uint32_t quantum_in_micros = quantum_in_milis*1000;
		
		pthread_mutex_lock(&mutex_timer_quantum);
		timer_sigue_contando = true;
		timer = quantum_in_micros;
		pthread_mutex_unlock(&mutex_timer_quantum);

		while(timer_sigue_contando) {
			usleep(1000);
			pthread_mutex_lock(&mutex_timer_quantum);
			timer -= 1000;
			pthread_mutex_unlock(&mutex_timer_quantum);
		
			if(timer <= 0) {
				pthread_mutex_lock(&mutex_timer_quantum);
				timer_sigue_contando = false; 
				pthread_mutex_unlock(&mutex_timer_quantum);
			}
		}

		if(!timer_sigue_contando && debe_interrumpir_al_cpu) {
			if(!send_interrumpir_cpu_from_kernel(logger, fd_cpu_interrupt)) {
				log_error(logger,"No se envio la interrupcion a cpu correctamente");
			}
		}
	}
}

/**
 * Planificador de bloqueos 
 */
void meter_proceso_en_block(t_pcb* proceso, char* key_cola_de_bloqueo) {
	pthread_mutex_lock(&mutex_dictionary_mutex_colas_block);
	sem_t* mutex_pointer = dictionary_get(mutex_colas_block ,key_cola_de_bloqueo);
	pthread_mutex_unlock(&mutex_dictionary_mutex_colas_block);
	pthread_mutex_lock(&mutex_dictionary_contador_colas_block);
	sem_t* contador_pointer = dictionary_get(contador_colas_block, key_cola_de_bloqueo);
	pthread_mutex_unlock(&mutex_dictionary_contador_colas_block);
	pthread_mutex_lock(&mutex_dictionary_sem_hilos_block);
	sem_t* sem_hilo_pointer = dictionary_get(sem_hilos_block, key_cola_de_bloqueo);	
	pthread_mutex_unlock(&mutex_dictionary_sem_hilos_block);
	sem_wait(mutex_pointer);
	actualizar_estado_proceso(logger_kernel_obligatorio, proceso, PCB_BLOCK);
	pthread_mutex_lock(&mutex_dictionary_colas);
	t_queue* cola_block_dinamica = dictionary_get(colas,key_cola_de_bloqueo);
	pthread_mutex_unlock(&mutex_dictionary_colas);
	queue_push(cola_block_dinamica, proceso);
	log_motivo_de_bloqueo(logger_kernel_obligatorio, proceso, proceso->dispositivo_bloqueo);
	sem_post(mutex_pointer);

	sem_post(contador_pointer);
	sem_post(sem_hilo_pointer);
}

t_pcb* sacar_proceso_de_block(char* key_cola_de_bloqueo) {
	pthread_mutex_lock(&mutex_dictionary_mutex_colas_block);
	sem_t* mutex_pointer = dictionary_get(mutex_colas_block ,key_cola_de_bloqueo);
	pthread_mutex_unlock(&mutex_dictionary_mutex_colas_block);
	pthread_mutex_lock(&mutex_dictionary_contador_colas_block);
	sem_t* contador_pointer = dictionary_get(contador_colas_block, key_cola_de_bloqueo);
	pthread_mutex_unlock(&mutex_dictionary_contador_colas_block);
	sem_wait(contador_pointer);
	sem_wait(mutex_pointer);
	pthread_mutex_lock(&mutex_dictionary_colas);
	t_queue* cola_block_dinamica = dictionary_get(colas,key_cola_de_bloqueo);
	pthread_mutex_unlock(&mutex_dictionary_colas);
	t_pcb* proceso = queue_pop(cola_block_dinamica);
	sem_post(mutex_pointer);

	return proceso;
}

void hilo_planificador_block_io(void* args) {
	char* key_cola_de_bloqueo = args;
	while(1) {
		pthread_mutex_lock(&mutex_dictionary_sem_hilos_block);
		sem_t* sem_hilo_pointer = dictionary_get(sem_hilos_block, key_cola_de_bloqueo);
		pthread_mutex_unlock(&mutex_dictionary_sem_hilos_block);
		sem_wait(sem_hilo_pointer);
		
		t_pcb* proceso = sacar_proceso_de_block(key_cola_de_bloqueo);

		pthread_mutex_lock(&mutex_dictionary_tiempos_io);
		char* tiempo_io_string = dictionary_get(tiempos_io, key_cola_de_bloqueo);
		pthread_mutex_unlock(&mutex_dictionary_tiempos_io);
		uint32_t tiempo_io_en_milis = atoi(tiempo_io_string) * proceso->unidades_de_trabajo;
		
		usleep(tiempo_io_en_milis*1000);

		proceso->debe_ser_bloqueado = false;

		meter_proceso_en_ready(proceso);
	}
}

void hilo_planificador_block_pantalla(void* args) {
	char* key_cola_de_bloqueo = args;
	while(1) {
		pthread_mutex_lock(&mutex_dictionary_sem_hilos_block);
		sem_t* sem_hilo_pointer = dictionary_get(sem_hilos_block, key_cola_de_bloqueo);
		pthread_mutex_unlock(&mutex_dictionary_sem_hilos_block);
		sem_wait(sem_hilo_pointer);
		
		t_pcb* proceso = sacar_proceso_de_block(key_cola_de_bloqueo);

		uint32_t valor_registro = -1; // Error

		switch(proceso->registro_para_bloqueo) {
			case 0:
				valor_registro = proceso->registro_AX;
				break;
			case 1:
				valor_registro = proceso->registro_BX;
				break;
			case 2:
				valor_registro = proceso->registro_CX;
				break;
			case 3:
				valor_registro = proceso->registro_DX;
				break;
		}

		if(valor_registro != -1) {
			send_mostrar_dato_en_pantalla_from_kernel(logger, proceso->consola_fd, string_itoa(valor_registro));
			sem_wait(&sem_fin_io_pantalla);
		} else {
			log_error(logger, "Hubo un error al leer el registro para imprimir en pantalla");
			log_error(logger, "registro_para_bloqueo: %d", proceso->registro_para_bloqueo);
			// Deberiamos finalizar el proceso?
		}
		
		proceso->debe_ser_bloqueado = false;

		meter_proceso_en_ready(proceso);
	}
}

void hilo_planificador_block_teclado(void* args) {
	char* key_cola_de_bloqueo = args;
	while(1) {
		pthread_mutex_lock(&mutex_dictionary_sem_hilos_block);
		sem_t* sem_hilo_pointer = dictionary_get(sem_hilos_block, key_cola_de_bloqueo);
		pthread_mutex_unlock(&mutex_dictionary_sem_hilos_block);
		sem_wait(sem_hilo_pointer);
		
		t_pcb* proceso = sacar_proceso_de_block(key_cola_de_bloqueo);

		if(!send_ingresar_dato_por_teclado_from_kernel(logger, proceso->consola_fd)) {
			log_error(logger, "Hubo un error al pedir que se ingrese un dato por teclado a consola: %d",proceso->consola_fd);
			// Deberiamos finalizar el proceso?
		}

		sem_wait(&sem_dato_por_teclado_ingresado);
		
		pthread_mutex_lock(&mutex_dictionary_dato_ingreso_por_teclado);
		char* dato = dictionary_get(dato_ingreso_por_teclado, string_itoa(proceso->id_proceso));
		pthread_mutex_unlock(&mutex_dictionary_dato_ingreso_por_teclado);
		uint32_t dato_a_guardar = atoi(dato);

		switch(proceso->registro_para_bloqueo) {
			case 0:
				proceso->registro_AX = dato_a_guardar;
				break;
			case 1:
				proceso->registro_BX = dato_a_guardar;
				break;
			case 2:
				proceso->registro_CX = dato_a_guardar;
				break;
			case 3:
				proceso->registro_DX = dato_a_guardar;
				break;
		}
		
		proceso->debe_ser_bloqueado = false;

		meter_proceso_en_ready(proceso);
	}
}

void hilo_planificador_block_page_fault(void* args) {
	char* key_cola_de_bloqueo = "PAGE_FAULT";
	while(1) {
		pthread_mutex_lock(&mutex_dictionary_sem_hilos_block);
		sem_t* sem_hilo_pointer = dictionary_get(sem_hilos_block, key_cola_de_bloqueo);
		pthread_mutex_unlock(&mutex_dictionary_sem_hilos_block);
		sem_wait(sem_hilo_pointer);
		
		t_pcb* proceso = sacar_proceso_de_block(key_cola_de_bloqueo);

		page_fault_memoria(fd_memoria, logger, proceso->id_proceso, proceso->page_fault_segmento, proceso->page_fault_pagina);

		proceso->debe_ser_bloqueado = false;
		meter_proceso_en_ready(proceso);
	}
}