#ifndef KERNEL_PLANIFICACION_H_
#define KERNEL_PLANIFICACION_H_

#include "commons.h"
t_queue* colaNew;
t_queue* colaReadySuspended;
t_list* colaReady;
t_list* listaExit;

pthread_mutex_t mutexNew;
pthread_mutex_t mutexRdy;
pthread_mutex_t mutexBlck;
pthread_mutex_t mutexExe;
pthread_mutex_t mutexExit;
pthread_mutex_t multiprocesamiento;

sem_t contadorNew;
sem_t contadorReady;
sem_t contadorExe;
sem_t contadorBlock;
sem_t multiprogramacion;
sem_t largoPlazo;
sem_t medianoPlazo;


typedef struct {
    uint32_t id_proceso;
    t_list* instrucciones;
    uint32_t program_counter;
    uint32_t registros_cpu;
    uint32_t tabla_segmentos;
    char* estado;
} t_pcb;
void procesoANew (t_pcb* proceso)
#endif