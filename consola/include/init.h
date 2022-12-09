#ifndef CONSOLA_INIT_H_
#define CONSOLA_INIT_H_

#include "commons.h"

void iniciar_programa();
void iniciar_logger();
bool cargar_config(t_config_consola* config_consola, char* path);

void terminar_programa();

#endif