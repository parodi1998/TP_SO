#include "config_cpu.h"

int entradas_tlb;
char* reemplazo_tlb;
int retardo_instruccion;
char* ip_memoria;
char* puerto_memoria;
char* puerto_memoria_dispatch;
char* puerto_escucha_interrupt;
char* log_path;


t_log* LOGGER;

t_config* CONFIG;


void iniciar_config_cpu(void){

	CONFIG = config_create("cpu.config");

	entradas_tlb = config_get_int_value(CONFIG,"ENTRADAS_TLB");
	reemplazo_tlb = config_get_string_value(CONFIG,"REEMPLAZO_TLB");
	retardo_instruccion = config_get_int_value(CONFIG,"RETARDO_INSTRUCCION");
	ip_memoria =  config_get_string_value(CONFIG,"IP_MEMORIA");
	puerto_memoria = config_get_string_value(CONFIG,"PUERTO_MEMORIA");
	puerto_memoria_dispatch = config_get_string_value(CONFIG,"PUERTO_ESCUCHA_DISPATCH");
	puerto_escucha_interrupt = config_get_string_value(CONFIG,"PUERTO_ESCUCHA_INTERRUPT");
	log_path = config_get_string_value(CONFIG,"LOG_PATH");


	LOGGER = log_create(log_path, "cpu", true, LOG_LEVEL_INFO);
}

t_config* get_config(){
	return CONFIG;
}

t_log* get_log(){
	return LOGGER;
}

int get_entradas_tlb(){
	return entradas_tlb;
}
char* get_reemplazo_tlb(){
	return reemplazo_tlb;
}
int get_retardo_instruccion(){
	return retardo_instruccion;
}
char* get_ip_memoria(){
	return ip_memoria;
}
char* get_puerto_memoria(){
	return puerto_memoria;
}
char* get_puerto_memoria_dispatch(){
	return puerto_memoria_dispatch;
}
char* get_puerto_escucha_interrupt(){
	return puerto_escucha_interrupt;
}
char* get_log_path(){
	return log_path;
}
