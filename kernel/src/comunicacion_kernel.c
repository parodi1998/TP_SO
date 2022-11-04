#include "../include/comunicacion_kernel.h"

static void procesar_conexion(void* void_args) {
    t_procesar_conexion_args* args = (t_procesar_conexion_args*) void_args;
    t_log* logger = args->log;
    int cliente_fd = args->fd;
    char* server_name = args->server_name;
    free(args);

    op_code codigo;
    t_list lista_instrucciones=new list();

    while (cliente_fd != -1) {

        if(!recibir_operacion(&codigo, cliente_fd)){
            log_error(logger, "Hubo un error al recibir el codigo de operacion");
            return;
        }

        switch (codigo) {
            case MENSAJE:
                recibir_mensaje(logger, cliente_fd);
                break;
            case -1:
                log_error(logger, "el cliente se desconecto. Terminando servidor");
                return;
            default:
                log_warning(logger,"Operacion desconocida. No quieras meter la pata");
                break;
        }
    }
    
    t_pcb* pcb_proceso = malloc(sizeof(pcb_t));
    pcb_proceso->id_proceso=contador;//hay que sumarle 1 en cada instruccion cuando hagas el que recibe la lista
    pcb_proceso->program_counter=0;
    //pcb_proceso->tabla_segmentos=pedir tabla;
    //pcb_proceso->registros_cpu=buscar registros;
    pcb_proceso->instrucciones=lista_instrucciones;

    procesoANew(pcb_proceso);

    log_warning(logger, "El cliente se desconecto de %s server", server_name);
    return;
}

int server_escuchar(t_log* logger, char* server_name, int server_socket) {
    int cliente_socket = esperar_cliente(logger, server_name, server_socket);

    if (cliente_socket != -1) {
        pthread_t hilo;
        t_procesar_conexion_args* args = malloc(sizeof(t_procesar_conexion_args));
        args->log = logger;
        args->fd = cliente_socket;
        args->server_name = server_name;
        pthread_create(&hilo, NULL, (void*) procesar_conexion, (void*) args);
        pthread_detach(hilo);
        return 1;
    }
    return 0;
}

void enviar_pcb_cpu(t_pcb* pcb_proceso){

	send_pid(fd_cpu,pcb_proceso->id_proceso);
    send_instrucciones(fd_cpu,pcb_proceso->instrucciones);
	send_PC(fd_cpu,pcb_proceso->program_counter);
	send_registroAX(fd_cpu,pcb_proceso->registro_AX);
	send_registroBX(fd_cpu,pcb_proceso->registro_BX);
	send_registroCX(fd_cpu,pcb_proceso->registro_CX);
	send_registroDX(fd_cpu,pcb_proceso->registro_DX);
	send_indice_tabla_paginas(fd_cpu,pcb_proceso->tabla_segmentos);
	
	list_clean(lista_instrucciones_kernel);
    }

void pedir_tabla_a_memoria(){
	send_TAM(fd_memoria,config_get_string_value(config_kernel,"PUERTO_MEMORIA"));
}