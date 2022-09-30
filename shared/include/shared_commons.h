#ifndef SHARED_COMMONS_H_
#define SHARED_COMMONS_H_

#include <inttypes.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/collections/list.h>

typedef enum {
    MENSAJE,
    PAQUETE,
    DEBUG = 100,
} op_code;

typedef struct {
	int size;
	void* stream;
} t_buffer;

typedef struct {
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

#endif