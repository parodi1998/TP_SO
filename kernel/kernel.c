#include "include/kernel.h"

int fd_memoria = 0;
int fd_cpu_dispatch = 0;
int fd_cpu_interrupt = 0;
int kernel_server_fd = 0;

t_dictionary* colas;
t_dictionary* contador_colas_block;
t_dictionary* sem_hilos_block;
t_dictionary* mutex_colas_block;
t_dictionary* tiempos_io;

t_dictionary* dato_ingreso_por_teclado;
t_dictionary* threads_dinamicos;

t_queue* cola_de_keys_de_diccionario;

pthread_mutex_t mutex_dictionary_colas;
pthread_mutex_t mutex_dictionary_contador_colas_block;
pthread_mutex_t mutex_dictionary_sem_hilos_block;
pthread_mutex_t mutex_dictionary_mutex_colas_block;
pthread_mutex_t mutex_dictionary_tiempos_io;
pthread_mutex_t mutex_dictionary_dato_ingreso_por_teclado;
pthread_mutex_t mutex_dictionary_threads_dinamicos;

pthread_mutex_t mutex_new;
pthread_mutex_t mutex_ready;
pthread_mutex_t mutex_ready_fifo;
pthread_mutex_t mutex_ready_rr;
pthread_mutex_t mutex_execute;
pthread_mutex_t mutex_exit;

pthread_mutex_t mutex_generador_pcb_id;

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
sem_t sem_grado_multiprogramacion;
sem_t sem_sincro_cargar_segmentos_en_memoria;
sem_t sem_sincro_finalizar_pcb_en_memoria;

sem_t sem_sacar_de_execute;
sem_t sem_finalizar_proceso;

sem_t sem_fin_io_pantalla;
sem_t sem_dato_por_teclado_ingresado;


pthread_t hilo_largo_plazo_new;
pthread_t hilo_corto_plazo_ready;
pthread_t hilo_corto_plazo_execute;
pthread_t hilo_largo_plazo_exit;
pthread_t hilo_cuenta_quantum;
pthread_t hilo_block_pantalla;
pthread_t hilo_block_teclado;
pthread_t hilo_block_page_fault;

uint32_t generador_pcb_id;

t_log* logger;
t_log* logger_kernel_obligatorio;
t_config_kernel* config_kernel;

void inicializar_diccionario() {
    generador_pcb_id = 0;
    colas = dictionary_create();
    tiempos_io = dictionary_create();
    dato_ingreso_por_teclado = dictionary_create();
    threads_dinamicos = dictionary_create();
    dictionary_put(colas,"NEW",queue_create());
    dictionary_put(colas,"READY_FIFO",queue_create());
    dictionary_put(colas,"READY_RR",queue_create());
    dictionary_put(colas,"EXECUTE",queue_create());
    dictionary_put(colas,"EXIT",queue_create());
    dictionary_put(colas,"PAGE_FAULT",queue_create());

    queue_push(cola_de_keys_de_diccionario, "NEW");
    queue_push(cola_de_keys_de_diccionario, "READY_FIFO");
    queue_push(cola_de_keys_de_diccionario, "READY_RR");
    queue_push(cola_de_keys_de_diccionario, "EXECUTE");
    queue_push(cola_de_keys_de_diccionario, "EXIT");
    queue_push(cola_de_keys_de_diccionario, "PAGE_FAULT");
}
/*
void destruir_diccionario() {
    queue_destroy_and_destroy_elements((t_queue*)dictionary_get(colas,"NEW"),free);
    queue_destroy_and_destroy_elements((t_queue*)dictionary_get(colas,"READY_FIFO"),free);
    queue_destroy_and_destroy_elements((t_queue*)dictionary_get(colas,"READY_RR"),free);
    queue_destroy_and_destroy_elements((t_queue*)dictionary_get(colas,"EXECUTE"),free);
    queue_destroy_and_destroy_elements((t_queue*)dictionary_get(colas,"EXIT"),free);
    queue_destroy_and_destroy_elements((t_queue*)dictionary_get(colas,"PAGE_FAULT"),free);

    dictionary_destroy(colas);
    dictionary_destroy(tiempos_io);
    dictionary_destroy(dato_ingreso_por_teclado);
    dictionary_destroy(threads_dinamicos);
}*/

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
    sem_init(&sem_sincro_cargar_segmentos_en_memoria, SEM_NOT_SHARE_BETWEEN_PROCESS, 0);
    sem_init(&sem_sincro_finalizar_pcb_en_memoria, SEM_NOT_SHARE_BETWEEN_PROCESS, 0);
    sem_init(&sem_sacar_de_execute, SEM_NOT_SHARE_BETWEEN_PROCESS, 0);
    sem_init(&sem_finalizar_proceso, SEM_NOT_SHARE_BETWEEN_PROCESS, 0);
    sem_init(&sem_fin_io_pantalla, SEM_NOT_SHARE_BETWEEN_PROCESS, 0);
    sem_init(&sem_dato_por_teclado_ingresado, SEM_NOT_SHARE_BETWEEN_PROCESS, 0);

    // new
	pthread_mutex_init(&mutex_dictionary_colas, NULL);
	pthread_mutex_init(&mutex_dictionary_contador_colas_block, NULL);
	pthread_mutex_init(&mutex_dictionary_sem_hilos_block, NULL);
	pthread_mutex_init(&mutex_dictionary_mutex_colas_block, NULL);
	pthread_mutex_init(&mutex_dictionary_tiempos_io, NULL);
	pthread_mutex_init(&mutex_dictionary_dato_ingreso_por_teclado, NULL);
	pthread_mutex_init(&mutex_dictionary_threads_dinamicos, NULL);
	pthread_mutex_init(&mutex_generador_pcb_id, NULL);
	
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
    crear_semaforos_y_mutex_de_cola_block_dinamica("PAGE_FAULT");
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
    sem_destroy(&sem_sincro_cargar_segmentos_en_memoria);
    sem_destroy(&sem_sincro_finalizar_pcb_en_memoria);
    sem_destroy(&sem_sacar_de_execute);
    sem_destroy(&sem_finalizar_proceso);
    sem_destroy(&sem_fin_io_pantalla);
    sem_destroy(&sem_dato_por_teclado_ingresado);
    
    pthread_mutex_destroy(&mutex_dictionary_colas);
    pthread_mutex_destroy(&mutex_dictionary_contador_colas_block);
    pthread_mutex_destroy(&mutex_dictionary_sem_hilos_block);
    pthread_mutex_destroy(&mutex_dictionary_mutex_colas_block);
    pthread_mutex_destroy(&mutex_dictionary_tiempos_io);
    pthread_mutex_destroy(&mutex_dictionary_dato_ingreso_por_teclado);
    pthread_mutex_destroy(&mutex_dictionary_threads_dinamicos);
    pthread_mutex_destroy(&mutex_generador_pcb_id);

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
    sem_destroy(&sem_grado_multiprogramacion);

    // BLOCK
    /*
    destruir_semaforos_y_mutex_de_cola_block_dinamica("PAGE_FAULT");
    dictionary_destroy(contador_colas_block);
    dictionary_destroy(sem_hilos_block);
    dictionary_destroy(mutex_colas_block);
    */
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

    pthread_create(&hilo_block_page_fault, NULL, (void*)hilo_planificador_block_page_fault, NULL);
	pthread_detach(hilo_block_page_fault);
}

