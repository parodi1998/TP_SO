#ifndef SERVER_UTILS_H_
#define SERVER_UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<string.h>
#include<assert.h>
#include "shared_commons.h"

void* recibir_buffer(int*, int);
t_list* recibir_paquete(int);
void recibir_mensaje(t_log*,int);
bool recibir_operacion(op_code* codigo, int socket_cliente);

#endif