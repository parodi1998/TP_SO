#include "../include/client_memoria.h"

char* leer_memoria(int socket_cliente,t_log* logger,uint32_t pid,uint32_t dir_fisica, uint32_t tamanio){
	char* mensaje = string_from_format("%d|%d|%d",pid,dir_fisica,tamanio);

	log_info(logger,"REQUEST LEER: %s" , mensaje );
	enviar_mensaje_memoria(mensaje,socket_cliente,LEER);
	char* rta = recibir_mensaje_memoria(socket_cliente);
	log_info(logger,"RESPUESTA LEER: %s" , rta );
	free(mensaje);

	return rta;

}

char* escribir_memoria(int socket_cliente,t_log* logger,uint32_t pid,uint32_t dir_fisica,uint32_t tamanio, char* contenido){
	char* mensaje = string_from_format("%d|%d|%d|%s",pid,dir_fisica,tamanio,contenido);

	log_info(logger,"REQUEST ESCRIBIR: %s" , mensaje );
	enviar_mensaje_memoria(mensaje,socket_cliente,ESCRIBIR);
	char* rta = recibir_mensaje_memoria(socket_cliente);
	log_info(logger,"RESPUESTA ESCRIBIR: %s" , rta );
	free(mensaje);
	return rta;
}

char* traducir_memoria(int socket_cliente,t_log* logger,uint32_t pid,uint32_t segment, uint32_t page,uint32_t es_escritura){
	char* mensaje = string_from_format("%d|%d|%d|%d",pid,segment,page,es_escritura);

	log_info(logger,"REQUEST TRADUCIR: %s" , mensaje );
	enviar_mensaje_memoria(mensaje,socket_cliente,TRADUCIR);
	char* rta = recibir_mensaje_memoria(socket_cliente);
	log_info(logger,"RESPUESTA TRADUCIR: %s" , rta );
	free(mensaje);
	return rta;
}

char* finalizar_proceso_memoria(int socket_cliente,t_log* logger,uint32_t pid){
	char* mensaje = string_from_format("%d",pid);

	log_info(logger,"REQUEST FINALIZAR_PROCESO: %s" , mensaje );
	enviar_mensaje_memoria(mensaje,socket_cliente,FINALIZAR_PROCESO);
	char* rta = recibir_mensaje_memoria(socket_cliente);
	log_info(logger,"RESPUESTA FINALIZAR_PROCESO: %s" , rta );
	free(mensaje);
	return rta;
}

char* recibir_config_para_mmu(int socket_cliente, t_log* logger){
	log_info(logger,"REQUEST CONFIG_CPU");
	enviar_mensaje_memoria("",socket_cliente,CONFIG_CPU);
	char* rta = recibir_mensaje_memoria(socket_cliente);
	log_info(logger,"RESPUESTA CONFIG_CPU: %s" , rta );
	return rta;
}

char* iniciar_segmento_memoria(int socket_cliente,t_log* logger,uint32_t pid,uint32_t segmento, uint32_t tamanio){
	char* mensaje = string_from_format("%d|%d|%d",pid,segmento,tamanio);

	log_info(logger,"REQUEST INICIAR_SEGMENTO: %s" , mensaje );
	enviar_mensaje_memoria(mensaje,socket_cliente,INICIAR_PROCESO);
	char* rta = recibir_mensaje_memoria(socket_cliente);
	log_info(logger,"RESPUESTA INICIAR_SEGMENTO: %s" , rta );
	free(mensaje);
	return rta;

}

char* page_fault_memoria(int socket_cliente,t_log* logger,uint32_t pid,uint32_t segment, uint32_t page){
	char* mensaje = string_from_format("%d|%d|%d",pid,segment,page);

	log_info(logger,"REQUEST PAGE_FAULT: %s" , mensaje );
	enviar_mensaje_memoria(mensaje,socket_cliente,PAGE_FAULT_MEMORIA);
	char* rta = recibir_mensaje_memoria(socket_cliente);
	log_info(logger,"RESPUESTA PAGE_FAULT: %s" , rta );
	free(mensaje);
	return rta;
}





void* serializar_paquete_memoria(t_paquete_memoria* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;


}

int crear_conexion_memoria(char* IP, char* PORT)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(IP, PORT, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
		printf("error");

	freeaddrinfo(server_info);

	return socket_cliente;
}

bool generar_conexion_kernel_a_memoria(t_log* logger, char* ip, char* puerto, int* fd_memoria) {
	*fd_memoria = crear_conexion(logger,"MEMORIA", ip, puerto);
	return *fd_memoria != 0;
}

void enviar_mensaje_memoria(char* mensaje, int socket_cliente,op_code_memoria codigo_operacion)
{
	t_paquete_memoria* paquete = malloc(sizeof(t_paquete_memoria));

		paquete->codigo_operacion = codigo_operacion;
		paquete->buffer = malloc(sizeof(t_buffer));
		paquete->buffer->size = strlen(mensaje) + 1;
		paquete->buffer->stream = malloc(paquete->buffer->size);
		memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

		int bytes = paquete->buffer->size + 2*sizeof(int);

		void* a_enviar = serializar_paquete_memoria(paquete, bytes);

		send(socket_cliente, a_enviar, bytes, 0);

		free(a_enviar);

}



char* recibir_mensaje_memoria(int socket_cliente)
{
	void * buffer;
	int size;

	recv(socket_cliente, &size, sizeof(int), MSG_WAITALL);
	buffer = malloc(size);
	recv(socket_cliente, buffer,size, MSG_WAITALL);

	return buffer;
}

void liberar_conexion_memoria(int socket_cliente)
{
	close(socket_cliente);
}
