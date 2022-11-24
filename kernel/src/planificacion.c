#include "../include/planificacion.h"

void meter_proceso_en_new(t_pcb* proceso) {

	pthread_mutex_lock(&mutex_new);
	t_queue* cola_new = dictionary_get(colas,"NEW");
	queue_push(cola_new, proceso);
	log_proceso_en_new(logger_kernel_obligatorio, proceso);
	pthread_mutex_unlock(&mutex_new);

	sem_post(&contador_new);
	sem_post(&sem_largo_plazo_new);
}

t_pcb* sacar_proceso_de_new() {
	sem_wait(&contador_new);			// si por casualidad esto se llama y no hay nada en new (no deberia pasar nunca) se bloquea

	pthread_mutex_lock(&mutex_new);
	t_queue* cola_new = dictionary_get(colas,"NEW");
	t_pcb* proceso = queue_pop(cola_new);
	pthread_mutex_unlock(&mutex_new);
	
	return proceso;
}

/**
 * Planificador Largo Plazo
 * */
void hilo_planificador_largo_plazo_new() {
	
	while(1) {
		
		sem_wait(&sem_largo_plazo_new);					// espera que le avisen que puede hacer algo
		sem_wait(&sem_grado_multiprogramacion);
		
		t_pcb* proceso = sacar_proceso_de_new();
		
		// wait(mutex_comunicacion_kernel_memoria)
		// send_proceso_a_memoria(proceso)				// memoria inicializa sus estructuras necesarias
		// proceso = esperar_proceso_de_memoria()		// obtenemos el indice de la tabla de paginas de cada segmento
		// signal(mutex_comunicacion_kernel_memoria)

		// pthread_mutex_lock(&mutex_analizando_interrupcion);
		// pthread_mutex_unlock(&mutex_analizando_interrupcion);
		// pthread_mutex_lock(&mutex_analizando_fin_de_bloqueo);
		// pthread_mutex_unlock(&mutex_analizando_fin_de_bloqueo);
		meter_proceso_en_ready(proceso);
	}

}

void meter_proceso_en_exit(t_pcb* proceso) {
	pthread_mutex_lock(&mutex_exit);
	actualizar_estado_proceso(logger_kernel_obligatorio, proceso, PCB_EXIT);
	t_queue* cola_exit = dictionary_get(colas,"EXIT");
	queue_push(cola_exit, proceso);
	pthread_mutex_unlock(&mutex_exit);

	sem_post(&contador_exit);
	sem_post(&sem_largo_plazo_exit);
}

t_pcb* sacar_proceso_de_exit() {
	sem_wait(&contador_exit);			

	pthread_mutex_lock(&mutex_exit);
	t_queue* cola_exit = dictionary_get(colas,"EXIT");
	t_pcb* proceso = queue_pop(cola_exit);
	pthread_mutex_unlock(&mutex_exit);
	
	return proceso;
}

