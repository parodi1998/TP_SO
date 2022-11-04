#include "../include/planificacion.h"


void procesoANew (t_pcb* proceso)
{
    pthread_mutex_lock(&mutexNew);
	queue_push(colaNew, proceso);
	log_info(log_kernel, "Entra el proceso de PID: %d a la cola de new.", proceso->id_proceso);
	pthread_mutex_unlock(&mutexNew);
    sem_post(&contadorNew); // Despierta al planificador de largo plazo
	sem_post(&largoPlazo);

}

pcb_t* sacarDeNew(){

	sem_wait(&contadorNew);
	pthread_mutex_lock(&mutexNew);

	pcb_t* proceso = queue_pop(colaNew);
	log_info(log_kernel, "[NEW] Se saca el proceso de PID: %d de la cola", proceso->PID);

	pthread_mutex_unlock(&mutexNew);

	return proceso;
}

void hiloNew_Ready(){

		sem_wait(&largoPlazo);//Este se trigerea cada vez que entra un proceso al new
        sem_wait(&multiprogramacion);//Hay que ver como hacer lo de la mp
		pcb_t* proceso = sacarDeNew();//Saca de la cola en fifo
        agregarAReady(proceso);
}


