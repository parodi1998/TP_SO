#ifndef CONSOLA_CONEXION_H_
#define CONSOLA_CONEXION_H_

#include "commons.h"

bool conectar_a_kernel(int* fd, t_config_consola* config_consola);
bool send_instrucciones(t_log* logger, int fd, t_list* instrucciones);
bool send_segmentos(t_log* logger, int fd, t_list* segmentos);
void recv_finalizar_consola_from_kernel(t_log* logger, int fd, char** mensaje);
bool send_finalizar_consola_from_consola(t_log* logger, int fd);

#endif