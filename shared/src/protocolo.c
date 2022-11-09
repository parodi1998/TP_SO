#include "../include/protocolo.h"

void log_list_of_chars(t_log* logger, t_list* list) {
    size_t size = list_size(list);
    for(size_t i = 0; i < size; i++) {
        char* item = list_get(list,i);
        log_info(logger,item);
    }
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

static void* serializar_instrucciones(op_code codigo, size_t* size, t_list* instrucciones) {
	/**
	 * Tamanio de la informacion es cantad de instrucciones + la cantidad de espacio que necesito para las instruccioens
	 * */
    op_code cop = codigo;
	size_t cantidad_instrucciones = list_size(instrucciones);
	size_t tamanio_instrucciones = calcular_tamanio_informacion(instrucciones, cantidad_instrucciones);
	size_t tamanio_informacion = sizeof(size_t) + tamanio_instrucciones;
	*size = 
		sizeof(op_code) + 		// codigo de operacion
		sizeof(size_t) +  		// cantidad de informacion util
		sizeof(size_t) +		// cantidad de instrucciones
		tamanio_instrucciones;	// (largo de instruccion + instruccion) * cantidad de instrucciones 

    void* stream = malloc(*size);
	void* p = stream;

	memcpy(p, &cop, sizeof(op_code)); // mando el codigo de operacion
	p += sizeof(op_code);

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

bool send_instrucciones(int fd, t_list* instrucciones) {
    size_t size;
    void* stream = serializar_instrucciones(INSTRUCCIONES, &size, instrucciones);
    if (send(fd, stream, size, 0) == -1) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

bool recv_instrucciones(int fd, t_list** instrucciones) {
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

    free(stream);
    return true;
}


/**
 * PCB: envio y recepcion
 * */

static void* serializar_pcb(size_t* size, t_pcb* pcb) {

    op_code cop = PCB_KERNEL;
	size_t cantidad_instrucciones = list_size(pcb->instrucciones);
	size_t tamanio_instrucciones = calcular_tamanio_informacion(pcb->instrucciones, cantidad_instrucciones);
	size_t tamanio_pcb = 7 * sizeof(size_t) + sizeof(t_estado_pcb) + tamanio_instrucciones;
	*size = 
		sizeof(op_code) + 		    // codigo de operacion
		sizeof(size_t) +  		    // tamanio total del pcb
        sizeof(size_t) +		    // pcb.id_proceso
        sizeof(size_t) +		    // pcb.program_counter
        sizeof(size_t) +		    // pcb.registro_AX 
        sizeof(size_t) +		    // pcb.registro_BX 
        sizeof(size_t) +		    // pcb.registro_CX 
        sizeof(size_t) +		    // pcb.registro_DX
        sizeof(size_t) +		    // pcb.tabla_segmentos
        sizeof(t_estado_pcb) +		// pcb.estado
		sizeof(size_t) +		    // cantidad de instrucciones
		tamanio_instrucciones;	    // (largo de instruccion + instruccion) * cantidad de instrucciones 

    void* stream = malloc(*size);
	void* p = stream;

	memcpy(p, &cop, sizeof(op_code)); // guardo el codigo de operacion
	p += sizeof(op_code);

    memcpy(p,&tamanio_pcb,sizeof(size_t)); // guardo el tamaño de toda la info a enviar, en este caso el pcb
	p += sizeof(size_t);

	memcpy(p,&pcb->id_proceso,sizeof(size_t)); // guardo el id del pcb
	p += sizeof(size_t);

    memcpy(p,&pcb->program_counter,sizeof(size_t)); // guardo el program_counter del pcb
	p += sizeof(size_t);

    memcpy(p,&pcb->tabla_segmentos,sizeof(size_t)); // guardo el tabla_paginas del pcb
	p += sizeof(size_t);

    memcpy(p,&pcb->registro_AX,sizeof(size_t)); // guardo el tabla_paginas del pcb
	p += sizeof(size_t);

    memcpy(p,&pcb->registro_BX,sizeof(size_t)); // guardo el tabla_paginas del pcb
	p += sizeof(size_t);

    memcpy(p,&pcb->registro_CX,sizeof(size_t)); // guardo el tabla_paginas del pcb
	p += sizeof(size_t);

    memcpy(p,&pcb->registro_DX,sizeof(size_t)); // guardo el tabla_paginas del pcb
	p += sizeof(size_t);

    memcpy(p,&pcb->estado,sizeof(t_estado_pcb)); // guardo el estado del pcb
	p += sizeof(t_estado_pcb);

    memcpy(p,&cantidad_instrucciones,sizeof(size_t)); // guardo la cantidad de instrucciones
	p += sizeof(size_t);

	for(size_t i = 0; i<cantidad_instrucciones; i++) {
        char* instruccion = list_get(pcb->instrucciones,i);
		size_t tamanio_instruccion = string_length(instruccion)+1;
		memcpy(p,&tamanio_instruccion,sizeof(size_t));
		p += sizeof(size_t);
		memcpy(p,instruccion,tamanio_instruccion);
		p += tamanio_instruccion;
    }

    return stream;
}

static void deserializar_pcb(void* stream, t_pcb** pcb) {
    // se supone que si llegue aca, ya saque el codigo de operacion del paquete
	size_t cantidad_instrucciones;
    t_pcb* pcb_aux = malloc(sizeof(t_pcb));
	t_list* instrucciones_aux = list_create();
	void* p = stream;

    memcpy(&pcb_aux->id_proceso, p, sizeof(size_t));	
	p += sizeof(size_t);

    memcpy(&pcb_aux->program_counter, p, sizeof(size_t));	
	p += sizeof(size_t);

    memcpy(&pcb_aux->tabla_segmentos, p, sizeof(size_t));	
	p += sizeof(size_t);

    memcpy(&pcb_aux->registro_AX, p, sizeof(size_t));	
	p += sizeof(size_t);

    memcpy(&pcb_aux->registro_BX, p, sizeof(size_t));	
	p += sizeof(size_t);

    memcpy(&pcb_aux->registro_CX, p, sizeof(size_t));	
	p += sizeof(size_t);

    memcpy(&pcb_aux->registro_DX, p, sizeof(size_t));	
	p += sizeof(size_t);

    memcpy(&pcb_aux->estado, p, sizeof(t_estado_pcb));	
	p += sizeof(t_estado_pcb);

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
	pcb_aux->instrucciones = instrucciones_aux;
    *pcb = pcb_aux;
}

bool send_pcb(int fd, t_pcb* pcb) {
    size_t size;
    void* stream = serializar_pcb(&size, pcb);
    if (send(fd, stream, size, 0) == -1) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

bool recv_pcb(int fd, t_pcb** pcb) {
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

    t_pcb* pcb_aux; // el malloc lo realiza la funcion deserializar
	
    deserializar_pcb(stream, &pcb_aux);

    *pcb = pcb_aux;

    free(stream);
    return true;
}


static void* serializar_segmentos(size_t* size, t_list* segmentos) {
    serializar_instrucciones(SEGMENTOS, size, segmentos);
}

bool send_segmentos(int fd, t_list* segmentos) {
    size_t size;
    void* stream = serializar_segmentos(&size, segmentos);
    if (send(fd, stream, size, 0) == -1) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

bool recv_segmentos(int fd, t_list** segmentos) {
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

    *segmentos = instrucciones_aux;

    free(stream);
    return true;
}
