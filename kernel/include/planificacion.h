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
t_queue* cola_new;
t_queue* cola_ready_rr;
t_queue* cola_ready_fifo;
t_queue* cola_execute;
t_queue* cola_exit;

// mutex
pthread_mutex_t mutex_new;
pthread_mutex_t mutex_ready;
pthread_mutex_t mutex_ready_fifo;
pthread_mutex_t mutex_ready_rr;
pthread_mutex_t mutex_execute;
pthread_mutex_t mutex_exit;

// semaforos contadores
sem_t contador_new;
sem_t contador_ready_fifo;
sem_t contador_ready_rr;
sem_t contador_execute;
sem_t contador_exit;
sem_t sem_largo_plazo_new;
sem_t sem_corto_plazo_ready;
sem_t sem_corto_plazo_execute;
sem_t sem_largo_plazo_exit;
sem_t sem_cpu_libre;
sem_t sem_comienza_timer_quantum;
sem_t sem_finaliza_timer_quantum;
sem_t sem_proceso_agregado_a_ready;
sem_t sem_proceso_sacado_de_ready;

// hilos
pthread_t hilo_largo_plazo_new;
pthread_t hilo_corto_plazo_ready;
pthread_t hilo_corto_plazo_execute;
pthread_t hilo_largo_plazo_exit;
pthread_t hilo_cuenta_quantum;

// Planificador largo plazo
void meter_proceso_en_new(t_pcb* proceso);
t_pcb* sacar_proceso_de_new();
void hilo_planificador_largo_plazo_new();

void meter_proceso_en_exit(t_pcb* proceso);
t_pcb* sacar_proceso_de_exit();
void hilo_planificador_largo_plazo_exit();

// Planificador corto plazo
void meter_proceso_en_ready(t_pcb* proceso);
void meter_proceso_en_ready_fifo(t_pcb* proceso);
void meter_proceso_en_ready_rr(t_pcb* proceso);
void meter_proceso_en_ready_feedback(t_pcb* proceso);  
t_pcb* sacar_proceso_de_ready();
t_pcb* sacar_proceso_de_ready_fifo();
t_pcb* sacar_proceso_de_ready_rr();
t_pcb* sacar_proceso_de_ready_feedback();
void hilo_planificador_corto_plazo_ready();

void meter_proceso_en_execute(t_pcb* proceso);
t_pcb* sacar_proceso_de_execute();
void hilo_planificador_corto_plazo_execute();

void hilo_timer_contador_quantum();

#endif