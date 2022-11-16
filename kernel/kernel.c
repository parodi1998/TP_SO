#include "include/kernel.h"

void inicializar_listas() {
    generador_pcb_id = 0;
	cola_new = list_create();
    cola_ready = list_create();
    cola_execute = list_create();
    cola_exit = list_create();
}

void destruir_listas() {
    list_destroy_and_destroy_elements(cola_new,free);
    list_destroy_and_destroy_elements(cola_ready,free);
    list_destroy_and_destroy_elements(cola_execute,free);
    list_destroy_and_destroy_elements(cola_exit,free);
}

void inicializar_semaforos() {
    // new
	pthread_mutex_init(&mutex_new, NULL);
	sem_init(&contador_new, SEM_NOT_SHARE_BETWEEN_PROCESS, 0); 
	sem_init(&sem_largo_plazo_new, SEM_NOT_SHARE_BETWEEN_PROCESS, 0);
    // ready
    pthread_mutex_init(&mutex_ready, NULL);
	sem_init(&contador_ready, SEM_NOT_SHARE_BETWEEN_PROCESS, 0); 
	sem_init(&sem_corto_plazo_ready, SEM_NOT_SHARE_BETWEEN_PROCESS, 0);
    // execute
    pthread_mutex_init(&mutex_execute, NULL);
	sem_init(&contador_execute, SEM_NOT_SHARE_BETWEEN_PROCESS, 0); 
	sem_init(&sem_corto_plazo_execute, SEM_NOT_SHARE_BETWEEN_PROCESS, 0);
    sem_init(&sem_cpu_libre, SEM_NOT_SHARE_BETWEEN_PROCESS, 1);
    //exit
    pthread_mutex_init(&mutex_exit, NULL);
	sem_init(&contador_exit, SEM_NOT_SHARE_BETWEEN_PROCESS, 0); 
	sem_init(&sem_largo_plazo_exit, SEM_NOT_SHARE_BETWEEN_PROCESS, 0);
    // quantum
    sem_init(&sem_comienza_timer_quantum, SEM_NOT_SHARE_BETWEEN_PROCESS, 0);
    sem_init(&sem_finaliza_timer_quantum, SEM_NOT_SHARE_BETWEEN_PROCESS, 0);
}

void destruir_semaforos() {
    pthread_mutex_destroy(&mutex_new);
    sem_destroy(&contador_new);
    sem_destroy(&sem_largo_plazo_new);

    pthread_mutex_destroy(&mutex_ready);
    sem_destroy(&contador_ready);
    sem_destroy(&sem_corto_plazo_ready);

    pthread_mutex_destroy(&mutex_execute);
    sem_destroy(&contador_execute);
    sem_destroy(&sem_corto_plazo_execute);
    sem_destroy(&sem_cpu_libre);

    pthread_mutex_destroy(&mutex_exit);
    sem_destroy(&contador_exit);
    sem_destroy(&sem_largo_plazo_exit);

    sem_destroy(&sem_comienza_timer_quantum);
    sem_destroy(&sem_finaliza_timer_quantum);
}

void inicializar_planificadores() {
	pthread_create(&hilo_largo_plazo_new, NULL, (void*)hilo_planificador_largo_plazo_new, NULL);
	pthread_detach(hilo_largo_plazo_new);

    pthread_create(&hilo_corto_plazo_ready, NULL, (void*)hilo_planificador_corto_plazo_ready, NULL);
	pthread_detach(hilo_corto_plazo_ready);

    pthread_create(&hilo_corto_plazo_execute, NULL, (void*)hilo_planificador_corto_plazo_execute, NULL);
	pthread_detach(hilo_corto_plazo_execute);

    pthread_create(&hilo_largo_plazo_exit, NULL, (void*)hilo_planificador_largo_plazo_exit, NULL);
	pthread_detach(hilo_largo_plazo_exit);

    pthread_create(&hilo_cuenta_quantum, NULL, (void*)hilo_timer_contador_quantum, NULL);
	pthread_detach(hilo_cuenta_quantum);
}

void inicializar_todo() {
    inicializar_listas();
    inicializar_semaforos();
    inicializar_planificadores();
}

void destruir_todo() {
    destruir_listas();
    destruir_semaforos();
}

int main(int argc, char** argv){
    
    if(!iniciar_programa()) {
        terminar_programa();
        return EXIT_SUCCESS;
    }

    /*
    int fd_cpu_dispatch = 0;
    if (!generar_conexiones_cpu(logger, config_kernel->ip_cpu, config_kernel->puerto_cpu_dispatch, &fd_cpu_dispatch)) {
    		terminar_programa();
    		return EXIT_FAILURE;
    }
    int fd_cpu_interrupt = 0;
    if (!generar_conexiones_cpu(logger, config_kernel->ip_cpu, config_kernel->puerto_cpu_interrupt, &fd_cpu_interrupt)) {
      		terminar_programa();
      		return EXIT_FAILURE;
     }
*/

    int kernel_server_fd = 0;
    if(!iniciar_kernel(&kernel_server_fd)) {
        log_error(logger,"No se pudo generar iniciar el proceso kernel");
        terminar_programa();
        return EXIT_FAILURE;
    }

    inicializar_todo();

	log_info(logger, "Servidor listo para recibir al cliente");
    while (server_escuchar(logger, "kernel", kernel_server_fd));    

    liberar_conexion(&kernel_server_fd);
    destruir_todo();
    terminar_programa();

    return EXIT_SUCCESS;
}