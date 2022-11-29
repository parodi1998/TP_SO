#include "../include/conexion.h"

bool iniciar_kernel(int* fd) {
     *fd = iniciar_servidor(logger, config_kernel->nombre_escucha,config_kernel->ip_escucha, config_kernel->puerto_escucha);
    return fd != 0;
}