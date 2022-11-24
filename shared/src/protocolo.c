#include "../include/protocolo.h"

/**
 * Consola -> Kernel: Envio y recepcion de Instrucciones
 * */
static size_t calcular_tamanio_informacion_de_lista(t_list* lista, size_t size_lista) {
	size_t tamanio_informacion;
	if(list_is_empty(lista)) {
		tamanio_informacion = 0;
	} else {
		for(size_t i = 0; i<size_lista; i++) {
        	char* item = list_get(lista,i);
        	tamanio_informacion += sizeof(size_t) + string_length(item)+1;
    	}
	}
	return tamanio_informacion;
}

static void* serializar_instrucciones_y_segmentos(size_t* size, t_list* instrucciones, t_list* segmentos) {
    op_code cop = INSTRUCCIONES_Y_SEGMENTOS;
	size_t cantidad_instrucciones = list_size(instrucciones);
	size_t tamanio_instrucciones = calcular_tamanio_informacion_de_lista(instrucciones, cantidad_instrucciones);
    size_t cantidad_segmentos = list_size(segmentos);
    size_t tamanio_segmentos = calcular_tamanio_informacion_de_lista(segmentos, cantidad_segmentos);
	size_t tamanio_informacion_total = 
        sizeof(size_t) +            //cantidad instrucciones
        tamanio_instrucciones +     //tamanio instrucciones
        sizeof(size_t) +            //cantidad segmentos
        tamanio_segmentos;          //tamanio segmentos
	
    *size = 
		sizeof(op_code) + 		    // codigo de operacion
		sizeof(size_t) +  		    // cantidad de informacion util
		tamanio_informacion_total;	// informacion util

    void* stream = malloc(*size);
	void* p = stream;

	memcpy(p, &cop, sizeof(op_code));
	p += sizeof(op_code);

    memcpy(p,&tamanio_informacion_total,sizeof(size_t)); 
	p += sizeof(size_t);

	memcpy(p,&cantidad_instrucciones,sizeof(size_t)); 
	p += sizeof(size_t);

	for(size_t i = 0; i<cantidad_instrucciones; i++) {
        char* instruccion = list_get(instrucciones,i);
		size_t tamanio_instruccion = string_length(instruccion)+1;
		memcpy(p,&tamanio_instruccion,sizeof(size_t));
		p += sizeof(size_t);
		memcpy(p,instruccion,tamanio_instruccion);
		p += tamanio_instruccion;
    }

    memcpy(p,&cantidad_segmentos,sizeof(size_t));
	p += sizeof(size_t);

	for(size_t i = 0; i<cantidad_segmentos; i++) {
        char* segmento = list_get(segmentos,i);
		size_t tamanio_segmento = string_length(segmento)+1;
		memcpy(p,&tamanio_segmento,sizeof(size_t));
		p += sizeof(size_t);
		memcpy(p,segmento,tamanio_segmento);
		p += tamanio_segmento;
    }

    return stream;
}

static void deserializar_instrucciones_y_segmentos(void* stream, t_list** instrucciones, t_list** segmentos) {
	// se supone que si llegue aca, ya saque el codigo de operacion del paquete
	size_t cantidad_instrucciones;
	t_list* instrucciones_aux = list_create();
    size_t cantidad_segmentos;
	t_list* segmentos_aux = list_create();
	
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

    memcpy(&cantidad_segmentos, p, sizeof(size_t));	
	p += sizeof(size_t);

	for(size_t i = 0; i<cantidad_segmentos; i++) {
		size_t tamanio_segmento;
		memcpy(&tamanio_segmento, p, sizeof(size_t));
		p += sizeof(size_t);
		char* segmento = malloc(tamanio_segmento);
		memcpy(segmento, p, tamanio_segmento);
		p += tamanio_segmento;
		list_add(segmentos_aux, segmento);
    }

	*instrucciones = instrucciones_aux;
    *segmentos = segmentos_aux;
}

