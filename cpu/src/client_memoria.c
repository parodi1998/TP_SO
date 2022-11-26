#include "../include/client_memoria.h"
#include "../include/config_cpu.h"

//principales mensajes de comunicacion con memoria

char* leer_memoria(uint32_t pid,uint32_t dir_fisica, uint32_t tamanio){
	char* mensaje = string_from_format("%d|%d|%d",pid,dir_fisica,tamanio);

	int conexion = crear_conexion_memoria_cpu();
	log_info(get_log(),"REQUEST LEER: %s" , mensaje );
	enviar_mensaje_memoria_cpu(mensaje,conexion,LEER);
	char* rta = recibir_mensaje_memoria_cpu(conexion);
	log_info(get_log(),"RESPUESTA LEER: %s" , rta );
	free(mensaje);

	return rta;

}

char* escribir_memoria(uint32_t pid,uint32_t dir_fisica,uint32_t tamanio, char* contenido){
	char* mensaje = string_from_format("%d|%d|%d|%s",dir_fisica,tamanio,contenido);

	int conexion = crear_conexion_memoria_cpu();
	log_info(get_log(),"REQUEST ESCRIBIR: %s" , mensaje );
	enviar_mensaje_memoria_cpu(mensaje,conexion,ESCRIBIR);
	char* rta = recibir_mensaje_memoria_cpu(conexion);
	log_info(get_log(),"RESPUESTA ESCRIBIR: %s" , rta );
	free(mensaje);
	return rta;
}

char* traducir_memoria(uint32_t pid,uint32_t segment, uint32_t page,uint32_t es_escritura){
	char* mensaje = string_from_format("%d|%d|%d|%d",pid,segment,page,es_escritura);

	int conexion = crear_conexion_memoria_cpu();
	log_info(get_log(),"REQUEST TRADUCIR: %s" , mensaje );
	enviar_mensaje_memoria_cpu(mensaje,conexion,TRADUCIR);
	char* rta = recibir_mensaje_memoria_cpu(conexion);
	log_info(get_log(),"RESPUESTA TRADUCIR: %s" , rta );
	free(mensaje);
	return rta;
}

char* finalizar_proceso(uint32_t pid,uint32_t segment){
	char* mensaje = string_from_format("%d|%d",pid,segment);

	int conexion = crear_conexion_memoria_cpu();
	log_info(get_log(),"REQUEST FINALIZAR_PROCESO: %s" , mensaje );
	enviar_mensaje_memoria_cpu(mensaje,conexion,FINALIZAR_PROCESO);
	char* rta = recibir_mensaje_memoria_cpu(conexion);
	log_info(get_log(),"RESPUESTA FINALIZAR_PROCESO: %s" , rta );
	free(mensaje);
	return rta;
}

char* recibir_config_para_mmu(){
	log_info(get_log(),"REQUEST CONFIG_CPU");
	int conexion = crear_conexion_memoria_cpu();
	enviar_mensaje_memoria_cpu("",conexion,CONFIG_CPU);
	char* rta = recibir_mensaje_memoria_cpu(conexion);
	log_info(get_log(),"RESPUESTA CONFIG_CPU: %s" , rta );
	return rta;
}







void* serializar_paquete_memoria_cpu(t_paquete_memoria* paquete, int bytes)
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

int crear_conexion_memoria_cpu()
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(get_ip_memoria(), get_puerto_memoria(), &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
		printf("error");

	freeaddrinfo(server_info);

	return socket_cliente;
}

//TODO
void enviar_mensaje_memoria_cpu(char* mensaje, int socket_cliente,op_code_memoria codigo_operacion)
{
	t_paquete_memoria* paquete = malloc(sizeof(t_paquete_memoria));

		paquete->codigo_operacion = codigo_operacion;
		paquete->buffer = malloc(sizeof(t_buffer));
		paquete->buffer->size = strlen(mensaje) + 1;
		paquete->buffer->stream = malloc(paquete->buffer->size);
		memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

		int bytes = paquete->buffer->size + 2*sizeof(int);

		void* a_enviar = serializar_paquete_memoria_cpu(paquete, bytes);

		send(socket_cliente, a_enviar, bytes, 0);

		free(a_enviar);

}



char* recibir_mensaje_memoria_cpu(int socket_cliente)
{
	void * buffer;
	int size;

	recv(socket_cliente, &size, sizeof(int), MSG_WAITALL);
	buffer = malloc(size);
	recv(socket_cliente, buffer,size, MSG_WAITALL);

	return buffer;
}

void liberar_conexion_memoria_cpu(int socket_cliente)
{
	close(socket_cliente);
}
