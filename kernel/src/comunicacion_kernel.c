#include "../include/comunicacion_kernel.h"

static void procesar_conexion(void* void_args) {
    t_procesar_conexion_args* args = (t_procesar_conexion_args*) void_args;
    t_log* logger = args->log;
    int cliente_fd = args->fd;
    char* server_name = args->server_name;
    free(args);

    op_code codigo;

    //t_pcb* pcb;

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