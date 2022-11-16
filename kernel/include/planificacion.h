#ifndef KERNEL_PLANIFICACION_H_
#define KERNEL_PLANIFICACION_H_

#include "commons.h"

#define SEM_NOT_SHARE_BETWEEN_PROCESS 0

typedef enum {
	PLANIFICADOR_FIFO,
    PLANIFICADOR_RR,
    PLANIFICADOR_FEEDBACK
} t_tipo_planificador;

// colas
t_list* cola_new;
// t_list* cola_exit;

// mutex
pthread_mutex_t mutex_new;

// semaforos contadores
sem_t contador_new;
sem_t sem_largo_plazo_new;

// hilos
pthread_t hilo_largo_plazo_new;

// Planificador largo plazo
void meter_proceso_en_new(t_pcb* proceso);
void hilo_planificador_largo_plazo_new();

#endif