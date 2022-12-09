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
	string_to_char_list(logger, response, instrucciones);
}

void recv_segmentos(t_log* logger, int fd, t_list** segmentos) {
	uint32_t size;
	char* response = recibir_buffer(&size, fd);
	string_to_char_list(logger, response, segmentos);
}

bool send_finalizar_consola_ok_from_kernel(t_log* logger, int fd) {
	bool respuesta = enviar_mensaje_bool(CONSOLA_EXIT, "Finaliza OK", fd);
	return respuesta;
}

bool send_finalizar_consola_error_segmentation_fault_from_kernel(t_log* logger, int fd) {
	bool respuesta = enviar_mensaje_bool(CONSOLA_EXIT, "Error: Segmentation Fault (SIGSEGV)", fd);
	return respuesta;
}

bool send_finalizar_consola_error_instruccion_from_kernel(t_log* logger, int fd) {
	bool respuesta = enviar_mensaje_bool(CONSOLA_EXIT, "Error: instruccion no identificada", fd);
	return respuesta;
}

bool send_finalizar_consola_error_comunicacion_from_kernel(t_log* logger, int fd) {
	bool respuesta = enviar_mensaje_bool(CONSOLA_EXIT, "Error: no se pudo realizar la operacion", fd);
	return respuesta;
}

void recv_finalizar_consola_from_consola(t_log* logger, int fd) {
	uint32_t size;
	char* response = recibir_buffer(&size, fd);
	free(response);
}

bool send_mostrar_dato_en_pantalla_from_kernel(t_log* logger, int fd, char* dato) {
	bool respuesta = enviar_mensaje_bool(CONSOLA_PANTALLA, dato, fd);
	return respuesta;
}

void recv_fin_mostrar_dato_en_pantalla_from_consola(t_log* logger, int fd) {
	uint32_t size;
	char* response = recibir_buffer(&size, fd);
	free(response);
}

bool send_interrumpir_cpu_from_kernel(t_log* logger, int fd) {
	bool respuesta = send_op_code(logger, fd, PCB_INTERRUPT);
	return respuesta;
}

bool send_ingresar_dato_por_teclado_from_kernel(t_log* logger, int fd) {
	bool respuesta = send_op_code(logger, fd, CONSOLA_TECLADO);
	return respuesta;
}

void recv_dato_ingresado_por_teclado_from_consola(t_log* logger, int fd, char** mensaje) {
	uint32_t size;
	char* response = recibir_buffer(&size, fd);
	*mensaje = response;
}
