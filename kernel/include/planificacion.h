#ifndef KERNEL_PLANIFICACION_H_
#define KERNEL_PLANIFICACION_H_

#include "commons.h"

#define SEM_NOT_SHARE_BETWEEN_PROCESS 0

t_list* cola_new;
// t_list* cola_exit;

pthread_mutex_t mutex_new;

sem_t contador_new;
sem_t sem_largo_plazo_new;

void meter_proceso_en_new(t_pcb* proceso);

#endif