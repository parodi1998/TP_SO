#include "../include/planificacion.h"

void meter_proceso_en_new(t_pcb* proceso) {

	pthread_mutex_lock(&mutex_new);
	queue_push(cola_new, proceso);
	log_proceso_en_new(logger, proceso);
	pthread_mutex_unlock(&mutex_new);

	sem_post(&contador_new);
	sem_post(&sem_largo_plazo_new);
}

t_pcb* sacar_proceso_de_new() {
	sem_wait(&contador_new);			// si por casualidad esto se llama y no hay nada en new (no deberia pasar nunca) se bloquea

	pthread_mutex_lock(&mutex_new);
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
		//wait(multiprogramacion)
		
		t_pcb* proceso = sacar_proceso_de_new();
		
		// wait(mutex_comunicacion_kernel_memoria)
		// send_proceso_a_memoria(proceso)				// memoria inicializa sus estructuras necesarias
		// proceso = esperar_proceso_de_memoria()		// obtenemos el indice de la tabla de paginas de cada segmento
		// signal(mutex_comunicacion_kernel_memoria)

		meter_proceso_en_ready(proceso);
	}

}

void meter_proceso_en_exit(t_pcb* proceso) {
	pthread_mutex_lock(&mutex_exit);
	actualizar_estado_proceso(logger, proceso, PCB_EXIT);
	queue_push(cola_exit, proceso);
	pthread_mutex_unlock(&mutex_exit);

	sem_post(&contador_exit);
	sem_post(&sem_largo_plazo_exit);
}

t_pcb* sacar_proceso_de_exit() {
	sem_wait(&contador_exit);			

	pthread_mutex_lock(&mutex_exit);
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
	}
}


/**
 * Planificador Corto Plazo
 * */

void meter_proceso_en_ready_fifo(t_pcb* proceso) {
	pthread_mutex_lock(&mutex_ready);
	pthread_mutex_lock(&mutex_ready_fifo);
	actualizar_estado_proceso(logger, proceso, PCB_READY);
	queue_push(cola_ready_fifo, proceso);
	log_procesos_en_ready(logger, cola_ready_fifo->elements, cola_ready_rr->elements, config_kernel->algoritmo_planificacion);
	pthread_mutex_unlock(&mutex_ready_fifo);
	pthread_mutex_unlock(&mutex_ready);

	sem_post(&contador_ready_fifo);
	sem_post(&sem_proceso_agregado_a_ready);
}

void meter_proceso_en_ready_rr(t_pcb* proceso) {
	pthread_mutex_lock(&mutex_ready);
	pthread_mutex_lock(&mutex_ready_rr);
	actualizar_estado_proceso(logger, proceso, PCB_READY);
	queue_push(cola_ready_rr, proceso);
	log_procesos_en_ready(logger, cola_ready_fifo->elements, cola_ready_rr->elements, config_kernel->algoritmo_planificacion);
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
	t_pcb* proceso = queue_pop(cola_ready_fifo);
	pthread_mutex_unlock(&mutex_ready_fifo);
	pthread_mutex_unlock(&mutex_ready);

	sem_post(&sem_proceso_sacado_de_ready);
	return proceso;
}

t_pcb* sacar_proceso_de_ready_rr() {
	sem_wait(&contador_ready_rr);			

	pthread_mutex_lock(&mutex_ready);
	pthread_mutex_lock(&mutex_ready_rr);
	t_pcb* proceso = queue_pop(cola_ready_rr);
	pthread_mutex_unlock(&mutex_ready_rr);
	pthread_mutex_unlock(&mutex_ready);

	sem_post(&sem_proceso_sacado_de_ready);
	return proceso;
}

t_pcb* sacar_proceso_de_ready_feedback() {
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
		sem_wait(&sem_cpu_libre);
		sem_wait(&sem_corto_plazo_ready);
		t_pcb* proceso = sacar_proceso_de_ready();
		sem_wait(&sem_proceso_sacado_de_ready);
		meter_proceso_en_execute(proceso);
	}
}

void meter_proceso_en_execute(t_pcb* proceso) {
	pthread_mutex_lock(&mutex_execute);
	actualizar_estado_proceso(logger, proceso, PCB_EXECUTE);
	queue_push(cola_execute, proceso);
	pthread_mutex_unlock(&mutex_execute);
	
	sem_post(&contador_execute);
	sem_post(&sem_corto_plazo_execute);
}

t_pcb* sacar_proceso_de_execute() {
	sem_wait(&contador_execute);			

	pthread_mutex_lock(&mutex_execute);
	t_pcb* proceso = queue_pop(cola_execute);
	pthread_mutex_unlock(&mutex_execute);
	
	return proceso;
}

void hilo_planificador_corto_plazo_execute() {
	while(1) {
		sem_wait(&sem_corto_plazo_execute);

		t_pcb* proceso = sacar_proceso_de_execute();

		// send_proceso_a_cpu(proceso);				// recordar agregar mutex en las comunicaciones si es necesario
		/*
		if(tengo_que_iniciar_timer()) {
			sem_post(&sem_comienza_timer_quantum);
		}
		*/

		// proceso = recv_proceso_de_cpu()			// esto deberia ser bloqueante
		sem_post(&sem_comienza_timer_quantum);
		sem_wait(&sem_finaliza_timer_quantum);		// creo que el sem finaliza no es necesario, aca lo use para sincro no mas

		meter_proceso_en_exit(proceso);

		sem_post(&sem_cpu_libre);
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