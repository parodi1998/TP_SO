#include "include/kernel.h"

int main(int argc, char** argv){

    if(!iniciar_programa()) {
        terminar_programa();
        return EXIT_SUCCESS;
    }

    int kernel_server_fd = 0;
    if(!iniciar_kernel(&kernel_server_fd)) {
        log_error(logger,"No se pudo generar iniciar el proceso kernel");
        terminar_programa();
        return EXIT_FAILURE;
    }

	log_info(logger, "Servidor listo para recibir al cliente");
	int cliente_fd = esperar_cliente(logger, "KERNEL_A_CONSOLA", kernel_server_fd);

    op_code codigo;
    if(!recibir_operacion(&codigo, cliente_fd)){
        log_error(logger, "Hubo un error al recibir el codigo de operacion");
        liberar_conexion(&kernel_server_fd);
        liberar_conexion(&cliente_fd);
        terminar_programa();
        return EXIT_FAILURE;
    }

    switch (codigo) {
        case MENSAJE:
            recibir_mensaje(logger, cliente_fd);
            break;
        case -1:
            log_error(logger, "el cliente se desconecto. Terminando servidor");
            return EXIT_FAILURE;
        default:
            log_warning(logger,"Operacion desconocida. No quieras meter la pata");
            break;
    }

    liberar_conexion(&kernel_server_fd);
    liberar_conexion(&cliente_fd);
    terminar_programa();

    return EXIT_SUCCESS;
}