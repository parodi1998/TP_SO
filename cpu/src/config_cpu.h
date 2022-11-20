/*
 * config.h
 *
 *  Created on: 2 nov. 2022
 *      Author: utnso
 */

#ifndef CONFIG_CPU_H_
#define CONFIG_CPU_H_

#include <stdint.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>

void iniciar_config_cpu(void);

t_config* get_config();
int get_entradas_tlb();
char* get_reemplazo_tlb();
int get_retardo_instruccion();
char* get_ip_memoria();
char* get_puerto_memoria();
char* get_puerto_memoria_dispatch();
char* get_puerto_escucha_interrupt();
char* get_log_path();

#endif /* CONFIG_CPU_H_ */
