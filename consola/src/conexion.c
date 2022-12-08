#include "../include/conexion.h"

bool conectar_a_kernel(int* fd, t_config_consola* config_consola) {
    *fd = crear_conexion(logger, "CONSOLA_A_KERNEL", config_consola->ip_kernel, config_consola->puerto_kernel);
    return fd != 0;
}

static char* char_list_to_string(t_log* logger, t_list* instrucciones) {
	char* respuesta = string_new();
	for(int i = 0; i < list_size(instrucciones); i++) {
		string_append(&respuesta, list_get(instrucciones, i));
		if(i < list_size(instrucciones) - 1) {
			string_append(&respuesta, "|");
		}
	} 
	return respuesta;
}

bool send_instrucciones(t_log* logger, int fd, t_list* instrucciones) {
	char* mensaje = char_list_to_string(logger, instrucciones);
	log_info(logger,"REQUEST CONSOLA_INSTRUCCIONES: %s" , mensaje );
	bool respuesta = enviar_mensaje_bool(CONSOLA_INSTRUCCIONES, mensaje, fd);
	log_info(logger,"RESPONSE CONSOLA_INSTRUCCIONES: %d" , respuesta );
	free(mensaje);
	return respuesta;
}

bool send_segmentos(t_log* logger, int fd, t_list* segmentos) {
	char* mensaje = char_list_to_string(logger, segmentos);
	log_info(logger,"REQUEST CONSOLA_SEGMENTOS: %s" , mensaje );
	bool respuesta = enviar_mensaje_bool(CONSOLA_SEGMENTOS, mensaje, fd);
	log_info(logger,"RESPONSE CONSOLA_SEGMENTOS: %d" , respuesta );
	free(mensaje);
	return respuesta;
}