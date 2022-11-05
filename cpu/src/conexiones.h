#ifndef CONEXIONES_H_
#define CONEXIONES_H_

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<semaphore.h>
#include "server.h"
#include "client.h"

#include "client_memoria.h"
#include "instrucciones.h"



t_log* iniciar_logger(void);
void leer_consola();
void paquete(int);
void terminar_programa(int, t_log*, t_config*);
int start(void);
int crearConexionAuxiliar(char* ip, int puerto, t_log* logger);

#endif /* CONEXIONES_H_ */
