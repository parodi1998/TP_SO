#include "include/kernel.h"

void inicializar_diccionario() {
    generador_pcb_id = 0;
    colas = dictionary_create();
    dictionary_put(colas,"NEW",queue_create());
    dictionary_put(colas,"READY_FIFO",queue_create());
    dictionary_put(colas,"READY_RR",queue_create());
    dictionary_put(colas,"EXECUTE",queue_create());
    dictionary_put(colas,"EXIT",queue_create());
    dictionary_put(colas,"PANTALLA",queue_create());    
}

void destruir_diccionario() {
    queue_destroy_and_destroy_elements((t_queue*)dictionary_get(colas,"NEW"),free);
    queue_destroy_and_destroy_elements((t_queue*)dictionary_get(colas,"READY_FIFO"),free);
    queue_destroy_and_destroy_elements((t_queue*)dictionary_get(colas,"READY_RR"),free);
    queue_destroy_and_destroy_elements((t_queue*)dictionary_get(colas,"EXECUTE"),free);
    queue_destroy_and_destroy_elements((t_queue*)dictionary_get(colas,"EXIT"),free);
    queue_destroy_and_destroy_elements((t_queue*)dictionary_get(colas,"PANTALLA"),free);
    dictionary_destroy(colas);
}

void crear_semaforos_y_mutex_de_cola_block_dinamica(char* key) {
    sem_t* mutex = malloc(sizeof(sem_t));
    sem_t* contador = malloc(sizeof(sem_t));
    sem_t* sem_hilo = malloc(sizeof(sem_t));

    sem_init(mutex, SEM_NOT_SHARE_BETWEEN_PROCESS, 1); 
	sem_init(contador, SEM_NOT_SHARE_BETWEEN_PROCESS, 0); 
	sem_init(sem_hilo, SEM_NOT_SHARE_BETWEEN_PROCESS, 0);

    dictionary_put(mutex_colas_block ,key, mutex);
    dictionary_put(contador_colas_block, key, contador);
    dictionary_put(sem_hilos_block, key, sem_hilo);
}

void inicializar_semaforos() {
    // new
	pthread_mutex_init(&mutex_new, NULL);
	sem_init(&contador_new, SEM_NOT_SHARE_BETWEEN_PROCESS, 0); 
	sem_init(&sem_largo_plazo_new, SEM_NOT_SHARE_BETWEEN_PROCESS, 0);
    // ready
    pthread_mutex_init(&mutex_ready, NULL);
    pthread_mutex_init(&mutex_ready_fifo, NULL);
	sem_init(&contador_ready_fifo, SEM_NOT_SHARE_BETWEEN_PROCESS, 0);
    pthread_mutex_init(&mutex_ready_rr, NULL);
	sem_init(&contador_ready_rr, SEM_NOT_SHARE_BETWEEN_PROCESS, 0);
    sem_init(&sem_corto_plazo_ready, SEM_NOT_SHARE_BETWEEN_PROCESS, 0);
    sem_init(&sem_proceso_agregado_a_ready, SEM_NOT_SHARE_BETWEEN_PROCESS, 0);
    sem_init(&sem_proceso_sacado_de_ready, SEM_NOT_SHARE_BETWEEN_PROCESS, 0);
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
    // multiprogramacion
    uint32_t grado_multiprogramacion = atoi(config_kernel->grado_max_multiprogramacion);
    sem_init(&sem_grado_multiprogramacion, SEM_NOT_SHARE_BETWEEN_PROCESS, grado_multiprogramacion);

    // block con diccionarios
    contador_colas_block = dictionary_create();
    sem_hilos_block = dictionary_create();
    mutex_colas_block = dictionary_create();
    crear_semaforos_y_mutex_de_cola_block_dinamica("PANTALLA");
}

void destruir_semaforos_y_mutex_de_cola_block_dinamica(char* key) {
    sem_t* mutex_pointer = dictionary_get(mutex_colas_block ,key);
	// sem_t mutex = *mutex_pointer;
	sem_t* contador_pointer = dictionary_get(contador_colas_block, key);
	//sem_t contador = *contador_pointer;
	sem_t* sem_hilo_pointer = dictionary_get(sem_hilos_block, key);
	//sem_t sem_hilo = *sem_hilo_pointer;

    sem_destroy(mutex_pointer);
    sem_destroy(contador_pointer);
    sem_destroy(sem_hilo_pointer);
}

void destruir_semaforos() {
    pthread_mutex_destroy(&mutex_new);
    sem_destroy(&contador_new);
    sem_destroy(&sem_largo_plazo_new);

    pthread_mutex_destroy(&mutex_ready);
    pthread_mutex_destroy(&mutex_ready_fifo);
    sem_destroy(&contador_ready_fifo);
    pthread_mutex_destroy(&mutex_ready_rr);
    sem_destroy(&contador_ready_rr);
    sem_destroy(&sem_corto_plazo_ready);
    sem_destroy(&sem_proceso_agregado_a_ready);
    sem_destroy(&sem_proceso_sacado_de_ready);

    pthread_mutex_destroy(&mutex_execute);
    sem_destroy(&contador_execute);
    sem_destroy(&sem_corto_plazo_execute);
    sem_destroy(&sem_cpu_libre);

    pthread_mutex_destroy(&mutex_exit);
    sem_destroy(&contador_exit);
    sem_destroy(&sem_largo_plazo_exit);

    sem_destroy(&sem_comienza_timer_quantum);
    sem_destroy(&sem_finaliza_timer_quantum);

    // BLOCK
    destruir_semaforos_y_mutex_de_cola_block_dinamica("PANTALLA");
    dictionary_destroy(contador_colas_block);
    dictionary_destroy(sem_hilos_block);
    dictionary_destroy(mutex_colas_block);
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

    pthread_create(&hilo_block, NULL, (void*)hilo_planificador_block, (void*) "PANTALLA");
	pthread_detach(hilo_block);
}

void inicializar_todo() {
    inicializar_semaforos();
    inicializar_planificadores();
    inicializar_diccionario(); 
}

void destruir_todo() {
    destruir_diccionario();
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