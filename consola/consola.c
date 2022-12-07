#include "include/consola.h"

t_log* logger;
t_config_consola* config_consola;

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

    // Leer archivo
    t_list* instrucciones;

    FILE* programa_con_instrucciones;
    programa_con_instrucciones = fopen(pseudocodigo_path,"r");
    if (programa_con_instrucciones == NULL) {
        log_error(logger, "No se pudo abrir el archivo del path %s",pseudocodigo_path);
        terminar_programa();
        return EXIT_FAILURE;
    }
 	else {
        instrucciones = cargar_instrucciones_de_archivo(programa_con_instrucciones);
    }
    fclose(programa_con_instrucciones);

    // No se que onda, pero si saco esta linea que sigue, no se mandan bien las instrucciones
    uint32_t cant_inst = list_size(instrucciones);

    // conectarse al kernel y enviar informacion
    int fd = 0;
    if(!conectar_a_kernel(&fd, config_consola)) {
        log_error(logger,"No se pudo generar la conexion al kernel");
        terminar_programa();
        return EXIT_FAILURE;
    }

    t_list* segmentos = config_consola->segmentos;

    if(!send_instrucciones(logger, fd, instrucciones)) {
        log_error(logger,"Hubo un error enviando las instrucciones al kernel");
        liberar_conexion(&fd);
        terminar_programa();
        return EXIT_SUCCESS;
    } else {
        log_info(logger,"La informacion fue enviada con exito al kernel");
    }

    if(!send_segmentos(logger, fd, segmentos)) {
        log_error(logger,"Hubo un error enviando los segmentos al kernel");
        liberar_conexion(&fd);
        terminar_programa();
        return EXIT_SUCCESS;
    } else {
        log_info(logger,"La informacion fue enviada con exito al kernel");
    }

    if(!send_op_code(logger, fd, CONSOLA_KERNEL_INIT_PCB)) {
        log_error(logger,"Hubo un error enviando el op code: CONSOLA_KERNEL_INIT_PCB");
        liberar_conexion(&fd);
        terminar_programa();
        return EXIT_SUCCESS;
    } else {
        log_info(logger,"La informacion fue enviada con exito al kernel");
    }

    op_code cod_op;
    bool mantener_conexion = true;
    uint32_t size = 0;

    while(mantener_conexion) {

        if(recv_op_code(logger, fd, &cod_op))
			cod_op = -1;

        switch(cod_op) {
            case CONSOLA_PANTALLA:
                break;
            case CONSOLA_TECLADO:
                break;
            case CONSOLA_EXIT:
                if(send_op_code(logger, fd, CONSOLA_EXIT)) {
                    log_info(logger,"se envio el op code con exito");
                } else {
                    log_info(logger,"hubo un error al devolver el op code");
                }
                mantener_conexion = false;
                break;
            default:
                break;

        }
    }

    liberar_conexion(&fd);
    terminar_programa();

    return EXIT_SUCCESS;
}