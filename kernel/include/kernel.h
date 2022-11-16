#ifndef KERNEL_MAIN_H_
#define KERNEL_MAIN_H_

#include "commons.h"
#include "init.h"
#include "conexion.h"
#include "comunicacion_kernel.h"
#include "planificacion.h"

void inicializar_listas();
void inicializar_semaforos();
void inicializar_planificadores();

#endif