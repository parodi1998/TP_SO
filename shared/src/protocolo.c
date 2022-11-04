#include "../include/protocolo.h"

// Para serparar las instrucciones, vamos a usar el caracter |
static char* serializar_instrucciones(t_list* instrucciones) {
    int index;  
    char* instrucciones_string = string_new();
    for(index = 0; index < list_size(instrucciones); index++) {
        string_append(&instrucciones_string,list_get(instrucciones,index));
        string_append(&instrucciones_string,"|");
    }
    return instrucciones_string;
}
/*
static t_list* deserializar_instrucciones(char* instrucciones_string) {
    
}
*/
bool send_instrucciones(int fd, t_list* instrucciones) {
    return enviar_mensaje_bool(INSTRUCCIONES, serializar_instrucciones(instrucciones), fd);
}

bool recv_instrucciones(int fd, t_list** instrucciones) {
    return true;
} 

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
