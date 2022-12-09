#ifndef SHARED_COMMONS_H_
#define SHARED_COMMONS_H_

#include <inttypes.h>
#include <sys/socket.h>
#include <signal.h>
#include <netdb.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <pthread.h>
#include <semaphore.h>

typedef enum {
	MENSAJE,
	INSTRUCCIONES_Y_SEGMENTOS,
	PCB_KERNEL,
	PCB_INTERRUPT,
	CONSOLA_INSTRUCCIONES,
	CONSOLA_SEGMENTOS,
	CONSOLA_KERNEL_INIT_PCB,
	CONSOLA_PANTALLA,
	CONSOLA_TECLADO,
	CONSOLA_EXIT,
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