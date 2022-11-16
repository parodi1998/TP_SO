#include "../include/planificacion.h"

static t_tipo_planificador convertir_string_a_tipo_planificador(char* planificador_string) {
	
	t_tipo_planificador planificador;
	
	if(string_equals_ignore_case(planificador_string,"RR")) {
		planificador = PLANIFICADOR_RR;
	} else if(string_equals_ignore_case(planificador_string,"FEEDBACK")) {
		planificador = PLANIFICADOR_FEEDBACK;
	} else {
		planificador = PLANIFICADOR_FIFO;
	}
    
    return planificador;
}

static uint32_t calcular_indice_siguiente_proceso_fifo(t_list* lista) {
	return 0;
}

static uint32_t calcular_indice_siguiente_proceso_rr(t_list* lista) {
	return 0;
}

static uint32_t calcular_indice_siguiente_proceso_feedback(t_list* lista) {
	return 0;
}

static uint32_t indice_siguiente_proceso_segun(t_list* lista, char* planificador_string) {
	uint32_t indice;
	t_tipo_planificador planificador = convertir_string_a_tipo_planificador(planificador_string);
	switch(planificador){
		PLANIFICADOR_FIFO:
			indice = calcular_indice_siguiente_proceso_fifo(lista);
			break;
		PLANIFICADOR_RR:
			indice = calcular_indice_siguiente_proceso_rr(lista);
			break;
		PLANIFICADOR_FEEDBACK:
			indice = calcular_indice_siguiente_proceso_feedback(lista);
			break;
	}
	return indice;
}

void meter_proceso_en_new(t_pcb* proceso) {

	pthread_mutex_lock(&mutex_new);
	list_add(cola_new, proceso);
	log_proceso_en_new(logger, proceso);
	pthread_mutex_unlock(&mutex_new);

	sem_post(&contador_new);
	sem_post(&sem_largo_plazo_new);
}

t_pcb* sacar_proceso_de_new(uint32_t index_proceso) {
	sem_wait(&contador_new);			// si por casualidad esto se llama y no hay nada en new (no deberia pasar nunca) se bloquea

	pthread_mutex_lock(&mutex_new);
	t_pcb* proceso = list_remove(cola_new, index_proceso);
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
		char* planificador = "FIFO";
		uint32_t indice = indice_siguiente_proceso_segun(cola_new, planificador);
		
		t_pcb* proceso = sacar_proceso_de_new(indice);
		
		//wait(mutex_comunicacion_kernel_memoria)
		//send_proceso_a_memoria(proceso)				// memoria inicializa sus estructuras necesarias
		//proceso = esperar_proceso_de_memoria()		// obtenemos el indice de la tabla de paginas de cada segmento
		//signal(mutex_comunicacion_kernel_memoria)

		meter_proceso_en_ready(proceso);
	}

}

void meter_proceso_en_exit(t_pcb* proceso) {
	pthread_mutex_lock(&mutex_exit);
	actualizar_estado_proceso(logger, proceso, PCB_EXIT);
	list_add(cola_exit, proceso);
	pthread_mutex_unlock(&mutex_exit);

	sem_post(&contador_exit);
	sem_post(&sem_largo_plazo_exit);
}

t_pcb* sacar_proceso_de_exit() {
	sem_wait(&contador_exit);			

	pthread_mutex_lock(&mutex_exit);
	t_pcb* proceso = list_remove(cola_exit, 0);
	pthread_mutex_unlock(&mutex_exit);
	
	return proceso;
}

void hilo_planificador_largo_plazo_exit() {
	while(1) {
		sem_wait(&sem_largo_plazo_exit);
		
		t_pcb* proceso = sacar_proceso_de_exit();

		// avisar a consola
		liberar_pcb(proceso);
	}
}


/**
 * Planificador Corto Plazo
 * */

void meter_proceso_en_ready(t_pcb* proceso) {
	pthread_mutex_lock(&mutex_ready);
	actualizar_estado_proceso(logger, proceso, PCB_READY);
	list_add(cola_ready, proceso);
	pthread_mutex_unlock(&mutex_ready);

	sem_post(&contador_ready);
	sem_post(&sem_corto_plazo_ready);
}

t_pcb* sacar_proceso_de_ready(uint32_t index_proceso) {
	sem_wait(&contador_ready);			

	pthread_mutex_lock(&mutex_ready);
	t_pcb* proceso = list_remove(cola_ready, 0);
	pthread_mutex_unlock(&mutex_ready);
	
	return proceso;
}

void hilo_planificador_corto_plazo_ready() {
	while(1) {
		
		sem_wait(&sem_corto_plazo_ready);
		
		char* planificador = config_kernel->algoritmo_planificacion;
		uint32_t indice = indice_siguiente_proceso_segun(cola_ready, planificador);
		
		t_pcb* proceso = sacar_proceso_de_ready(indice);
		
		//wait(mutex_comunicacion_kernel_memoria)
		//send_proceso_a_memoria(proceso)				// memoria inicializa sus estructuras necesarias
		//proceso = esperar_proceso_de_memoria()		// obtenemos el indice de la tabla de paginas de cada segmento
		//signal(mutex_comunicacion_kernel_memoria)

		sem_post(&sem_comienza_timer_quantum);
		sem_wait(&sem_finaliza_timer_quantum);
		
		meter_proceso_en_exit(proceso);
		//meter_proceso_en_ready(proceso)
	}
}

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