static void inicializar_estructuras_para_io_block_dinamica() {
    for(int index = 0; index < list_size(config_kernel->dispositivos_IO); index++) {
        char* dispositivo_io = list_get(config_kernel->dispositivos_IO, index);
        char* tiempo_io = list_get(config_kernel->tiempos_IO, index);
        dictionary_put(colas, dispositivo_io, queue_create());
        dictionary_put(tiempos_io, dispositivo_io, tiempo_io);
        crear_semaforos_y_mutex_de_cola_block_dinamica(dispositivo_io);

        pthread_t* hilo_block_io = malloc(sizeof(pthread_t));
        pthread_create(hilo_block_io, NULL, (void*)hilo_planificador_block_io, (void*) dispositivo_io);
	    pthread_detach(*hilo_block_io);

        dictionary_put(threads_dinamicos, dispositivo_io, hilo_block_io);

        queue_push(cola_de_keys_de_diccionario, dispositivo_io);
    } 
}
/*
static void destruir_estructuras_para_io_block_dinamica() {
    for(int index = 0; index < list_size(config_kernel->dispositivos_IO); index++) {
        char* dispositivo_io = list_get(config_kernel->dispositivos_IO, index);
        t_queue* cola_a_borrar = dictionary_remove(colas, dispositivo_io);
        char* tiempo_io_a_borrar = dictionary_remove(tiempos_io, dispositivo_io);
        queue_destroy_and_destroy_elements(cola_a_borrar, free);
        free(tiempo_io_a_borrar);
        destruir_semaforos_y_mutex_de_cola_block_dinamica(dispositivo_io);
    }
}*/

void inicializar_todo() {
    inicializar_semaforos();
    inicializar_planificadores();
    inicializar_diccionario(); 
    inicializar_estructuras_para_io_block_dinamica();
}

void destruir_elemento_queue_de_diccionario_si_existe_key(t_dictionary* diccionario, char* key) {
    if(dictionary_has_key(diccionario,key)) {
        t_queue* queue_a_borrar = dictionary_remove(diccionario, key);
        queue_destroy_and_destroy_elements(queue_a_borrar, free);
    }
}

void destruir_elemento_char_de_diccionario_si_existe_key(t_dictionary* diccionario, char* key) {
    if(dictionary_has_key(diccionario,key)) {
        char* char_a_borrar = dictionary_remove(diccionario, key);
        free(char_a_borrar);
    }
}

void destruir_elemento_sem_t_de_diccionario_si_existe_key(t_dictionary* diccionario, char* key) {
    if(dictionary_has_key(diccionario,key)) {
        sem_t* semaforo_a_borrar = dictionary_remove(diccionario, key);
        sem_destroy(semaforo_a_borrar);
    }
}

