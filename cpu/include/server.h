/*
 * server.h
 *
 *  Created on: Oct 5, 2022
 *      Author: utnso
 */

#ifndef SERVER_H_
#define SERVER_H_

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<strings.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include "config_cpu.h"
#include "../../shared/include/sockets.h"

void liberar_conexion_cpu_kernel(int* socket_cliente);
bool iniciar_server_cpu_dispatch(int* fd);
bool iniciar_server_cpu_interrupt(int* fd);

#endif /* SERVER_H_ */
