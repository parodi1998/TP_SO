#include "../include/protocolo.h"

static void* serializar_pid(uint32_t parametro1) {
    void* stream = malloc(sizeof(uint32_t));
    memcpy(stream, &parametro1, sizeof(uint32_t));

    return stream;
}

bool send_pid(int fd,uint32_t parametro1){
	
    size_t size = sizeof(uint32_t);

    void* stream = serializar_pid(parametro1);

    if (send(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }

    free(stream);
    return true;
}

static void* serializar_registro(uint32_t parametro1) {
    void* stream = malloc(sizeof(uint32_t));
    memcpy(stream, &parametro1, sizeof(uint32_t));

    return stream;
}

bool send_registroAX(int fd,uint32_t parametro1){
    size_t size = sizeof(uint32_t);

    void* stream = serializar_registro(parametro1);

    if (send(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }

    free(stream);
    return true;
}

bool send_registroBX(int fd,uint32_t parametro1){
    size_t size = sizeof(uint32_t);

    void* stream = serializar_registro(parametro1);

    if (send(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }

    free(stream);
    return true;
}

bool send_registroCX(int fd,uint32_t parametro1){
    size_t size = sizeof(uint32_t);

    void* stream = serializar_registro(parametro1);

    if (send(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }

    free(stream);
    return true;
}

bool send_registroDX(int fd,uint32_t parametro1){
    size_t size = sizeof(uint32_t);

    void* stream = serializar_registro(parametro1);

    if (send(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }

    free(stream);
    return true;
}

static void* serializar_indice_tabla_paginas(uint32_t parametro1) {
    void* stream = malloc(sizeof(uint32_t));
    memcpy(stream, &parametro1, sizeof(uint32_t));
    return stream;
}

bool send_indice_tabla_paginas(int fd, uint32_t parametro1) {
    size_t size = sizeof(uint32_t);
    void* stream = serializar_indice_tabla_paginas(parametro1);

    if (send(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

static void* serializar_PC(uint32_t parametro1) {
   void* stream = malloc(sizeof(uint32_t));
    memcpy(stream, &parametro1, sizeof(uint32_t));
    return stream;
}

bool send_PC(int fd, uint32_t parametro1) {;
   size_t size = sizeof(uint32_t);
    void* stream = serializar_PC(parametro1);
    if (send(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

static void* serializar_TAM(uint32_t parametro1) {
    void* stream = malloc(sizeof(uint32_t));
    memcpy(stream, &parametro1, sizeof(uint32_t));
    return stream;
}

bool send_TAM(int fd, uint32_t parametro1) {
    size_t size = sizeof(uint32_t);
    void* stream = serializar_TAM(parametro1);
    if (send(fd, stream, size, MSG_WAITALL) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}


/**
 * Consola -> Kernel: Envio y recepcion de Instrucciones
 * */
static size_t calcular_tamanio_informacion(t_list* instrucciones, size_t cantidad_instrucciones) {
	size_t tamanio_informacion;
	if(list_is_empty(instrucciones)) {
		tamanio_informacion = 0;
	} else {
		for(size_t i = 0; i<cantidad_instrucciones; i++) {
        	char* instruccion = list_get(instrucciones,i);
        	tamanio_informacion += sizeof(size_t) + string_length(instruccion)+1;
    	}
	}
	return tamanio_informacion;
}

static void* serializar_instrucciones(size_t* size, t_list* instrucciones, char* segmentos) {
	/**
	 * Tamanio de la informacion es cantad de instrucciones + la cantidad de espacio que necesito para las instruccioens
	 * */
    op_code cop = INSTRUCCIONES;
	size_t cantidad_instrucciones = list_size(instrucciones);
    size_t tamanio_segmentos = string_length(segmentos)+1;
	size_t tamanio_instrucciones = calcular_tamanio_informacion(instrucciones, cantidad_instrucciones);
	size_t tamanio_informacion = sizeof(size_t) + tamanio_instrucciones;
	*size = 
		sizeof(op_code) + 		// codigo de operacion
		sizeof(size_t) +		// tamaño del string segmentos
        tamanio_segmentos +     // segmentos
		sizeof(size_t) +  		// cantidad de informacion util
		sizeof(size_t) +		// cantidad de instrucciones
		tamanio_instrucciones;	// (largo de instruccion + instruccion) * cantidad de instrucciones 

    void* stream = malloc(*size);
	void* p = stream;

	memcpy(p, &cop, sizeof(op_code)); // mando el codigo de operacion
	p += sizeof(op_code);

    memcpy(p,&tamanio_segmentos,sizeof(size_t)); // mando el tamaño del string segmentos
	p += sizeof(size_t);

	memcpy(p,segmentos,sizeof(tamanio_segmentos)); // mando los segmentos
	p += tamanio_segmentos;

    memcpy(p,&tamanio_informacion,sizeof(size_t)); // mando el tamaño de toda la info a recibir
	p += sizeof(size_t);

	memcpy(p,&cantidad_instrucciones,sizeof(size_t)); // mando la cantidad de instrucciones
	p += sizeof(size_t);

	for(size_t i = 0; i<cantidad_instrucciones; i++) {
        char* instruccion = list_get(instrucciones,i);
		size_t tamanio_instruccion = string_length(instruccion)+1;
		memcpy(p,&tamanio_instruccion,sizeof(size_t));
		p += sizeof(size_t);
		memcpy(p,instruccion,tamanio_instruccion);
		p += tamanio_instruccion;
    }

    return stream;
}

static void deserializar_instrucciones(void* stream, t_list** instrucciones) {
	// se supone que si llegue aca, ya saque el codigo de operacion del paquete
	size_t cantidad_instrucciones;
	t_list* instrucciones_aux = list_create();
	void* p = stream;
	memcpy(&cantidad_instrucciones, p, sizeof(size_t));	
	p += sizeof(size_t);

	for(size_t i = 0; i<cantidad_instrucciones; i++) {
		size_t tamanio_instruccion;
		memcpy(&tamanio_instruccion, p, sizeof(size_t));
		p += sizeof(size_t);
		char* instruccion = malloc(tamanio_instruccion);
		memcpy(instruccion, p, tamanio_instruccion);
		p += tamanio_instruccion;
		list_add(instrucciones_aux, instruccion);
    }
	*instrucciones = instrucciones_aux;
}

bool send_instrucciones(int fd, t_list* instrucciones, char* segmentos) {
    size_t size;
    void* stream = serializar_instrucciones(&size, instrucciones, segmentos);
    if (send(fd, stream, size, 0) == -1) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

bool recv_instrucciones(int fd, t_list** instrucciones,char** segmentos) {
	// tamanio del proceso
    size_t sz_segmentos;
    if (recv(fd, &sz_segmentos, sizeof(size_t), 0) != sizeof(size_t)) {
        return false;
    }

    // string con los segmentos
    char* segmentos_aux = malloc(sizeof(sz_segmentos));
    if (recv(fd, segmentos_aux, sz_segmentos, 0) != sz_segmentos) {
        free(segmentos_aux);
        return false;
    }

    // tamanio total del stream
    size_t size;
    if (recv(fd, &size, sizeof(size_t), 0) != sizeof(size_t)) {
        return false;
    }
    // recibe TODO el stream
    void* stream = malloc(size);
    if (recv(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }

    t_list* instrucciones_aux; // el malloc lo realiza la funcion deserializar
	
    deserializar_instrucciones(stream, &instrucciones_aux);

    *instrucciones = instrucciones_aux;
	*segmentos = segmentos_aux;

    free(stream);
    return true;
}