bool send_instrucciones_y_segmentos(int fd, t_list* instrucciones, t_list* segmentos) {
    size_t size;
    void* stream = serializar_instrucciones_y_segmentos(&size, instrucciones, segmentos);
    if (send(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

bool recv_instrucciones_y_segmentos(int fd, t_list** instrucciones, t_list** segmentos) {
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
    t_list* segmentos_aux; // el malloc lo realiza la funcion deserializar
	
    deserializar_instrucciones_y_segmentos(stream, &instrucciones_aux, &segmentos_aux);

    *instrucciones = instrucciones_aux;
    *segmentos = segmentos_aux;

    free(stream);
    return true;
}

/**
 * PCB: envio y recepcion
 * */

static void* serializar_pcb(size_t* size, t_pcb* pcb) {

    op_code cop = PCB_KERNEL;
	size_t cantidad_instrucciones = list_size(pcb->instrucciones);
	size_t tamanio_instrucciones = calcular_tamanio_informacion_de_lista(pcb->instrucciones, cantidad_instrucciones);
    size_t cantidad_segmentos = list_size(pcb->tabla_segmentos);
	size_t tamanio_segmentos = 2 * sizeof(size_t) * cantidad_segmentos;
	size_t tamanio_dispositivo_bloqueo = string_length(pcb->dispositivo_bloqueo)+1;
	size_t tamanio_pcb = 11 * sizeof(size_t) + 4 * sizeof(bool) + 2 * sizeof(t_estado_pcb) + tamanio_dispositivo_bloqueo + tamanio_instrucciones + tamanio_segmentos;
	*size = 
		sizeof(op_code) + 		    // codigo de operacion
		sizeof(size_t) +  		    // tamanio total del pcb
        sizeof(size_t) +		    // pcb.id_proceso
        sizeof(size_t) +		    // pcb.program_counter
        sizeof(size_t) +		    // pcb.registro_AX 
        sizeof(size_t) +		    // pcb.registro_BX 
        sizeof(size_t) +		    // pcb.registro_CX 
        sizeof(size_t) +		    // pcb.registro_DX
        sizeof(size_t) +		    // pcb.consola_fd
        sizeof(size_t) +		    // cantidad segmentos
        sizeof(t_estado_pcb) +		// pcb.estado_anterior
        sizeof(t_estado_pcb) +      // pcb.estado_actual
        sizeof(bool) +              // pcb.debe_ser_finalizado
        sizeof(bool) +              // pcb.debe_ser_bloqueado
        sizeof(bool) +              // pcb.puede_ser_interrumpido
        sizeof(bool) +              // pcb.fue_interrumpido
        sizeof(size_t) +            // pcb.registro_para_bloqueo
        sizeof(size_t) +            // pcb.unidades_de_trabajo
        sizeof(size_t) +            // tamanio_dispositivo_bloqueo
        tamanio_dispositivo_bloqueo+// pcb.dispositivo_bloqueo
        tamanio_segmentos +         // 2 size_t * cantidad segmentos
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

    memcpy(p,&pcb->registro_AX,sizeof(size_t)); // guardo el registro_AX del pcb
	p += sizeof(size_t);

    memcpy(p,&pcb->registro_BX,sizeof(size_t)); // guardo el registro_BX del pcb
	p += sizeof(size_t);

    memcpy(p,&pcb->registro_CX,sizeof(size_t)); // guardo el registro_CX del pcb
	p += sizeof(size_t);

    memcpy(p,&pcb->registro_DX,sizeof(size_t)); // guardo el registro_DX del pcb
	p += sizeof(size_t);

    memcpy(p,&pcb->consola_fd,sizeof(size_t)); // guardo el fd consola del pcb
	p += sizeof(size_t);

    memcpy(p,&pcb->debe_ser_finalizado,sizeof(bool)); // guardo el estado debe_ser_finalizado del pcb
	p += sizeof(bool);

    memcpy(p,&pcb->debe_ser_bloqueado,sizeof(bool)); // guardo el estado debe_ser_bloqueado del pcb
	p += sizeof(bool);

    memcpy(p,&pcb->puede_ser_interrumpido,sizeof(bool)); // guardo el estado puede_ser_interrumpido del pcb
	p += sizeof(bool);

    memcpy(p,&pcb->fue_interrumpido,sizeof(bool)); // guardo el estado fue_interrumpido del pcb
	p += sizeof(bool);

    memcpy(p,&pcb->registro_para_bloqueo,sizeof(size_t)); // guardo el index del registro_para_bloqueo del pcb
	p += sizeof(size_t);

    memcpy(p,&pcb->unidades_de_trabajo,sizeof(size_t)); // guardo las unidades_de_trabajo para el bloqueo del pcb
	p += sizeof(size_t);

	memcpy(p,&tamanio_dispositivo_bloqueo,sizeof(size_t));
	p += sizeof(size_t);

	memcpy(p,pcb->dispositivo_bloqueo,tamanio_dispositivo_bloqueo);
	p += tamanio_dispositivo_bloqueo;

    memcpy(p,&pcb->estado_anterior,sizeof(t_estado_pcb)); // guardo el estado anterior del pcb
	p += sizeof(t_estado_pcb);

    memcpy(p,&pcb->estado_actual,sizeof(t_estado_pcb)); // guardo el estado actual del pcb
	p += sizeof(t_estado_pcb);

    memcpy(p,&cantidad_segmentos,sizeof(size_t)); // guardo la cantidad_segmentos del pcb
	p += sizeof(size_t);

    for(size_t i = 0; i<cantidad_segmentos; i++) {
        t_pcb_segmentos* segmento = list_get(pcb->tabla_segmentos,i);
        memcpy(p,segmento,sizeof(size_t)); // guardo el tamanio_segmento
	    p += sizeof(size_t);
        memcpy(p,segmento+sizeof(size_t),sizeof(size_t)); // guardo el id_tabla_paginas
	    p += sizeof(size_t);
    }

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
    size_t cantidad_segmentos;
    t_pcb* pcb_aux = malloc(sizeof(t_pcb));
	t_list* instrucciones_aux = list_create();
    t_list* segmentos_aux = list_create();
	void* p = stream;

    memcpy(&pcb_aux->id_proceso, p, sizeof(size_t));	
	p += sizeof(size_t);

    memcpy(&pcb_aux->program_counter, p, sizeof(size_t));	
	p += sizeof(size_t);

    memcpy(&pcb_aux->registro_AX, p, sizeof(size_t));	
	p += sizeof(size_t);

    memcpy(&pcb_aux->registro_BX, p, sizeof(size_t));	
	p += sizeof(size_t);

    memcpy(&pcb_aux->registro_CX, p, sizeof(size_t));	
	p += sizeof(size_t);

    memcpy(&pcb_aux->registro_DX, p, sizeof(size_t));	
	p += sizeof(size_t);

    memcpy(&pcb_aux->consola_fd, p, sizeof(size_t));	
	p += sizeof(size_t);

    memcpy(&pcb_aux->debe_ser_finalizado, p, sizeof(bool));	
	p += sizeof(bool);

    memcpy(&pcb_aux->debe_ser_bloqueado, p, sizeof(bool));	
	p += sizeof(bool);

    memcpy(&pcb_aux->puede_ser_interrumpido, p, sizeof(bool));	
	p += sizeof(bool);

    memcpy(&pcb_aux->fue_interrumpido, p, sizeof(bool));	
	p += sizeof(bool);

    memcpy(&pcb_aux->registro_para_bloqueo, p, sizeof(size_t));	
	p += sizeof(size_t);

    memcpy(&pcb_aux->unidades_de_trabajo, p, sizeof(size_t));	
	p += sizeof(size_t);

    size_t tamanio_dispositivo_bloqueo;
    memcpy(&tamanio_dispositivo_bloqueo, p, sizeof(size_t));	
	p += sizeof(size_t);

    char* dispositivo_bloqueo = malloc(tamanio_dispositivo_bloqueo);
	memcpy(dispositivo_bloqueo, p, tamanio_dispositivo_bloqueo);
    pcb_aux->dispositivo_bloqueo = dispositivo_bloqueo;
	p += tamanio_dispositivo_bloqueo;
    
    memcpy(&pcb_aux->estado_anterior, p, sizeof(t_estado_pcb));	
	p += sizeof(t_estado_pcb);

    memcpy(&pcb_aux->estado_actual, p, sizeof(t_estado_pcb));	
	p += sizeof(t_estado_pcb);

    memcpy(&cantidad_segmentos, p, sizeof(size_t));	
	p += sizeof(size_t);

    for(size_t i = 0; i<cantidad_segmentos; i++) {
		size_t tamanio_segmento;
        size_t id_tabla_paginas;
		memcpy(&tamanio_segmento, p, sizeof(size_t));
		p += sizeof(size_t);
        memcpy(&id_tabla_paginas, p, sizeof(size_t));
		p += sizeof(size_t);
		t_pcb_segmentos* segmento_pcb = malloc(sizeof(t_pcb_segmentos));
        segmento_pcb->id_tabla_paginas = id_tabla_paginas;
        segmento_pcb->tamanio_segmento = tamanio_segmento;
        list_add(segmentos_aux, segmento_pcb);
    }

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
    pcb_aux->tabla_segmentos = segmentos_aux;
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

void liberar_pcb(t_pcb* proceso) {
    list_destroy_and_destroy_elements(proceso->instrucciones,free);
    list_destroy_and_destroy_elements(proceso->tabla_segmentos,free);
    free(proceso);
}