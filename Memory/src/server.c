/*
 * server.c
 *
 *  Created on: 26 ago. 2022
 *      Author: utnso
 */
#include "../include/memory_configuration_manager.h"
#include "../include/memory_file_management.h"
#include "../include/server.h"

void iniciar_servidor_memory(void)
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
    	esperar_cliente_memory(socket_servidor);
}

void esperar_cliente_memory(int socket_servidor)
{
	struct sockaddr_in dir_cliente;

	int tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	pthread_t thread;

	pthread_create(&thread,NULL,(void*)serve_client_memory,&socket_cliente);
	pthread_detach(thread);

}

void serve_client_memory(int* socket)
{
	while(*socket != -1) {
		int cod_op;
		if(recv(*socket, &cod_op, sizeof(int), MSG_WAITALL) == -1)
			cod_op = -1;
		process_request_memory(cod_op, *socket);
	}
	
}

void process_request_memory(int cod_op, int cliente_fd) {
	int size;
	int op_code_response;
	void* msg = NULL;
	void* response = NULL;
		switch (cod_op) {
		case CONFIG_CPU:
			msg = recibir_mensaje_memory(cliente_fd, &size);
			response = config_cpu();
			size = sizeof(response);
			devolver_mensaje_memory(response,size, cliente_fd,op_code_response);
			break;
		case INICIAR_PROCESO:
			msg = recibir_mensaje_memory(cliente_fd, &size);
			response = procesar_mensaje_iniciar_proceso(msg,&size,&op_code_response);
			devolver_mensaje_memory(response,size, cliente_fd,op_code_response);
			break;
		case TRADUCIR:
			msg = recibir_mensaje_memory(cliente_fd, &size);
			response = procesar_traducir_direccion(msg,&size,&op_code_response);
			devolver_mensaje_memory(response,size, cliente_fd,op_code_response);
			break;
		case ESCRIBIR:
			msg = recibir_mensaje_memory(cliente_fd, &size);
			response = procesar_mensaje_escribir(msg,&size,&op_code_response);
			devolver_mensaje_memory(response,size, cliente_fd,op_code_response);
			break;
		case LEER:
			msg = recibir_mensaje_memory(cliente_fd, &size);
			response = procesar_mensaje_leer(msg,&size,&op_code_response);
			devolver_mensaje_memory(response,size, cliente_fd,op_code_response);
			break;
		case SWAP_PAGE:
			msg = recibir_mensaje_memory(cliente_fd, &size);
			response = procesar_swapping(msg,&size,&op_code_response);
			devolver_mensaje_memory(response,size, cliente_fd,op_code_response);
			break;
		case SUSPENDER_PROCESO:
			msg = recibir_mensaje_memory(cliente_fd, &size);
			response = procesar_suspender_proceso(msg,&size,&op_code_response);
			devolver_mensaje_memory(response,size, cliente_fd,op_code_response);
			break;
		case FINALIZAR_PROCESO:
			msg = recibir_mensaje_memory(cliente_fd, &size);
			response = procesar_finalizar_proceso(msg,&size,&op_code_response);
			devolver_mensaje_memory(response,size, cliente_fd,op_code_response);
			break;
		}
}

void* recibir_mensaje_memory(int socket_cliente, int* size)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void* serializar_paquete_memory(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}

void devolver_mensaje_memory(void* payload, int size, int socket_cliente,int op_code)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = op_code;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = size;
	paquete->buffer->stream = malloc(size);
	memcpy(paquete->buffer->stream, payload,size);

	int bytes = paquete->buffer->size + sizeof(int);

	void* a_enviar = serializar_paquete_memory(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void* procesar_mensaje_iniciar_proceso(char* string,int* size_response,int* op_code){
	//ENTRADA <PID>|<SEGMENT>|<SIZE>
	//SALIDA <NUMERIC>
	char** array = string_split(string,"|");
	uint32_t pid = (volatile uint32_t) atoi( array[0]);
	uint32_t segment = (volatile uint32_t) atoi( array[1]);
	uint32_t size = (volatile uint32_t) atoi( array[2]);

	int32_t response = initialize_process(pid,segment,size);
	char* message = string_itoa(response);

	*size_response = sizeof(response);
	*op_code = INICIAR_PROCESO;

	return (void*)message;
}

void* procesar_mensaje_leer(char* string,int* size_response,int* op_code){
	//ENTRADA: <PID>|<ADDRESS>|<SIZE>
	//SALIDA:<CONTENIDO>
	char** array = string_split(string,"|");
	uint32_t pid = (volatile uint32_t) atoi( array[0]);
	uint32_t address = (volatile uint32_t) atoi( array[1]);
	uint32_t size = (volatile uint32_t) atoi( array[2]);
	*size_response = size;
	*op_code = LEER;
	return read_value_in_memory(pid,address,size);
}

void* procesar_mensaje_escribir(char* string,int* size_response,int* op_code){
	//ENTRADA:<PID><ADDRESS>|<SIZE>|<VALUE>
	//SALIDA: "OK"
	char** array = string_split(string,"|");
	uint32_t pid = (volatile uint32_t) atoi( array[0]);
	uint32_t address = (volatile uint32_t) atoi( array[1]);
	uint32_t size = (volatile uint32_t) atoi( array[2]);

	save_value_in_memory(pid,address,(void*) array[3],size);
	*op_code = OK;
	*size_response = string_length("OK")+1;
	return (void*)"OK";

}

void* procesar_suspender_proceso(char* string,int* size,int* op_code){
	//ENTRADA:<PID>|<SEGMENT>
	//SALIDA:<OK>
	char** array = string_split(string,"|");
	uint32_t pid = (volatile uint32_t) atoi( array[0]);
	uint32_t segment = (volatile uint32_t) atoi( array[1]);
	suspend_process(pid,segment);

	*size = string_length("OK")+1;
	*op_code = OK;
	return (void*)"OK";
}


void* procesar_finalizar_proceso(char* string,int* size,int* op_code){
	//ENTRADA:<PID>
	//SALIDA:<OK>
	uint32_t pid = (volatile uint32_t) atoi( string);
	finalize_process(pid);
	*size = string_length("OK")+1;
	*op_code = OK;
	return (void*)"OK";
}

void* procesar_traducir_direccion(char* string,int* size, int* op_code){
	//ENTRADA: <PID>|<SEGMENT>|<PAGE>|<ES ESCRITURA>
	//SALIDA: <HUBO_PAGE_FAULT>|<FRAME>
	char** array = string_split(string,"|");
	uint32_t pid = (volatile uint32_t) atoi( array[0]);
	uint32_t segment = (volatile uint32_t) atoi( array[1]);
	uint32_t page = (volatile uint32_t) atoi( array[2]);
	uint32_t is_writting = (volatile uint32_t) atoi( array[3]);

	t_translation_response* response = translate_logical_address(pid, segment,page,is_writting);

	*op_code = response->result;

	char* response_string = string_from_format("%d|%d",response->page_fault,response->frame);

	free(response);
	*size = sizeof(response_string);

	return response_string;
}

void* procesar_swapping(char* string, int* size,int* op_code){
	//ENTRADA: <PID>|<SEGMENTO>|<PAGINA>
	//SALIDA: OK
	char** array = string_split(string,"|");
		uint32_t pid = (volatile uint32_t) atoi( array[0]);
		uint32_t segment = (volatile uint32_t) atoi( array[1]);
		uint32_t page_number = (volatile uint32_t) atoi( array[2]);
		swap_page(pid, segment,page_number);
		*size = string_length("OK")+1;
		*op_code = OK;
		return (void*)"OK";

}
