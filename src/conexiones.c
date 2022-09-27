#include "conexiones.h"
.h"

char * concatenar(char* palabra1, char* palabra2)
{
	 char *palabraFinal = string_new();
	string_append(&palabraFinal, palabra1);
	string_append(&palabraFinal, palabra2);
	return palabraFinal;
}

cpu_config* cargarConfiguracion(t_config* rutaConfiguracion){
	cpu_config* configuracion_Cpu = malloc(sizeof(cpu_Config));
	
	configuracion_Cpu->entrada = malloc(sizeof(int));
	configuracion_Cpu->entrada = config_get_int_value(rutaConfiguracion, "ENTRADAS_TLB");
	
	configuracion_Cpu->reemplazo = malloc(sizeof(int));
	configuracion_Cpu->reemplazo = config_get_string_value(rutaConfiguracion, "REEMPLAZO_TLB");
	
	configuracion_Cpu->retardo = malloc(sizeof(int));
	configuracion_Cpu->retardo = config_get_int_value(rutaConfiguracion, "RETARDO_INSTRUCCION");
	
	configuracion_Cpu->ip_memoria = malloc(sizeof(int));
	configuracion_Cpu->ip_memoria = config_get_string_value(rutaConfiguracion, "IP_MEMORIA");
	
	configuracion_Cpu->puerto_memoria = malloc(sizeof(int));
	configuracion_Cpu->puerto_memoria = config_get_int_value(rutaConfiguracion, "PUERTO_MEMORIA");
	
	configuracion_Cpu->puerto_escucha_dispatch = malloc(sizeof(int));
	configuracion_Cpu->puerto_escucha_dispatch = config_get_int_value(rutaConfiguracion, "PUERTO_ESCUCHA_DISPATCH");
	
	configuracion_Cpu->puerto_escucha_interrupt = malloc(sizeof(int));
	configuracion_Cpu->puerto_escucha_interrupt = config_get_int_value(rutaConfiguracion, "PUERTO_ESCUCHA_INTERRUPT");
	
	return configuracion_Cpu;
}

int start(void)
{
	
	int conexion_Memoria;
	cpu_config* configuracion_Cpu;
	
	char* valor;
	t_log* logger;
	t_config* config;

	/* ---------------- LOGGING ---------------- */
	logger = iniciar_logger();

	log_info(logger, "Se inició el logger");

//	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

	config = iniciar_config();
	log_info(logger, "Se cargó la configuración");
	configuracion_Cpu = cargarConfiguracion(config);
	log_info(logger, "Se cargó la configuración: \n ENTRADAS_TLB: %d; \n REEMPLAZO_TLB: %s; \n RETARDO_INSTRUCCION: &d; \n IP MEMORIA: %s; PUERTO MEMORIA: %d \n PUERTO_ESCUCHA_DISPATCH: &d; \n PUERTO_ESCUCHA_INTERRUPT: %d", configuracion_Cpu->entrada, configuracion_Cpu->reemplazo, configuracion_Cpu->retardo, configuracion_Cpu->ip_memoria, configuracion_Cpu->puerto_memoria, configuracion_Cpu->puerto_escucha_dispatch, configuracion_Cpu->puerto_escucha_interrupt);

	//Conectar a memoria
	conexion_Memoria = crear_conexion(configuracion_Cpu->ip_memoria, configuracion_Cpu->puerto_memoria);

	// Enviamos al servidor el valor de CLAVE como mensaje

	// Armamos y enviamos el paquete
	paquete(conexion);

	terminar_programa(conexion, logger, config);

	/*---------------------------------------------------PARTE 5-------------------------------------------------------------*/
	// Proximamente
	return 0;
}

t_log* iniciar_logger(void)
{
	t_log* nuevo_logger;

	if ((nuevo_logger = log_create("tp0.log", "TP0", 1, LOG_LEVEL_INFO)) == NULL)
	{
		printf("Error al crear logger\n");
		exit(1);
	}
	return nuevo_logger;
}

t_config* iniciar_config(void)
{
	t_config* nuevo_config;
	char *current_dir = getcwd(NULL, 0);
	char* path = getcwd(NULL, 0);
	path = concatenar(path, "/Tp0.config");

	if ((nuevo_config = config_create(path)) == NULL)
	{
		printf("No pude leer la configuración\n");
		exit(1);
	}
	return nuevo_config;
}



void paquete(int conexion)
{
	// Ahora toca lo divertido!
	char* leido;
	t_paquete* paquete;

	// Leemos y esta vez agregamos las lineas al paquete


	// ¡No te olvides de liberar las líneas y el paquete antes de regresar!
	
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	/* Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config) 
	  con las funciones de las commons y del TP mencionadas en el enunciado */
	if (logger != NULL)
	{
		log_destroy(logger);
	}
	if (config != NULL)
	{
		config_destroy(config);
	}

}
