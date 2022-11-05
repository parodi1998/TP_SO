#include "include/consola.h"

// funcion de prueba, despues borrarla
static t_pcb* create_pcb(t_list* instrucciones) {
    t_pcb* pcb = malloc(sizeof(t_pcb));
    pcb->id_proceso = 1;
    pcb->instrucciones = instrucciones;
    pcb->program_counter = 1;
    pcb->tabla_segmentos = 2;
    pcb->registro_AX = 3;
    pcb->registro_BX = 4;
    pcb->registro_CX = 5;
    pcb->registro_DX = 6;
    pcb->estado = PCB_NEW;
    return pcb;
}

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

    char* segmentos = config_consola->segmentos;

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
    size_t cant_inst = list_size(instrucciones);

    // conectarse al kernel y enviar informacion
    int fd = 0;
    if(!conectar_a_kernel(&fd, config_consola)) {
        log_error(logger,"No se pudo generar la conexion al kernel");
        terminar_programa();
        return EXIT_FAILURE;
    }

    if(!send_instrucciones(fd, instrucciones)) {
        log_error(logger,"Hubo un error enviando la informacion al kernel");
    } else {
        log_info(logger,"La informacion fue enviada con exito al kernel");
    }

    t_pcb* pcb = create_pcb(instrucciones);

    if(!send_pcb(fd, pcb)) {
        log_error(logger,"Hubo un error enviando el pcb al kernel");
    } else {
        log_info(logger,"El pcb fue enviada con exito al kernel");
    }

    liberar_conexion(&fd);
    terminar_programa();

    return EXIT_SUCCESS;
}