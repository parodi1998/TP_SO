#ifndef KERNEL_CONEXION_H_
#define KERNEL_CONEXION_H_

#include "commons.h"

bool iniciar_kernel(int* fd);
void recv_segmentos(t_log* logger, int fd, t_list** segmentos);
void recv_instrucciones(t_log* logger, int fd, t_list** instrucciones);
void recv_finalizar_consola_from_consola(t_log* logger, int fd);
bool send_finalizar_consola_error_comunicacion_from_kernel(t_log* logger, int fd);
bool send_finalizar_consola_error_instruccion_from_kernel(t_log* logger, int fd);
bool send_finalizar_consola_error_segmentation_fault_from_kernel(t_log* logger, int fd);
bool send_finalizar_consola_ok_from_kernel(t_log* logger, int fd);
bool send_mostrar_dato_en_pantalla_from_kernel(t_log* logger, int fd, char* dato);
void recv_fin_mostrar_dato_en_pantalla_from_consola(t_log* logger, int fd);

#endif