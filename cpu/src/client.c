/*
 * client.c
 *
 *  Created on: Oct 5, 2022
 *      Author: utnso
 */
#include "../include/client.h"

int generarCliente (int argc, char * ip_in, char* puerto_in)
{
	printf("Estoy generando Cliente");
if (argc > 2)
{
char* ip;
int fd, numbytes, puerto;
char buf[100];
//puerto = atoi(argv[2]);
puerto = atoi(puerto_in);
//puerto = 8002;
//ip=argv[1];
ip = ip_in;


struct hostent *he;
struct sockaddr_in server;
if ((he=gethostbyname(ip))== NULL){
printf("gethostbyname() error \n");
exit(-1);
}
if ((fd=socket(AF_INET, SOCK_STREAM, 0))==-1){
//if ((fd=socket(1, SOCK_SEQPACKET, 0))==-1){

printf("socket() error\n");
exit(-1);
}


//PONER ACÁ LO QUE COPIÉ
struct addrinfo hints, *servinfo;

  // Init de hints
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  // Recibe addrinfo

  getaddrinfo(ip_in, puerto_in, &hints, &servinfo);
  //HASTA ACÁ

//server.sin_family = AF_INET;
server.sin_family = 1;
//server.sin_port = htons(puerto);
server.sin_port = htons(puerto_in);
server.sin_addr = *((struct in_addr *)he->h_addr);
bzero(&(server.sin_zero),8);
int longitud_del_cliente;
//longitud_del_cliente = sizeof(struct sockaddr_in);
longitud_del_cliente = sizeof(server);

//PONER ACÁ NUEVAMENTE
// Crea un socket con la informacion recibida (del primero, suficiente)

  int socket_cliente = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
  printf("Sokcet: %d", socket_cliente);
  // Fallo en crear el socket
  if(socket_cliente == -1) {
    printf("Error a");
      return 0;
  }

  // Error conectando
  if(connect(socket_cliente, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
//	  printf("connect() error\n");
      //freeaddrinfo(servinfo);

	  printf("Veamos qué valor acepta");
	  for (int ij = 0; ij < 10000; ij++)
	  {
		  printf("/n Probando, %d /n", ij);
		  if(connect(ij, servinfo->ai_addr, servinfo->ai_addrlen) > -1)
		  {
			  printf("  Socket Encontrado: %d", ij);

		  }
		  else
		  {
			  printf("/n Lo siento, no es: &d, ", ij);
		  }
		  close(ij);
	  }

  }
//HASTA ACÁ NUEVAMENTE


//if(connect(fd, (struct sockaddr *)&server,	longitud_del_cliente)==-1){
//printf("connect() error\n");
//exit(-1);
//}
buf[numbytes]='\0';
//if ((numbytes=recv(fd, buf, 100, 0))==-1){
if ((numbytes=recv(socket_cliente, buf, 100, 0))==-1){
printf("Error en recv() \n");
exit(-1);
}
buf[numbytes]='\0';

printf("Mensaje del Servidor: %s\n", buf);
close(fd);
}
else{
printf("No se ingresó el ip y puerto por parámetro\n");
}
}

