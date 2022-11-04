#include "../include/conexion.h"

bool iniciar_kernel(int* fd) {
     *fd = iniciar_servidor(logger, config_kernel->nombre_escucha,config_kernel->ip_escucha, config_kernel->puerto_escucha);
    return fd != 0;
}

void conexion_kernel_a_memoria(){

	char* ip_memoria = config_get_string_value(config_kernel,"IP_MEMORIA");
	char* puerto_memoria = config_get_string_value(config_kernel,"PUERTO_MEMORIA");


	fd_memoria=0;
		if (!generar_conexion_kernel_a_memoria(log_kernel, ip_memoria, puerto_memoria, &fd_memoria)) {
			cerrar_programa4(log_kernel);
			//return EXIT_FAILURE;
		}
		log_trace(log_kernel,"El fd_memoria despues de grar conexiones es: %d",fd_memoria);

}