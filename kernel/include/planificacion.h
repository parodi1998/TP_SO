#ifndef KERNEL_PLANIFICACION_H_
#define KERNEL_PLANIFICACION_H_

#include "commons.h"
#include "kernel_logs.h"
#include "conexion.h"

#define SEM_NOT_SHARE_BETWEEN_PROCESS 0

typedef enum {
	PLANIFICADOR_FIFO,
    PLANIFICADOR_RR,
    PLANIFICADOR_FEEDBACK
} t_tipo_planificador;

extern int fd_memoria;
extern int fd_cpu_dispatch;
extern int fd_cpu_interrupt;

// diccionario de colas
extern t_dictionary* colas;
extern t_dictionary* contador_colas_block;
extern t_dictionary* sem_hilos_block;
extern t_dictionary* mutex_colas_block;
extern t_dictionary* tiempos_io;

extern t_dictionary* dato_ingreso_por_teclado;

// mutex
extern pthread_mutex_t mutex_dictionary_colas;
extern pthread_mutex_t mutex_dictionary_contador_colas_block;
extern pthread_mutex_t mutex_dictionary_sem_hilos_block;
extern pthread_mutex_t mutex_dictionary_mutex_colas_block;
extern pthread_mutex_t mutex_dictionary_tiempos_io;
extern pthread_mutex_t mutex_dictionary_dato_ingreso_por_teclado;

extern pthread_mutex_t mutex_new;
extern pthread_mutex_t mutex_ready;
extern pthread_mutex_t mutex_ready_fifo;
extern pthread_mutex_t mutex_ready_rr;
extern pthread_mutex_t mutex_execute;
extern pthread_mutex_t mutex_exit;

extern pthread_mutex_t mutex_timer_quantum;
extern pthread_mutex_t mutex_debe_ser_interrumpido_cpu;

// semaforos contadores
extern sem_t contador_new;
extern sem_t contador_ready_fifo;
extern sem_t contador_ready_rr;
extern sem_t contador_execute;
extern sem_t contador_exit;
extern sem_t sem_largo_plazo_new;
extern sem_t sem_corto_plazo_ready;
extern sem_t sem_corto_plazo_execute;
extern sem_t sem_largo_plazo_exit;
extern sem_t sem_cpu_libre;
extern sem_t sem_comienza_timer_quantum;
extern sem_t sem_finaliza_timer_quantum;
extern sem_t sem_proceso_agregado_a_ready;
extern sem_t sem_proceso_sacado_de_ready;
extern sem_t sem_grado_multiprogramacion;
extern sem_t sem_sincro_cargar_segmentos_en_memoria;
extern sem_t sem_sincro_finalizar_pcb_en_memoria;

extern sem_t sem_sacar_de_execute;
extern sem_t sem_finalizar_proceso;
extern sem_t sem_fin_io_pantalla;
extern sem_t sem_dato_por_teclado_ingresado;

// hilos
extern pthread_t hilo_largo_plazo_new;
extern pthread_t hilo_corto_plazo_ready;
extern pthread_t hilo_corto_plazo_execute;
extern pthread_t hilo_largo_plazo_exit;
extern pthread_t hilo_cuenta_quantum;
extern pthread_t hilo_block_pantalla;
extern pthread_t hilo_block_teclado;
extern pthread_t hilo_block_page_fault;

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

void devolver_proceso_a_ready(t_pcb* proceso);
void meter_proceso_en_execute(t_pcb* proceso);
t_pcb* sacar_proceso_de_execute();
void hilo_planificador_corto_plazo_execute();

void hilo_timer_contador_quantum();

// Planificador de bloqueos
void meter_proceso_en_block(t_pcb* proceso, char* key_cola_de_bloqueo);
t_pcb* sacar_proceso_de_block(char* key_cola_de_bloqueo);
void hilo_planificador_block_io(void* args);
void hilo_planificador_block_pantalla();
void hilo_planificador_block_teclado();
void hilo_planificador_block_page_fault();

#endif