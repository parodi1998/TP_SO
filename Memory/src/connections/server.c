/*
 * server.c
 *
 *  Created on: 26 ago. 2022
 *      Author: utnso
 */
#include "server.h"
#include "../config_manager/memory_configuration_manager.h"
#include "../memory_file_management.h"

void iniciar_servidor(void)
{
	int socket_servidor;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo("127.0.0.1", port_getter(), &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next)
    {
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor);
            continue;
        }
        break;
    }

	listen(socket_servidor, SOMAXCONN);

    freeaddrinfo(servinfo);

    while(1)
    	esperar_cliente(socket_servidor);
}

void esperar_cliente(int socket_servidor)
{
	struct sockaddr_in dir_cliente;

	int tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	pthread_create(&thread,NULL,(void*)serve_client,&socket_cliente);
	pthread_detach(thread);

}

void serve_client(int* socket)
{
	int cod_op;
	if(recv(*socket, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		cod_op = -1;
	process_request(cod_op, *socket);
}

void process_request(int cod_op, int cliente_fd) {
	int size;
	void* msg;
	void* response;
	msg = recibir_mensaje(cliente_fd, &size);
		switch (cod_op) {
		case INICIAR_PROCESO:
			response = procesar_mensaje_iniciar_proceso(msg,&size);
			break;
		case ESCRIBIR:
			response = procesar_mensaje_escribir(msg,&size);
			break;
		case LEER:
			response = procesar_mensaje_leer(msg,&size);
			break;
		case SUSPENDER_PROCESO:
			response = procesar_suspender_proceso(msg,&size);
			break;
		case RESTAURAR_PROCESO:
			response = procesar_restaurar_proceso(msg,&size);
			break;
		case FINALIZAR_PROCESO:
			response = procesar_finalizar_proceso(msg,&size);
			break;
		}
		devolver_mensaje(response,size, cliente_fd);
		free(msg);
}

void* recibir_mensaje(int socket_cliente, int* size)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void* serializar_paquete(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}

void devolver_mensaje(void* payload, int size, int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = size;
	paquete->buffer->stream = malloc(size);
	memcpy(paquete->buffer->stream, payload,size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void* procesar_mensaje_iniciar_proceso(char* string,int* size_response){
	//<PID>|<SEGMENT>|<SIZE>
	char** array = string_split(string,"|");
	uint32_t pid = (volatile uint32_t) atoi( array[0]);
	uint32_t segment = (volatile uint32_t) atoi( array[1]);
	uint32_t size = (volatile uint32_t) atoi( array[2]);

	int32_t response = initialize_process(pid,segment,size);
	char* message = process_numeric_response(response);
	*size_response = string_length(message);
	return (void*)message;
}


char* process_numeric_response(int32_t response){
	if(response == SUCCESS){
		return "Successful Operation";
	}
	if(response == FAILURE){
		return "Operation Failure";
	}
	return string_itoa(response);
}

void* procesar_mensaje_leer(char* string,int* size_response){
	//<ADDRESS>|<SIZE>
	char** array = string_split(string,"|");
	uint32_t address = (volatile uint32_t) atoi( array[0]);
	uint32_t size = (volatile uint32_t) atoi( array[1]);
	*size_response = size;
	return read_value_in_memory(address,size);
}

void* procesar_mensaje_escribir(char* string,int* size_response){
	//<ADDRESS>|<SIZE>|<VALUE>
	char** array = string_split(string,"|");
	uint32_t address = (volatile uint32_t) atoi( array[0]);
	uint32_t size = (volatile uint32_t) atoi( array[1]);
	void* value =(void*) array[2];
	save_value_in_memory(address,value,size);
	char* message = process_numeric_response(SUCCESS);
	*size_response = string_length(message);
	return (void*)message;

}

void* procesar_suspender_proceso(char* string,int* size){
	//<PID>|<SEGMENT>
	char** array = string_split(string,"|");
	uint32_t pid = (volatile uint32_t) atoi( array[0]);
	uint32_t segment = (volatile uint32_t) atoi( array[1]);
	int32_t response = suspend_process(pid,segment);
	char* message = process_numeric_response(response);
	*size = string_length(message);
	return (void*)message;
}


void* procesar_finalizar_proceso(char* string,int* size){
	//<PID>|<SEGMENT>
	char** array = string_split(string,"|");
	uint32_t pid = (volatile uint32_t) atoi( array[0]);
	uint32_t segment = (volatile uint32_t) atoi( array[1]);
	int32_t response = finalize_process(pid,segment);
	char* message = process_numeric_response(response);
	*size = string_length(message);
	return (void*)message;
}


void* procesar_restaurar_proceso(char* string,int* size){
	//<PID>|<SEGMENT>
	char** array = string_split(string,"|");
	uint32_t pid = (volatile uint32_t) atoi( array[0]);
	uint32_t segment = (volatile uint32_t) atoi( array[1]);
	int32_t response = restore_process(pid,segment);
	char* message = process_numeric_response(response);
	*size = string_length(message);
	return (void*)message;
}
