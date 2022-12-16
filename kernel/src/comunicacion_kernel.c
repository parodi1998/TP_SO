#include "../include/comunicacion_kernel.h"
#include "../include/planificacion.h"

static void carga_tabla_segmentos_pcb( t_list** lista_a_cargar, t_list* segmentos) {
    t_list* tabla_segmentos = list_create();
    uint32_t index;
    uint32_t size = list_size(segmentos);
    for(index = 0; index < size; index++) {
        char* segmento = list_get(segmentos,index);
        t_pcb_segmentos* segmento_pcb = malloc(sizeof(t_pcb_segmentos));
        segmento_pcb->id_tabla_paginas = 0;
        segmento_pcb->tamanio_segmento = atoi(segmento);
        list_add(tabla_segmentos, segmento_pcb);
    }
    *lista_a_cargar = tabla_segmentos;
}

static void crear_colas_de_block_teclado_y_pantalla_por_proceso(char* key) {

    sem_t* mutex = malloc(sizeof(sem_t));
    sem_t* contador = malloc(sizeof(sem_t));
    sem_t* sem_hilo = malloc(sizeof(sem_t));

    sem_init(mutex, SEM_NOT_SHARE_BETWEEN_PROCESS, 1); 
	sem_init(contador, SEM_NOT_SHARE_BETWEEN_PROCESS, 0); 
	sem_init(sem_hilo, SEM_NOT_SHARE_BETWEEN_PROCESS, 0);

    pthread_mutex_lock(&mutex_dictionary_colas);
    dictionary_put(colas, key, queue_create());
	pthread_mutex_unlock(&mutex_dictionary_colas);

    pthread_mutex_lock(&mutex_dictionary_mutex_colas_block);
    dictionary_put(mutex_colas_block ,key, mutex);
	pthread_mutex_unlock(&mutex_dictionary_mutex_colas_block);

    pthread_mutex_lock(&mutex_dictionary_contador_colas_block);
    dictionary_put(contador_colas_block, key, contador);
	pthread_mutex_unlock(&mutex_dictionary_contador_colas_block);

    pthread_mutex_lock(&mutex_dictionary_sem_hilos_block);
    dictionary_put(sem_hilos_block, key, sem_hilo);
	pthread_mutex_unlock(&mutex_dictionary_sem_hilos_block);
}

