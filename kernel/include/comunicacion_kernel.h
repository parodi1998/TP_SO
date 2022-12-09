#ifndef COMUNICACION_KERNEL_H_
#define COMUNICACION_KERNEL_H_

#include "commons.h"
#include "planificacion.h"
#include "conexion.h"

extern uint32_t generador_pcb_id;
int server_escuchar(t_log* logger, char* server_name, int server_socket);

#endif