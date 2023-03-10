/*
 * server.c
 *
 *  Created on: Oct 5, 2022
 *      Author: utnso
 */

#include"../include/server.h"

void liberar_conexion_cpu_kernel(int* socket_cliente) {
    close(*socket_cliente);
    *socket_cliente = -1;
}

int cpu_conexion_server(int argc, char * puerto_in)
{
if (argc > 1)
{

int fd, fd2, longitud_cliente, puerto;
//puerto=atoi(argv[1]);
puerto=atoi(puerto_in);

struct sockaddr_in server;
struct sockaddr_in client;

server.sin_family=AF_INET;
server.sin_port = htons(puerto);
server.sin_addr.s_addr = INADDR_ANY;
bzero(&(server.sin_zero),8);

if ((fd=socket(AF_INET  , SOCK_STREAM, 0)) < 0){
perror("Error de apertura de socket");
exit(-1);
}

if(bind(fd,(struct sockaddr*)&server, sizeof(struct sockaddr))== -1) {
printf("error en bind() \n");
exit(-1);
}

if(listen(fd, 5) == -1) {
printf("error en listen()\n");
exit(-1);
}

while(1) {
longitud_cliente= sizeof(struct sockaddr_in);
if ((fd2 = accept(fd, (struct sockaddr *)&client, &longitud_cliente)) == -1) {
printf("error en accept()\n");
exit(-1);
}
send(fd2, "Bienvenido a mi servidor.\n", 26, 0);
liberar_conexion_cpu_kernel(&fd2);
}
liberar_conexion_cpu_kernel(&fd);
}
else{
printf("No se ingresó el puerto  por parámetro\n");
}
return 0;
}


bool iniciar_server_cpu_dispatch(int* fd) {
     *fd = iniciar_servidor(get_log(), "CPU_DISPATCH", get_ip(), get_puerto_escucha_dispatch());
    return fd != 0;
}

bool iniciar_server_cpu_interrupt(int* fd) {
     *fd = iniciar_servidor(get_log(), "CPU_INTERRUPT", get_ip(), get_puerto_escucha_interrupt());
    return fd != 0;
}
