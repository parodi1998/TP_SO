#ifndef CLIENT_H_
#define CLIENT_H_

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<semaphore.h>

#include "utils.h"



t_log* iniciar_logger(void);
t_config* iniciar_config(void);
void leer_consola();
void paquete(int);
void terminar_programa(int, t_log*, t_config*);
int start(int arg);
cpu_config* cargarConfiguracion(t_config*, t_log*);
int crearConexionAuxiliar(char* ip, int puerto, t_log* logger);

#endif /* CLIENT_H_ */
