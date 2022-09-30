#include "include/kernel.h"

int main(int argc, char** argv){

    iniciar_programa();

    // Leer archivo de configuracion
    config_kernel = malloc(sizeof(t_config_kernel));
    if(!cargar_config(config_kernel)) {
        log_error(logger,"No se pudo abrir el archivo de configuracion kernel.config");
        terminar_programa();
        return EXIT_FAILURE;
    }

    terminar_programa();

    return EXIT_SUCCESS;
}