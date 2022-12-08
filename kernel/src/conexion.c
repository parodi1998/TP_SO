#include "../include/conexion.h"

bool iniciar_kernel(int* fd) {
     *fd = iniciar_servidor(logger, config_kernel->nombre_escucha,config_kernel->ip_escucha, config_kernel->puerto_escucha);
    return fd != 0;
}

static void string_to_char_list(t_log* logger, char* string, t_list** lista_destino) {
	char** arr = string_split(string,"|");

	t_list* list = list_create();
    int index;
    for(index = 0; arr[index] != NULL; index++) {
        list_add(list, arr[index]);
    }
    
	*lista_destino = list;
}

void recv_instrucciones(t_log* logger, int fd, t_list** instrucciones) {
	uint32_t size;
	char* response = recibir_buffer(&size, fd);
	log_info(logger,"RECIBO CONSOLA_INSTRUCCIONES: %s" , response);
	string_to_char_list(logger, response, instrucciones);
}

void recv_segmentos(t_log* logger, int fd, t_list** segmentos) {
	uint32_t size;
	char* response = recibir_buffer(&size, fd);
	log_info(logger,"RECIBO CONSOLA_SEGMENTOS: %s" , response);
	string_to_char_list(logger, response, segmentos);
}