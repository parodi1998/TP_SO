#ifndef KERNEL_INIT_H_
#define KERNEL_INIT_H_

#include "commons.h"

void iniciar_programa();
void iniciar_logger();
bool cargar_config(t_config_kernel* config_kernel);

void terminar_programa();

#endif