#ifndef KERNEL_CONEXION_H_
#define KERNEL_CONEXION_H_

#include "commons.h"

bool iniciar_kernel(int* fd);
void recv_segmentos(t_log* logger, int fd, t_list** segmentos);
void recv_instrucciones(t_log* logger, int fd, t_list** instrucciones);

#endif