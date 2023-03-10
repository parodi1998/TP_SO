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


#include "instrucciones.h"
#include "conexiones_cpu.h"

#include "config_cpu.h"
#include "../../shared/include/protocolo.h"

#include <commons/collections/list.h>

extern int fd_server_cpu_dispatch;
extern int fd_server_cpu_interrupt;
extern int fd_client_kernel_dispatch;
extern int fd_client_kernel_interrupt; 

t_log* iniciar_logger(void);
void leer_consola();
void paquete(int);
void terminar_programa(int, t_log*, t_config*);
int start();
void comunicacion_cpu_kernel_distpach();
int crearConexionAuxiliar(char* ip, int puerto, t_log* logger);
t_contexto_ejecucion iniciar_proceso(t_pcb*);

#endif /* CONEXIONES_H_ */
