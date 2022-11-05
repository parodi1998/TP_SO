#include "../include/planificacion.h"

void procesoANew (t_pcb* proceso)
{
    pthread_mutex_lock(&mutexNew);
	queue_push(colaNew, proceso);
	log_info(logger, "Entra el proceso de ID: %d a la cola de new.", proceso->id_proceso);
	pthread_mutex_unlock(&mutexNew);
    sem_post(&contadorNew); // Despierta al planificador de largo plazo
	sem_post(&largoPlazo);

}

t_pcb* sacarDeNew(){

	sem_wait(&contadorNew);
	pthread_mutex_lock(&mutexNew);

	t_pcb* proceso = queue_pop(colaNew);
	log_info(logger, "[NEW] Se saca el proceso de PID: %d de la cola", proceso->id_proceso);

	pthread_mutex_unlock(&mutexNew);

	return proceso;
}

void hiloNew_Ready(){

		sem_wait(&largoPlazo);//Este se trigerea cada vez que entra un proceso al new
        sem_wait(&multiprogramacion);//Hay que ver como hacer lo de la mp
		t_pcb* proceso = sacarDeNew();//Saca de la cola en fifo
        //agregarAReady(proceso);
}


