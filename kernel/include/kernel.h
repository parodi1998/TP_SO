#ifndef KERNEL_MAIN_H_
#define KERNEL_MAIN_H_

#include "commons.h"
#include "init.h"
#include "conexion.h"
#include "comunicacion_kernel.h"
#include "planificacion.h"
#include <signal.h>

void inicializar_diccionario();
void inicializar_semaforos();
void inicializar_planificadores();

void destruir_diccionario();
void destruir_semaforos();

void inicializar_todo();
void destruir_todo();

void signalHandle(int signal);

#endif
