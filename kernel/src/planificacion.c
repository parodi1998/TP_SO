#include "../include/planificacion.h"

void meter_proceso_en_new(t_pcb* proceso) {

	pthread_mutex_lock(&mutex_new);
	list_add(cola_new, proceso);
	log_proceso_en_new(logger, proceso);
	pthread_mutex_unlock(&mutex_new);

	sem_post(&contador_new);
	sem_post(&sem_largo_plazo_new);
}

/**
 * Planificador Largo Plazo
 * */
void hilo_planificador_largo_plazo_new() {
	
	while(1) {
		
		sem_wait(&sem_largo_plazo_new);					// espera que le avisen que puede hacer algo
		//wait(multiprogramacion)
		char* planificador = "FIFO";
		//uint32_t indice = indice_siguiente_proceso_segun(cola_new, planificador)
		uint32_t indice = 0; //Aca deberiamos poner siempre Fifo
		//t_pcb* proceso = sacar_proceso_de_new(indice);
		
		//wait(mutex_comunicacion_kernel_memoria)
		//send_proceso_a_memoria(proceso)				// memoria inicializa sus estructuras necesarias
		//proceso = esperar_proceso_de_memoria()		// obtenemos el indice de la tabla de paginas de cada segmento
		//signal(mutex_comunicacion_kernel_memoria)

		//meter_proceso_en_ready(proceso)
	}

}


/*
t_pcb* sacarDeNew(){

	sem_wait(&contadorNew);
	pthread_mutex_lock(&mutexNew);

	t_pcb* proceso = queue_pop(colaNew);
	log_info(logger, "[NEW] Se saca el proceso de PID: %d de la cola", proceso->id_proceso);

	pthread_mutex_unlock(&mutexNew);

	return proceso;
}

//hilos

void hiloNewRdy(){
		sem_wait(&largoPlazo);//Este se trigerea cada vez que entra un proceso al new
        sem_wait(&multiprogramacion);//Hay que ver como hacer lo de la mp
		t_pcb* proceso = sacarDeNew();//Saca de la cola en fifo
        //agregarAReady(proceso);
}
*/
/*
cola_new
cola_exit

func meter_proceso_en_new(proceso) {
	wait(mutex_new)
	agregar_a_lista(cola_new, proceso)
	signal(mutex_new)

	signal(sem_new)
	signal(sem_largo_plazo_new)
	
}

func sacar_proceso_de_new(index_proceso): proceso {
	wait(sem_new) // si por casualidad esto se llama y no hay nada en new (no deberia pasar nunca) se bloquea

	wait(mutex_new)
	proceso = sacar_item_de_lista(cola_new,index_proceso)
	signal(mutex_new)
	
	return proceso
}

func meter_proceso_en_exit(proceso) {
	wait(mutex_exit)
	agregar_a_lista(cola_exit, proceso)
	signal(mutex_exit)

	signal(sem_exit);
	
}

func hilo_planificador_largo_plazo_new() {

	wait(sem_largo_plazo_new) // espera que le avisen que puede hacer algo
	wait(multiprogramacion)
	planificador = config.planificador
	indice = indice_siguiente_proceso_segun(cola_new, planificador) //Aca deberiamos poner siempre Fifo
	proceso = sacar_proceso_de_new(indice)
	
	wait(mutex_comunicacion_kernel_memoria)
	send_proceso_a_memoria(proceso)				// memoria inicializa sus estructuras necesarias
	proceso = esperar_proceso_de_memoria()		// obtenemos el indice de la tabla de paginas de cada segmento
	signal(mutex_comunicacion_kernel_memoria)

	meter_proceso_en_ready(proceso)
}

func hilo_planificador_largo_plazo_exit() {

	wait(sem_largo_plazo_exit) // espera que le avisen que puede hacer algo
}

func indice_siguiente_proceso_segun(lista, planificador): int {
	indice;
	switch(planificador){
		FIFO:
			calcular_indice_siguiente_proceso_fifo(lista);
			break;
		default:
			indice = 0;
			break;
	}
	return indice;
}

func calcular_indice_siguiente_proceso_fifo(lista) {
	return 0;
}

*/