void hilo_planificador_largo_plazo_exit() {
	while(1) {
		sem_wait(&sem_largo_plazo_exit);
		
		t_pcb* proceso = sacar_proceso_de_exit();

		// wait_memoria_liberar(proceso);
		// wait_consola_finalizar(proceso);
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
	t_queue* cola_ready_fifo = dictionary_get(colas,"READY_FIFO");
	t_queue* cola_ready_rr = dictionary_get(colas,"READY_RR");
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
	t_queue* cola_ready_fifo = dictionary_get(colas,"READY_FIFO");
	t_queue* cola_ready_rr = dictionary_get(colas,"READY_RR");
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
	// pthread_mutex_lock(&mutex_agregando_proceso_a_ready);
	if(string_equals_ignore_case(config_kernel->algoritmo_planificacion,"FEEDBACK")) {
		meter_proceso_en_ready_feedback(proceso);
	} else if(string_equals_ignore_case(config_kernel->algoritmo_planificacion,"RR")) {
		meter_proceso_en_ready_rr(proceso);
	} else {
		meter_proceso_en_ready_fifo(proceso);
	}
	sem_wait(&sem_proceso_agregado_a_ready);
	// pthread_mutex_unlock(&mutex_agregando_proceso_a_ready);
	sem_post(&sem_corto_plazo_ready);
}

t_pcb* sacar_proceso_de_ready_fifo() {
	sem_wait(&contador_ready_fifo);			

	pthread_mutex_lock(&mutex_ready);
	pthread_mutex_lock(&mutex_ready_fifo);
	t_queue* cola_ready_fifo = dictionary_get(colas,"READY_FIFO");
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
	t_queue* cola_ready_rr = dictionary_get(colas,"READY_RR");
	t_pcb* proceso = queue_pop(cola_ready_rr);
	proceso->puede_ser_interrumpido = true;
	pthread_mutex_unlock(&mutex_ready_rr);
	pthread_mutex_unlock(&mutex_ready);

	sem_post(&sem_proceso_sacado_de_ready);
	return proceso;
}

t_pcb* sacar_proceso_de_ready_feedback() {
	t_queue* cola_ready_rr = dictionary_get(colas,"READY_RR");
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
	t_queue* cola_execute = dictionary_get(colas,"EXECUTE");
	queue_push(cola_execute, proceso);
	pthread_mutex_unlock(&mutex_execute);
	
	sem_post(&contador_execute);
	sem_post(&sem_corto_plazo_execute);
}

t_pcb* sacar_proceso_de_execute() {
	sem_wait(&contador_execute);

	pthread_mutex_lock(&mutex_execute);
	t_queue* cola_execute = dictionary_get(colas,"EXECUTE");
	t_pcb* proceso = queue_pop(cola_execute);
	pthread_mutex_unlock(&mutex_execute);
	
	return proceso;
}

void devolver_proceso_a_ready(t_pcb* proceso) {
	// pthread_mutex_lock(&mutex_analizando_interrupcion);

	t_queue* cola_ready_fifo = dictionary_get(colas,"READY_FIFO");
	t_queue* cola_ready_rr = dictionary_get(colas,"READY_RR");

	if(queue_is_empty(cola_ready_fifo) && queue_is_empty(cola_ready_rr)) {
		meter_proceso_en_execute(proceso);
	} else {
		meter_proceso_en_ready(proceso);
		sem_post(&sem_cpu_libre);
	}
	// pthread_mutex_lock(&mutex_analizando_interrupcion);
}

void hilo_planificador_corto_plazo_execute() {
	while(1) {
		sem_wait(&sem_corto_plazo_execute);

		t_pcb* proceso = sacar_proceso_de_execute();

		// send_proceso_a_cpu(proceso);				// recordar agregar mutex en las comunicaciones si es necesario
		/*
		if(proceso->puede_ser_interrumpido) {
			sem_post(&sem_comienza_timer_quantum);
		}
		*/

		// Para el testo
		// proceso = recv_proceso_de_cpu()			// esto deberia ser bloqueante
		sem_post(&sem_comienza_timer_quantum);
		sem_wait(&sem_finaliza_timer_quantum);		// creo que el sem finaliza no es necesario, aca lo use para sincro no mas

		if(proceso->debe_ser_finalizado) {
			meter_proceso_en_exit(proceso);
			sem_post(&sem_cpu_libre);
		} else if(proceso->debe_ser_bloqueado) {
			meter_proceso_en_block(proceso, proceso->dispositivo_bloqueo);
			sem_post(&sem_cpu_libre);
		} else {
			devolver_proceso_a_ready(proceso);
		}
	}
}

/**
 * Timer de quantum
 */
void hilo_timer_contador_quantum() {
	while(1) {
		sem_wait(&sem_comienza_timer_quantum);

		float quantum_in_seconds = atoi(config_kernel->quantum_RR) / 1000;
		
		sleep(quantum_in_seconds);

		// enviar_interrupcion_a_cpu();

		//imagino que esto podria estar en un hilo que se encargue solo de enviar la señal interrupt a cpu
		sem_post(&sem_finaliza_timer_quantum);
	}
}

/**
 * Planificador de bloqueos 
 */
void meter_proceso_en_block(t_pcb* proceso, char* key_cola_de_bloqueo) {
	sem_t* mutex_pointer = dictionary_get(mutex_colas_block ,key_cola_de_bloqueo);
	sem_t* contador_pointer = dictionary_get(contador_colas_block, key_cola_de_bloqueo);
	sem_t* sem_hilo_pointer = dictionary_get(sem_hilos_block, key_cola_de_bloqueo);
	sem_wait(mutex_pointer);
	actualizar_estado_proceso(logger_kernel_obligatorio, proceso, PCB_BLOCK);
	t_queue* cola_block_dinamica = dictionary_get(colas,key_cola_de_bloqueo);
	queue_push(cola_block_dinamica, proceso);
	log_motivo_de_bloqueo(logger_kernel_obligatorio, proceso, key_cola_de_bloqueo);
	sem_post(mutex_pointer);

	sem_post(contador_pointer);
	sem_post(sem_hilo_pointer);
}

t_pcb* sacar_proceso_de_block(char* key_cola_de_bloqueo) {
	sem_t* mutex_pointer = dictionary_get(mutex_colas_block ,key_cola_de_bloqueo);
	sem_t* contador_pointer = dictionary_get(contador_colas_block, key_cola_de_bloqueo);
	sem_wait(contador_pointer);
	sem_wait(mutex_pointer);
	t_queue* cola_block_dinamica = dictionary_get(colas,key_cola_de_bloqueo);
	t_pcb* proceso = queue_pop(cola_block_dinamica);
	sem_post(mutex_pointer);

	return proceso;
}

void hilo_planificador_block_io(void* args) {
	char* key_cola_de_bloqueo = args;
	while(1) {
		sem_t* sem_hilo_pointer = dictionary_get(sem_hilos_block, key_cola_de_bloqueo);
		sem_wait(sem_hilo_pointer);
		
		t_pcb* proceso = sacar_proceso_de_block(key_cola_de_bloqueo);

		if(string_equals_ignore_case(proceso->dispositivo_bloqueo,"DISCO")) {
			proceso->dispositivo_bloqueo = "IMPRESORA";
		} else {
			proceso->debe_ser_finalizado = true;
        	proceso->debe_ser_bloqueado = false;
		}

		float quantum_in_seconds = atoi(config_kernel->quantum_RR) / 1000;
		
		sleep(quantum_in_seconds);

		meter_proceso_en_ready(proceso);
	}
}

void hilo_planificador_block_pantalla() {
	char* key_cola_de_bloqueo = "PANTALLA";
	while(1) {
		sem_t* sem_hilo_pointer = dictionary_get(sem_hilos_block, key_cola_de_bloqueo);
		sem_wait(sem_hilo_pointer);
		
		t_pcb* proceso = sacar_proceso_de_block(key_cola_de_bloqueo);
		//proceso->debe_ser_bloqueado = false;

		// enviar_mensaje_a_consola();
		// esperar_respuesta_de_consola();

		// pthread_mutex_lock(&mutex_analizando_fin_de_bloqueo);
		// pthread_mutex_lock(&mutex_analizando_interrupcion);
		// pthread_mutex_unlock(&mutex_analizando_interrupcion);
		// meter_proceso_en_block(proceso, "BLOCK");
		// pthread_mutex_unlock(&mutex_analizando_fin_de_bloqueo);
        proceso->debe_ser_bloqueado = true;
		proceso->dispositivo_bloqueo = "DISCO";

		float quantum_in_seconds = atoi(config_kernel->quantum_RR) / 1000;
		
		sleep(quantum_in_seconds);

		meter_proceso_en_ready(proceso);
	}
}

void hilo_planificador_block_teclado(void* args) {
	char* key_cola_de_bloqueo = "TECLADO";
	while(1) {
		sem_t* sem_hilo_pointer = dictionary_get(sem_hilos_block, key_cola_de_bloqueo);
		sem_wait(sem_hilo_pointer);
		
		t_pcb* proceso = sacar_proceso_de_block(key_cola_de_bloqueo);
		proceso->debe_ser_bloqueado = false;

		// enviar_mensaje_a_consola();
		// esperar_respuesta_de_consola();

		// pthread_mutex_lock(&mutex_analizando_fin_de_bloqueo);
		// pthread_mutex_lock(&mutex_analizando_interrupcion);
		// pthread_mutex_unlock(&mutex_analizando_interrupcion);
		// meter_proceso_en_block(proceso, "BLOCK");
		// pthread_mutex_unlock(&mutex_analizando_fin_de_bloqueo);
		proceso->debe_ser_bloqueado = true;
		proceso->dispositivo_bloqueo = "PAGE_FAULT";

		float quantum_in_seconds = atoi(config_kernel->quantum_RR) / 1000;
		
		sleep(quantum_in_seconds);

		meter_proceso_en_ready(proceso);
	}
}

void hilo_planificador_block_page_fault(void* args) {
	char* key_cola_de_bloqueo = "PAGE_FAULT";
	while(1) {
		sem_t* sem_hilo_pointer = dictionary_get(sem_hilos_block, key_cola_de_bloqueo);
		sem_wait(sem_hilo_pointer);
		
		t_pcb* proceso = sacar_proceso_de_block(key_cola_de_bloqueo);
		proceso->debe_ser_bloqueado = false;

		// enviar_mensaje_a_consola();
		// esperar_respuesta_de_consola();

		// pthread_mutex_lock(&mutex_analizando_fin_de_bloqueo);
		// pthread_mutex_lock(&mutex_analizando_interrupcion);
		// pthread_mutex_unlock(&mutex_analizando_interrupcion);
		// meter_proceso_en_block(proceso, "BLOCK");
		// pthread_mutex_unlock(&mutex_analizando_fin_de_bloqueo);
		proceso->debe_ser_bloqueado = true;
		proceso->dispositivo_bloqueo = "IMPRESORA";

		float quantum_in_seconds = atoi(config_kernel->quantum_RR) / 1000;
		
		sleep(quantum_in_seconds);

		meter_proceso_en_ready(proceso);
	}
}