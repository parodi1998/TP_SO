/*
 * server.c
 *
 *  Created on: 26 ago. 2022
 *      Author: utnso
 */
#include "../include/memory_configuration_manager.h"
#include "../include/memory_file_management.h"
#include "../include/server.h"

int fd_client_kernel;
int fd_client_cpu;

pthread_t hilo_memoria_cpu;
pthread_t hilo_memoria_kernel;

sem_t sem_memoria_finalizar;

int iniciar_servidor(t_log* logger, const char* name, char* ip, char* puerto) {
    int socket_servidor;
    struct addrinfo hints, *servinfo;

    // Inicializando hints
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // Recibe los addrinfo
    getaddrinfo(ip, puerto, &hints, &servinfo);

    bool conecto = false;

    // Itera por cada addrinfo devuelto
    for (struct addrinfo *p = servinfo; p != NULL; p = p->ai_next) {
        socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (socket_servidor == -1) // fallo de crear socket
            continue;

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            // Si entra aca fallo el bind
            close(socket_servidor);
            continue;
        }
        // Ni bien conecta uno nos vamos del for
        conecto = true;
        break;
    }

    if(!conecto) {
        free(servinfo);
        return 0;
    }

    listen(socket_servidor, SOMAXCONN); // Escuchando (hasta SOMAXCONN conexiones simultaneas)

    // Aviso al logger
    log_info(logger, "Escuchando en %s:%s (%s)\n", ip, puerto, name);

    freeaddrinfo(servinfo);

    return socket_servidor;
}

bool iniciar_server_memoria(int* fd) {
     *fd = iniciar_servidor(get_logger(), "MEMORIA", get_ip(), port_getter());
    return fd != 0;
}

static int esperar_cliente_memoria(t_log* logger, const char* name, int socket_servidor) {
    struct sockaddr_in dir_cliente;
    socklen_t tam_direccion = sizeof(struct sockaddr_in);

    int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

    log_info(logger, "Cliente conectado (a %s)\n", name);

    return socket_cliente;
}

void iniciar_servidor_memory(void)
{
	int socket_servidor = 0;

	if(!iniciar_server_memoria(&socket_servidor)) {
        log_error(get_logger(),"No se iniciar el server memoria");
        // terminar_programa(); deberia llamarse a una funcion que libere toda la memoria de loggers, configs, listas, giladas
        return;
    } else {
		sem_init(&sem_memoria_finalizar, 0, 0);
		
		fd_client_cpu = esperar_cliente_memoria(get_logger(), "CPU", socket_servidor);
		if (fd_client_cpu != -1) {
			pthread_create(&hilo_memoria_cpu, NULL, (void*) serve_client_memory, &fd_client_cpu);
    		pthread_detach(hilo_memoria_cpu);
		} else {
			return;
		}

		fd_client_kernel = esperar_cliente_memoria(get_logger(), "KERNEL", socket_servidor);
		if (fd_client_kernel != -1) {
			pthread_create(&hilo_memoria_kernel, NULL, (void*) serve_client_memory, &fd_client_kernel);
    		pthread_detach(hilo_memoria_kernel);
		} else {
			return;
		}	

		sem_wait(&sem_memoria_finalizar);
		sem_destroy(&sem_memoria_finalizar);
		return;
	}
}

void serve_client_memory(int* socket)
{
	while(*socket != -1) {
		int cod_op;
		if(recv(*socket, &cod_op, sizeof(int), MSG_WAITALL) == -1)
			cod_op = -1;
		process_request_memory(cod_op, *socket);
	}
	sem_post(&sem_memoria_finalizar);
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
			size =  string_length((char*)response);
			devolver_mensaje_memory(response,size, cliente_fd,op_code_response);
			free(msg);
			free(response);
			break;
		case INICIAR_PROCESO:
			msg = recibir_mensaje_memory(cliente_fd, &size);
			response = procesar_mensaje_iniciar_proceso(msg,&size,&op_code_response);
			devolver_mensaje_memory(response,size, cliente_fd,op_code_response);
			free(msg);
			free(response);
			break;
		case TRADUCIR:
			msg = recibir_mensaje_memory(cliente_fd, &size);
			response = procesar_traducir_direccion(msg,&size,&op_code_response);
			devolver_mensaje_memory(response,size, cliente_fd,op_code_response);
			free(msg);
			free(response);
			break;
		case ESCRIBIR:
			msg = recibir_mensaje_memory(cliente_fd, &size);
			response = procesar_mensaje_escribir(msg,&size,&op_code_response);
			devolver_mensaje_memory(response,size, cliente_fd,op_code_response);
			free(msg);
			free(response);
			break;
		case LEER:
			msg = recibir_mensaje_memory(cliente_fd, &size);
			response = procesar_mensaje_leer(msg,&size,&op_code_response);
			devolver_mensaje_memory(response,size, cliente_fd,op_code_response);
			free(msg);
			free(response);
			break;
		case SWAP_PAGE:
			msg = recibir_mensaje_memory(cliente_fd, &size);
			response = procesar_swapping(msg,&size,&op_code_response);
			devolver_mensaje_memory(response,size, cliente_fd,op_code_response);
			devolver_mensaje_memory(response,size, fd_client_cpu,ACTUALIZAR_TLB);
			free(msg);
			free(response);
			break;
		case SUSPENDER_PROCESO:
			msg = recibir_mensaje_memory(cliente_fd, &size);
			response = procesar_suspender_proceso(msg,&size,&op_code_response);
			devolver_mensaje_memory(response,size, cliente_fd,op_code_response);
			free(msg);
			free(response);
			break;
		case FINALIZAR_PROCESO:
			msg = recibir_mensaje_memory(cliente_fd, &size);
			response = procesar_finalizar_proceso(msg,&size,&op_code_response);
			devolver_mensaje_memory(response,size, cliente_fd,op_code_response);
			free(msg);
			free(response);
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

	free(array[0]);
	free(array[1]);
	free(array[2]);
	free(array);

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
	void* response = read_value_in_memory(pid,address,size);

	free(array[0]);
	free(array[1]);
	free(array[2]);
	free(array);

	return response;
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

	free(array[0]);
	free(array[1]);
	free(array[2]);
	free(array[3]);
	free(array);

	return (void*)"OK";

}

void* procesar_suspender_proceso(char* string,int* size,int* op_code){
	//ENTRADA:<PID>|<SEGMENT>
	//SALIDA:<OK>
	char** array = string_split(string,"|");
	uint32_t pid = (volatile uint32_t) atoi( array[0]);
	uint32_t segment = (volatile uint32_t) atoi( array[1]);
	suspend_process(pid,segment);

	free(array[0]);
	free(array[1]);
	free(array);


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

	free(array[0]);
	free(array[1]);
	free(array[2]);
	free(array[3]);
	free(array);

	return response_string;
}

void* procesar_swapping(char* string, int* size,int* op_code){
	//ENTRADA: <PID>|<SEGMENTO>|<PAGINA>
	//SALIDA: <SEGMENTO>|<PAGINA> de la victima
	char** array = string_split(string,"|");
	uint32_t pid = (volatile uint32_t) atoi( array[0]);
	uint32_t segment = (volatile uint32_t) atoi( array[1]);
	uint32_t page_number = (volatile uint32_t) atoi( array[2]);
	char* response = swap_page(pid, segment,page_number);
	*size = string_length(response);
	*op_code = OK;

	free(array[0]);
	free(array[1]);
	free(array[2]);
	free(array);
	return (void*)response;

}