void destruir_elemento_pthread_t_de_diccionario_si_existe_key(t_dictionary* diccionario, char* key) {
    if(dictionary_has_key(diccionario,key)) {
        pthread_t* thread_a_borrar = dictionary_remove(diccionario, key);
        pthread_cancel(*thread_a_borrar);
    }
}

void destruir_y_remover_elementos_de_diccionarios() {
    
    bool lista_de_keys_esta_vacia = queue_is_empty(cola_de_keys_de_diccionario);

    if(lista_de_keys_esta_vacia) {
        return;
    }
    
    while(!lista_de_keys_esta_vacia) {
        char* key = queue_pop(cola_de_keys_de_diccionario);
        
        log_info(logger,"KEY: %s", key);
        log_info(logger,"Diccionario colas size: %d", dictionary_size(colas));
        log_info(logger,"Diccionario tiempos_io size: %d", dictionary_size(tiempos_io));
        log_info(logger,"Diccionario dato_ingreso_por_teclado size: %d", dictionary_size(dato_ingreso_por_teclado));
        log_info(logger,"Diccionario contador_colas_block size: %d", dictionary_size(contador_colas_block));
        log_info(logger,"Diccionario sem_hilos_block size: %d", dictionary_size(sem_hilos_block));
        log_info(logger,"Diccionario mutex_colas_block size: %d", dictionary_size(mutex_colas_block));
        log_info(logger,"Diccionario threads_dinamicos size: %d", dictionary_size(threads_dinamicos));
        destruir_elemento_sem_t_de_diccionario_si_existe_key(threads_dinamicos, key);
        destruir_elemento_queue_de_diccionario_si_existe_key(colas,key);
        destruir_elemento_char_de_diccionario_si_existe_key(tiempos_io, key);
        destruir_elemento_char_de_diccionario_si_existe_key(dato_ingreso_por_teclado, key);
        destruir_elemento_sem_t_de_diccionario_si_existe_key(contador_colas_block, key);
        destruir_elemento_sem_t_de_diccionario_si_existe_key(sem_hilos_block, key);
        destruir_elemento_sem_t_de_diccionario_si_existe_key(mutex_colas_block, key);
        free(key);

        lista_de_keys_esta_vacia = queue_is_empty(cola_de_keys_de_diccionario);
    }
    queue_destroy_and_destroy_elements(cola_de_keys_de_diccionario, free);
}

void cerrar_hilos() {
    pthread_cancel(hilo_largo_plazo_new);
    pthread_cancel(hilo_corto_plazo_ready);
    pthread_cancel(hilo_corto_plazo_execute);
    pthread_cancel(hilo_largo_plazo_exit);
    pthread_cancel(hilo_cuenta_quantum);
    pthread_cancel(hilo_block_pantalla);
    pthread_cancel(hilo_block_teclado);
    pthread_cancel(hilo_block_page_fault);
}

void destruir_todo() {

    cerrar_hilos();

    destruir_y_remover_elementos_de_diccionarios();

    destruir_semaforos();
}

void handle_sigint(int sig)
{
    destruir_todo();
    terminar_programa();
    exit(EXIT_SUCCESS);
}

int main(int argc, char** argv){
    
    if(!iniciar_programa()) {
        terminar_programa();
        return EXIT_SUCCESS;
    }

    if(!generar_conexion_kernel_a_memoria(logger, config_kernel->ip_memoria, config_kernel->puerto_memoria, &fd_memoria)) {
        log_error(logger,"No se pudo generar la conexion con el modulo memoria");
        terminar_programa();
        return EXIT_FAILURE;
    }

    if(!generar_conexion_kernel_a_cpu_dispatch(logger, config_kernel->ip_cpu, config_kernel->puerto_cpu_dispatch, &fd_cpu_dispatch)) {
        log_error(logger,"No se pudo generar la conexion con cpu dispatch");
        terminar_programa();
        return EXIT_FAILURE;
    }

    if(!generar_conexion_kernel_a_cpu_interrupt(logger, config_kernel->ip_cpu, config_kernel->puerto_cpu_interrupt, &fd_cpu_interrupt)) {
        log_error(logger,"No se pudo generar la conexion con cpu interrupt");
        terminar_programa();
        return EXIT_FAILURE;
    }

    kernel_server_fd = 0;
    if(!iniciar_kernel(&kernel_server_fd)) {
        log_error(logger,"No se pudo iniciar kernel como servidor");
        terminar_programa();
        return EXIT_FAILURE;
    }

    cola_de_keys_de_diccionario = queue_create();
    inicializar_todo();

    signal(SIGINT, handle_sigint);

	log_info(logger, "Servidor listo para recibir al cliente");
    while (server_escuchar(logger, "kernel", kernel_server_fd));    

    liberar_conexion(&kernel_server_fd);
    destruir_todo();
    terminar_programa();

    return EXIT_SUCCESS;
}