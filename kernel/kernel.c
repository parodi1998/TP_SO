#include "include/kernel.h"

void inicializar_listas(){

	colaNew = queue_create();
	colaReady = list_create();
	listaExit = list_create();
	lista_instrucciones_kernel = list_create();

}

int main(int argc, char** argv){

    inicializar_listas();

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
    while (server_escuchar(logger, "kernel", kernel_server_fd));    

    liberar_conexion(&kernel_server_fd);
    terminar_programa();

    return EXIT_SUCCESS;
}