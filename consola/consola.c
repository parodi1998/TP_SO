#include "include/consola.h"

int main(int argc, char** argv){

    iniciar_programa();

    if(argc < 3) {
        log_error(logger,"La cantidad de argumentos ingresados es incorrecta");
        log_warning(logger,"Ejemplo de inicializacion: ./consola.out consola.config pseudo_codigo_1.txt");
        terminar_programa();
        return EXIT_FAILURE;
    }

    if(argc > 3) {
        log_warning(logger,"La cantidad de argumentos ingresados excede lo necesario, solo seran utilizados los primeros 2 parametros");
        log_warning(logger,"Archivo de configuracion de la consola: %s", argv[1]);
        log_warning(logger,"Direccion archivo de pseudocodigo: %s", argv[2]);
    }

    char* config_path = argv[1];
    char* pseudocodigo_path = argv[2];

    // Leer archivo de configuracion
    config_consola = malloc(sizeof(t_config_consola));
    if(!cargar_config(config_consola, config_path)) {
        log_error(logger,"No se pudo abrir el archivo de configuracion consola.config");
        terminar_programa();
        return EXIT_FAILURE;
    }

    // conectarse al kernel y enviar informacion
    int fd = 0;
    if(!conectar_a_kernel(&fd, config_consola)) {
        log_error(logger,"No se pudo generar la conexion al kernel");
        terminar_programa();
        return EXIT_FAILURE;
    }

    enviar_mensaje("mensaje de prueba", fd);

    liberar_conexion(&fd);
    terminar_programa();

    return EXIT_SUCCESS;
}