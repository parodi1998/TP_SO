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
#include<strings.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

void liberar_conexion_cpu_kernel(int* socket_cliente);

#endif /* SERVER_H_ */
