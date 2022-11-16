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

		//meter_proceso_en_ready(proceso)
	}

}


/*

func meter_proceso_en_exit(proceso) {
	wait(mutex_exit)
	agregar_a_lista(cola_exit, proceso)
	signal(mutex_exit)

	signal(sem_exit);
	
}

func hilo_planificador_largo_plazo_exit() {

	wait(sem_largo_plazo_exit) // espera que le avisen que puede hacer algo
}


*/