#include "../include/protocolo.h"

/**
 * Funciones utiles
 */

char* convertir_estado_pcb_a_string(t_estado_pcb estado) {
    char* estado_string = string_new();
    switch(estado) {
        case PCB_NEW:
            string_append(&estado_string, "NEW");
            break;
        case PCB_READY:
            string_append(&estado_string, "READY");
            break;
        case PCB_EXECUTE:
            string_append(&estado_string, "EXECUTE");
            break;
        case PCB_BLOCK:
            string_append(&estado_string, "BLOCK");
            break;
        case PCB_EXIT:
            string_append(&estado_string, "EXIT");
            break;     
        default:
            string_append(&estado_string, "UNKNOWN");
            break;  
    }
    return estado_string;
}

void log_list_of_chars(t_log* logger, t_list* list) {
    size_t size = list_size(list);
    for(size_t i = 0; i < size; i++) {
        char* item = list_get(list,i);
        log_info(logger,item);
    }
}

void log_pcb(t_log* logger, t_pcb* pcb) {
    log_info(logger, "Imprimiendo PCB en logger");
    log_info(logger, "PCB -> ID: %d", pcb->id_proceso);
    log_info(logger, "PCB -> PC: %d", pcb->program_counter);
    log_info(logger, "PCB -> ESTADO_ANTERIOR: %s", convertir_estado_pcb_a_string(pcb->estado_anterior));
    log_info(logger, "PCB -> ESTADO_ACTUAL: %s", convertir_estado_pcb_a_string(pcb->estado_actual));
    log_info(logger, "PCB -> INSTRUCCIONES:");
    log_list_of_chars(logger, pcb->instrucciones);
    log_info(logger, "PCB -> SEGMENTOS: No logro imprimirlos bien todavia");
    size_t size = list_size(pcb->tabla_segmentos);
    for(size_t i = 0; i < size; i++) {
        t_pcb_segmentos* item = list_get(pcb->tabla_segmentos,i);
        log_info(logger,"Segmento %d tamanio %d id_tabla_paginas %d", i, *item, *(item+sizeof(size_t)));
    }
}

static char* lista_de_pids(t_list* procesos) {
    
    void* pid_as_string(void* proceso) {
        t_pcb* p = proceso;
        return (void*) string_itoa(p->id_proceso);
    }

    t_list* pids_list = list_map(procesos,pid_as_string);
    char* pids_string = string_new();

    for(uint32_t index = 0; index < list_size(pids_list); index++) {
            char* pid = list_get(pids_list,index);
            if(index == list_size(pids_list) - 1) {
                string_append_with_format(&pids_string, "%s",pid);
            } else {
                string_append_with_format(&pids_string, "%s,",pid);
            }   
        }

    free(pids_list);
    return pids_string;
}

/**
 * Logs obligatiorios
 */

void log_proceso_en_new(t_log* logger, t_pcb* proceso) {
    log_info(logger, "Se crea el proceso <%d> en NEW", proceso->id_proceso);
}

void log_procesos_en_ready(t_log* logger, t_list* procesos_fifo, t_list* procesos_rr, char* algoritmo) {
    if(string_equals_ignore_case(algoritmo,"FEEDBACK")) {
		log_info(logger, "Cola Ready <%s>: [%s] [%s]", algoritmo, lista_de_pids(procesos_rr), lista_de_pids(procesos_fifo));
	} else if(string_equals_ignore_case(algoritmo,"RR")) {
        log_info(logger, "Cola Ready <%s>: [%s]", algoritmo, lista_de_pids(procesos_rr));
	} else {
        log_info(logger, "Cola Ready <%s>: [%s]", algoritmo, lista_de_pids(procesos_fifo));
    }
}

void log_proceso_cambio_de_estado(t_log* logger, t_pcb* proceso) {
    int id = proceso->id_proceso;
    char* estado_anterior = convertir_estado_pcb_a_string(proceso->estado_anterior);
    char* estado_actual = convertir_estado_pcb_a_string(proceso->estado_actual);
    log_info(logger, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <%s>", id, estado_anterior, estado_actual);
    free(estado_anterior);
    free(estado_actual);
}

void actualizar_estado_proceso(t_log* logger, t_pcb* proceso, t_estado_pcb nuevo_estado) {
	t_estado_pcb estado_aux;
	estado_aux = proceso->estado_actual;
	proceso->estado_actual = nuevo_estado;
	proceso->estado_anterior = estado_aux;
	log_proceso_cambio_de_estado(logger, proceso);
}


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
	size_t tamanio_pcb = 7 * sizeof(size_t) + 2 * sizeof(t_estado_pcb) + tamanio_instrucciones + tamanio_segmentos;
	*size = 
		sizeof(op_code) + 		    // codigo de operacion
		sizeof(size_t) +  		    // tamanio total del pcb
        sizeof(size_t) +		    // pcb.id_proceso
        sizeof(size_t) +		    // pcb.program_counter
        sizeof(size_t) +		    // pcb.registro_AX 
        sizeof(size_t) +		    // pcb.registro_BX 
        sizeof(size_t) +		    // pcb.registro_CX 
        sizeof(size_t) +		    // pcb.registro_DX
        sizeof(size_t) +		    // cantidad segmentos
        sizeof(t_estado_pcb) +		// pcb.estado_anterior
        sizeof(t_estado_pcb) +      // pcb.estado_actual
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