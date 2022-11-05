#include "../include/comunicacion_kernel.h"

static void procesar_conexion(void* void_args) {
    t_procesar_conexion_args* args = (t_procesar_conexion_args*) void_args;
    t_log* logger = args->log;
    int cliente_fd = args->fd;
    char* server_name = args->server_name;
    free(args);

    op_code codigo;
    t_list* instrucciones;
    char* segmentos;
    size_t cantidad_instrucciones;
    t_pcb* pcb;
    t_pcb* pcb_proceso;

    while (cliente_fd != -1) {

        if(!recibir_operacion(&codigo, cliente_fd)){
            contador++;
            pcb_proceso = malloc(sizeof(t_pcb));
            pcb_proceso->id_proceso = contador;
            pcb_proceso->program_counter = 0;
            //pcb_proceso->tabla_segmentos=pedir tabla;
            //pcb_proceso->registros_cpu=buscar registros;
            pcb_proceso->instrucciones = instrucciones;

            procesoANew(pcb_proceso);
            break;
            
        }

        switch (codigo) {
            case INSTRUCCIONES:
                log_info(logger, "Estas en la opcion de recibir INSTRUCCIONES");
                if(!recv_instrucciones(cliente_fd, &instrucciones)) {
                    log_error(logger,"Hubo un error al recuperar la lista de instrucciones");
                }else {
                    log_info(logger,"Se recibio la informacion con exito");
                    cantidad_instrucciones = list_size(instrucciones);
                    for(size_t i = 0; i < cantidad_instrucciones; i++) {
                        char* instruccion = list_get(instrucciones,i);
                        log_info(logger,instruccion);
                    }
                    log_info(logger,"");
                }
                break;
            case PCB_KERNEL: 
                log_info(logger, "Estas en la opcion de recibir un PCB");
                    if(!recv_pcb(cliente_fd, &pcb)) {
                        log_error(logger,"Hubo un error al recuperar el pcb");
                    }else {
                        log_info(logger,"Se recibio la informacion con exito");
                        log_info(logger,"PCB - ID: %d", pcb->id_proceso);
                        log_info(logger,"PCB - PROGRAM COUNTER: %d", pcb->program_counter);
                        log_info(logger,"PCB - TABLA DE SEGMENTOS: %d", pcb->tabla_segmentos);
                        log_info(logger,"PCB - REGISTRO AX: %d", pcb->registro_AX);
                        log_info(logger,"PCB - REGISTRO BX: %d", pcb->registro_BX);
                        log_info(logger,"PCB - REGISTRO CX: %d", pcb->registro_CX);
                        log_info(logger,"PCB - REGISTRO DX: %d", pcb->registro_DX);
                        cantidad_instrucciones = list_size(pcb->instrucciones);
                        for(size_t i = 0; i < cantidad_instrucciones; i++) {
                            char* instruccion = list_get(pcb->instrucciones,i);
                            log_info(logger,instruccion);
                        }
                        log_info(logger,"");
                    }
                break;
            case -1:
                log_error(logger, "el cliente se desconecto. Terminando servidor");
                return;
            default:
                log_error(logger, "Algo anduvo mal en el server de %s", server_name);
                log_info(logger, "Cop: %d", codigo);
                break;
        }
    }

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