static void procesar_conexion(void* void_args) {
    t_procesar_conexion_args* args = (t_procesar_conexion_args*) void_args;
    t_log* logger = args->log;
    uint32_t cliente_fd = args->fd;
    char* server_name = args->server_name;
    free(args);

    op_code codigo;
    t_list* instrucciones;
    t_list* segmentos;
    t_pcb* pcb_proceso;
    char* dato_recibido = string_new();
    char* id_proceso = string_new();

    while (cliente_fd != -1) {

        if (recv(cliente_fd, &codigo, sizeof(op_code), MSG_WAITALL) == -1) 
            codigo = -1;

        switch (codigo) {
            case CONSOLA_INSTRUCCIONES:
                recv_instrucciones(logger, cliente_fd, &instrucciones);
                break;

            case CONSOLA_SEGMENTOS:
                recv_segmentos(logger, cliente_fd, &segmentos);
                break;
            case CONSOLA_KERNEL_INIT_PCB:

                pthread_mutex_lock(&mutex_generador_pcb_id);
	            generador_pcb_id++;
	            pthread_mutex_unlock(&mutex_generador_pcb_id);
                
                pcb_proceso = malloc(sizeof(t_pcb));
                pcb_proceso->id_proceso = generador_pcb_id;
                pcb_proceso->program_counter = 0;
                carga_tabla_segmentos_pcb(&pcb_proceso->tabla_segmentos, segmentos);
                pcb_proceso->registro_AX = 0;
                pcb_proceso->registro_BX = 0;
                pcb_proceso->registro_CX = 0;
                pcb_proceso->registro_DX = 0;
                pcb_proceso->instrucciones = instrucciones;
                pcb_proceso->estado_anterior = PCB_NEW;
                pcb_proceso->estado_actual = PCB_NEW;
                pcb_proceso->consola_fd = cliente_fd;
                pcb_proceso->debe_ser_finalizado = false;
                pcb_proceso->debe_ser_bloqueado = false;
                pcb_proceso->puede_ser_interrumpido = false;
                pcb_proceso->fue_interrumpido = false;
                pcb_proceso->finaliza_por_segmentation_fault = false;
                pcb_proceso->finaliza_por_error_instruccion = false;
                pcb_proceso->finaliza_por_error_de_ejecucion = false;
                pcb_proceso->registro_para_bloqueo = 0;
                pcb_proceso->unidades_de_trabajo = 0;
                pcb_proceso->dispositivo_bloqueo = string_new();
                pcb_proceso->page_fault_segmento = 0;
                pcb_proceso->page_fault_pagina = 0;

                id_proceso = string_itoa(pcb_proceso->id_proceso);

                pthread_mutex_lock(&mutex_dictionary_dato_ingreso_por_teclado);
                dictionary_put(dato_ingreso_por_teclado, id_proceso, string_new());
	            pthread_mutex_unlock(&mutex_dictionary_dato_ingreso_por_teclado);

                crear_colas_de_block_teclado_y_pantalla_por_proceso(string_from_format("%d-PANTALLA",pcb_proceso->id_proceso));
                crear_colas_de_block_teclado_y_pantalla_por_proceso(string_from_format("%d-TECLADO",pcb_proceso->id_proceso));

                pthread_mutex_lock(&mutex_dictionary_threads_dinamicos);
                pthread_t* hilo_block_io_teclado = malloc(sizeof(pthread_t));
                pthread_create(hilo_block_io_teclado, NULL, (void*)hilo_planificador_block_teclado, (void*) string_from_format("%d-TECLADO",pcb_proceso->id_proceso));
	            pthread_detach(*hilo_block_io_teclado);
                dictionary_put(threads_dinamicos, string_from_format("%d-PANTALLA",pcb_proceso->id_proceso), hilo_block_io_teclado);
	            pthread_mutex_unlock(&mutex_dictionary_threads_dinamicos);

                pthread_mutex_lock(&mutex_dictionary_threads_dinamicos);
                pthread_t* hilo_block_io_pantalla = malloc(sizeof(pthread_t));
                pthread_create(hilo_block_io_pantalla, NULL, (void*)hilo_planificador_block_pantalla, (void*) string_from_format("%d-PANTALLA",pcb_proceso->id_proceso));
                pthread_detach(*hilo_block_io_pantalla);
                dictionary_put(threads_dinamicos, string_from_format("%d-TECLADO",pcb_proceso->id_proceso), hilo_block_io_pantalla);
	            pthread_mutex_unlock(&mutex_dictionary_threads_dinamicos);

                pthread_mutex_lock(&mutex_dictionary_threads_dinamicos);
                queue_push(cola_de_keys_de_diccionario, id_proceso);
                queue_push(cola_de_keys_de_diccionario, string_from_format("%d-PANTALLA",pcb_proceso->id_proceso));
                queue_push(cola_de_keys_de_diccionario, string_from_format("%d-TECLADO",pcb_proceso->id_proceso));
	            pthread_mutex_unlock(&mutex_dictionary_threads_dinamicos);

                meter_proceso_en_new(pcb_proceso);
                break;
            case CONSOLA_EXIT:
                sem_post(&sem_finalizar_proceso);
                liberar_conexion(&cliente_fd);
                break;
            case CONSOLA_PANTALLA:
                recv_fin_mostrar_dato_en_pantalla_from_consola(logger, cliente_fd);
                sem_post(&sem_fin_io_pantalla);
                break;
            case CONSOLA_TECLADO:
                recv_dato_ingresado_por_teclado_from_consola(logger, cliente_fd, &dato_recibido);
                log_info(logger,"Dato ingresado por teclado: %s", dato_recibido);
                pthread_mutex_lock(&mutex_dictionary_dato_ingreso_por_teclado);
                dictionary_put(dato_ingreso_por_teclado, id_proceso, dato_recibido);
	            pthread_mutex_unlock(&mutex_dictionary_dato_ingreso_por_teclado);
                sem_post(&sem_dato_por_teclado_ingresado);
                break;
            default:
                break;
        }
    }

    log_warning(logger, "El cliente se desconecto de %s server", server_name);
    return;
}

int server_escuchar(t_log* logger, char* server_name, int server_socket) {
    uint32_t cliente_socket = esperar_cliente(logger, server_name, server_socket);

    if (cliente_socket != -1) {
        pthread_t hilo;
        t_procesar_conexion_args* args = malloc(sizeof(t_procesar_conexion_args));
        args->log = logger;
        args->fd = cliente_socket;
        args->server_name = server_name;
        pthread_mutex_lock(&mutex_dictionary_threads_dinamicos);
        pthread_create(&hilo, NULL, (void*) procesar_conexion, (void*) args);
        pthread_detach(hilo);
        pthread_mutex_unlock(&mutex_dictionary_threads_dinamicos);
        return 1;
    }
    return 0;
}