#include "../include/conexion.h"

bool conectar_a_kernel(int* fd, t_config_consola* config_consola) {
    *fd = crear_conexion(logger, "CONSOLA_A_KERNEL", config_consola->ip_kernel, config_consola->puerto_kernel);
    return fd != 0;
}