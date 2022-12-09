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
	bool respuesta = enviar_mensaje_bool(CONSOLA_INSTRUCCIONES, mensaje, fd);
	free(mensaje);
	return respuesta;
}

bool send_segmentos(t_log* logger, int fd, t_list* segmentos) {
	char* mensaje = char_list_to_string(logger, segmentos);
	bool respuesta = enviar_mensaje_bool(CONSOLA_SEGMENTOS, mensaje, fd);
	free(mensaje);
	return respuesta;
}

void recv_finalizar_consola_from_kernel(t_log* logger, int fd, char** mensaje) {
	uint32_t size;
	char* response = recibir_buffer(&size, fd);
	*mensaje = response;
}

void recv_mostrar_dato_en_pantalla_from_kernel(t_log* logger, int fd, char** mensaje) {
	uint32_t size;
	char* response = recibir_buffer(&size, fd);
	*mensaje = response;
}

bool send_fin_mostrar_dato_en_pantalla_from_consola(t_log* logger, int fd) {
	bool respuesta = enviar_mensaje_bool(CONSOLA_PANTALLA, "OK", fd);
	return respuesta;
}


bool send_finalizar_consola_from_consola(t_log* logger, int fd) {
	bool respuesta = enviar_mensaje_bool(CONSOLA_EXIT, "", fd);
	return respuesta;
}

bool send_dato_ingresado_por_teclado_from_consola(t_log* logger, int fd, char* dato) {
	bool respuesta = enviar_mensaje_bool(CONSOLA_TECLADO, dato, fd);
	return respuesta;
}