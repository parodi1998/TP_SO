#include "../include/protocolo.h"

void log_pcb(t_log* logger, t_pcb* proceso) {
	log_info(logger, "id_proceso: %d", proceso->id_proceso);
	for(int index = 0; index < list_size(proceso->instrucciones); index++) {
		char* instruccion = list_get(proceso->instrucciones, index);
		log_info(logger, "instruccion nro %d: %s", index, instruccion);
	}
	log_info(logger, "program_counter: %d", proceso->program_counter);
	log_info(logger, "registro_AX: %d", proceso->registro_AX);
	log_info(logger, "registro_BX: %d", proceso->registro_BX);
	log_info(logger, "registro_CX: %d", proceso->registro_CX);
	log_info(logger, "registro_DX: %d", proceso->registro_DX);
	for(int index = 0; index < list_size(proceso->tabla_segmentos); index++) {
		t_pcb_segmentos* segmento = list_get(proceso->tabla_segmentos, index);
		log_info(logger, "segmento nro %d tamanio: %d id_pagina: %d", index, segmento->tamanio_segmento, segmento->id_tabla_paginas);
	}
	log_info(logger, "estado_anterior: %d", proceso->estado_anterior);
	log_info(logger, "estado_actual: %d", proceso->estado_actual);
	log_info(logger, "consola_fd: %d", proceso->consola_fd);
	log_info(logger, "debe_ser_finalizado: %d", proceso->debe_ser_finalizado);
	log_info(logger, "debe_ser_bloqueado: %d", proceso->debe_ser_bloqueado);
	log_info(logger, "puede_ser_interrumpido: %d", proceso->puede_ser_interrumpido);
	log_info(logger, "fue_interrumpido: %d", proceso->fue_interrumpido);
	log_info(logger, "dispositivo_bloqueo: %s", proceso->dispositivo_bloqueo);
	log_info(logger, "registro_para_bloqueo: %d", proceso->registro_para_bloqueo);
	log_info(logger, "unidades_de_trabajo: %d", proceso->unidades_de_trabajo);
	log_info(logger, "page_fault_segmento: %d", proceso->page_fault_segmento);
	log_info(logger, "page_fault_pagina: %d", proceso->page_fault_pagina);
	
}

/**
 * Consola -> Kernel: Envio y recepcion de Instrucciones
 * */
static uint32_t calcular_tamanio_informacion_de_lista(t_log* logger, t_list* lista, uint32_t size_lista) {
	uint32_t tamanio_informacion = 0;
	if(list_is_empty(lista)) {
		tamanio_informacion = 0;
	} else {
		for(uint32_t i = 0; i<size_lista; i++) {
        	char* item = list_get(lista,i);
        	tamanio_informacion += sizeof(uint32_t) + string_length(item)+1;
    	}
	}
	return tamanio_informacion;
}

/**
 * PCB: envio y recepcion
 * */

static void* serializar_pcb(t_log* logger, uint32_t* size, t_pcb* pcb) {

    op_code cop = PCB_KERNEL;
	uint32_t cantidad_instrucciones = list_size(pcb->instrucciones);
	uint32_t tamanio_instrucciones = calcular_tamanio_informacion_de_lista(logger,pcb->instrucciones, cantidad_instrucciones);
    uint32_t cantidad_segmentos = list_size(pcb->tabla_segmentos);
	uint32_t tamanio_segmentos = 2 * sizeof(uint32_t) * cantidad_segmentos;
	uint32_t tamanio_dispositivo_bloqueo = string_length(pcb->dispositivo_bloqueo)+1;
	uint32_t tamanio_pcb = 14 * sizeof(uint32_t) + 7 * sizeof(bool) + 2 * sizeof(t_estado_pcb) + tamanio_dispositivo_bloqueo + tamanio_instrucciones + tamanio_segmentos;
	*size = 
		sizeof(op_code) + 		    // codigo de operacion
		sizeof(uint32_t) +  		    // tamanio total del pcb
        sizeof(uint32_t) +		    // pcb.id_proceso
        sizeof(uint32_t) +		    // pcb.program_counter
        sizeof(uint32_t) +		    // pcb.registro_AX 
        sizeof(uint32_t) +		    // pcb.registro_BX 
        sizeof(uint32_t) +		    // pcb.registro_CX 
        sizeof(uint32_t) +		    // pcb.registro_DX
        sizeof(uint32_t) +		    // pcb.consola_fd
        sizeof(uint32_t) +		    // cantidad segmentos
        sizeof(t_estado_pcb) +		// pcb.estado_anterior
        sizeof(t_estado_pcb) +      // pcb.estado_actual
        sizeof(bool) +              // pcb.debe_ser_finalizado
        sizeof(bool) +              // pcb.debe_ser_bloqueado
        sizeof(bool) +              // pcb.puede_ser_interrumpido
        sizeof(bool) +              // pcb.fue_interrumpido
		sizeof(bool) +              // pcb.finaliza_por_segmentation_fault
        sizeof(bool) +              // pcb.finaliza_por_error_instruccion
        sizeof(bool) +              // pcb.finaliza_por_error_de_ejecucion
        sizeof(uint32_t) +            // pcb.registro_para_bloqueo
        sizeof(uint32_t) +            // pcb.unidades_de_trabajo
        sizeof(uint32_t) +            // tamanio_dispositivo_bloqueo
        sizeof(uint32_t) +		    // pcb.page_fault_segmento
        sizeof(uint32_t) +		    // pcb.page_fault_pagina
        tamanio_dispositivo_bloqueo+// pcb.dispositivo_bloqueo
        tamanio_segmentos +         // 2 uint32_t * cantidad segmentos
		sizeof(uint32_t) +		    // cantidad de instrucciones
		tamanio_instrucciones;	    // (largo de instruccion + instruccion) * cantidad de instrucciones 

    void* stream = malloc(*size);
	void* p = stream;

	memcpy(p, &cop, sizeof(op_code)); // guardo el codigo de operacion
	p += sizeof(op_code);

    memcpy(p,&tamanio_pcb,sizeof(uint32_t)); // guardo el tamaño de toda la info a enviar, en este caso el pcb
	p += sizeof(uint32_t);

	memcpy(p,&pcb->id_proceso,sizeof(uint32_t)); // guardo el id del pcb
	p += sizeof(uint32_t);

    memcpy(p,&pcb->program_counter,sizeof(uint32_t)); // guardo el program_counter del pcb
	p += sizeof(uint32_t);

    memcpy(p,&pcb->registro_AX,sizeof(uint32_t)); // guardo el registro_AX del pcb
	p += sizeof(uint32_t);

    memcpy(p,&pcb->registro_BX,sizeof(uint32_t)); // guardo el registro_BX del pcb
	p += sizeof(uint32_t);

    memcpy(p,&pcb->registro_CX,sizeof(uint32_t)); // guardo el registro_CX del pcb
	p += sizeof(uint32_t);

    memcpy(p,&pcb->registro_DX,sizeof(uint32_t)); // guardo el registro_DX del pcb
	p += sizeof(uint32_t);

    memcpy(p,&pcb->consola_fd,sizeof(uint32_t)); // guardo el fd consola del pcb
	p += sizeof(uint32_t);

    memcpy(p,&pcb->page_fault_segmento,sizeof(uint32_t)); // guardo el page_fault_segmento del pcb
	p += sizeof(uint32_t);

    memcpy(p,&pcb->page_fault_pagina,sizeof(uint32_t)); // guardo el page_fault_pagina del pcb
	p += sizeof(uint32_t);

    memcpy(p,&pcb->debe_ser_finalizado,sizeof(bool)); // guardo el estado debe_ser_finalizado del pcb
	p += sizeof(bool);

    memcpy(p,&pcb->debe_ser_bloqueado,sizeof(bool)); // guardo el estado debe_ser_bloqueado del pcb
	p += sizeof(bool);

    memcpy(p,&pcb->puede_ser_interrumpido,sizeof(bool)); // guardo el estado puede_ser_interrumpido del pcb
	p += sizeof(bool);

    memcpy(p,&pcb->fue_interrumpido,sizeof(bool)); // guardo el estado fue_interrumpido del pcb
	p += sizeof(bool);

	memcpy(p,&pcb->finaliza_por_segmentation_fault,sizeof(bool)); // guardo el estado finaliza_por_segmentation_fault del pcb
	p += sizeof(bool);

	memcpy(p,&pcb->finaliza_por_error_instruccion,sizeof(bool)); // guardo el estado finaliza_por_error_instruccion del pcb
	p += sizeof(bool);

	memcpy(p,&pcb->finaliza_por_error_de_ejecucion,sizeof(bool)); // guardo el estado finaliza_por_error_de_ejecucion del pcb
	p += sizeof(bool);

    memcpy(p,&pcb->registro_para_bloqueo,sizeof(uint32_t)); // guardo el index del registro_para_bloqueo del pcb
	p += sizeof(uint32_t);

    memcpy(p,&pcb->unidades_de_trabajo,sizeof(uint32_t)); // guardo las unidades_de_trabajo para el bloqueo del pcb
	p += sizeof(uint32_t);

	memcpy(p,&tamanio_dispositivo_bloqueo,sizeof(uint32_t));
	p += sizeof(uint32_t);

	memcpy(p,pcb->dispositivo_bloqueo,tamanio_dispositivo_bloqueo);
	p += tamanio_dispositivo_bloqueo;

    memcpy(p,&pcb->estado_anterior,sizeof(t_estado_pcb)); // guardo el estado anterior del pcb
	p += sizeof(t_estado_pcb);

    memcpy(p,&pcb->estado_actual,sizeof(t_estado_pcb)); // guardo el estado actual del pcb
	p += sizeof(t_estado_pcb);

    memcpy(p,&cantidad_segmentos,sizeof(uint32_t)); // guardo la cantidad_segmentos del pcb
	p += sizeof(uint32_t);

    for(uint32_t i = 0; i<cantidad_segmentos; i++) {
        t_pcb_segmentos* segmento = list_get(pcb->tabla_segmentos,i);
        memcpy(p,&segmento->tamanio_segmento,sizeof(uint32_t)); // guardo el tamanio_segmento
	    p += sizeof(uint32_t);
        memcpy(p,&segmento->id_tabla_paginas,sizeof(uint32_t)); // guardo el id_tabla_paginas
	    p += sizeof(uint32_t);
    }

    memcpy(p,&cantidad_instrucciones,sizeof(uint32_t)); // guardo la cantidad de instrucciones
	p += sizeof(uint32_t);

	for(uint32_t i = 0; i<cantidad_instrucciones; i++) {
        char* instruccion = list_get(pcb->instrucciones,i);
		uint32_t tamanio_instruccion = string_length(instruccion)+1;
		memcpy(p,&tamanio_instruccion,sizeof(uint32_t));
		p += sizeof(uint32_t);
		memcpy(p,instruccion,tamanio_instruccion);
		p += tamanio_instruccion;
    }

    return stream;
}

static void deserializar_pcb(t_log* logger, void* stream, t_pcb** pcb) {
    // se supone que si llegue aca, ya saque el codigo de operacion del paquete
	uint32_t cantidad_instrucciones;
    uint32_t cantidad_segmentos;
    t_pcb* pcb_aux = malloc(sizeof(t_pcb));
	t_list* instrucciones_aux = list_create();
    t_list* segmentos_aux = list_create();
	void* p = stream;

    memcpy(&pcb_aux->id_proceso, p, sizeof(uint32_t));	
	p += sizeof(uint32_t);

    memcpy(&pcb_aux->program_counter, p, sizeof(uint32_t));	
	p += sizeof(uint32_t);

    memcpy(&pcb_aux->registro_AX, p, sizeof(uint32_t));	
	p += sizeof(uint32_t);

    memcpy(&pcb_aux->registro_BX, p, sizeof(uint32_t));	
	p += sizeof(uint32_t);

    memcpy(&pcb_aux->registro_CX, p, sizeof(uint32_t));	
	p += sizeof(uint32_t);

    memcpy(&pcb_aux->registro_DX, p, sizeof(uint32_t));	
	p += sizeof(uint32_t);

    memcpy(&pcb_aux->consola_fd, p, sizeof(uint32_t));	
	p += sizeof(uint32_t);
    
    memcpy(&pcb_aux->page_fault_segmento, p, sizeof(uint32_t));	
	p += sizeof(uint32_t);

    memcpy(&pcb_aux->page_fault_pagina, p, sizeof(uint32_t));	
	p += sizeof(uint32_t);

    memcpy(&pcb_aux->debe_ser_finalizado, p, sizeof(bool));	
	p += sizeof(bool);

    memcpy(&pcb_aux->debe_ser_bloqueado, p, sizeof(bool));	
	p += sizeof(bool);

    memcpy(&pcb_aux->puede_ser_interrumpido, p, sizeof(bool));	
	p += sizeof(bool);

    memcpy(&pcb_aux->fue_interrumpido, p, sizeof(bool));	
	p += sizeof(bool);

	memcpy(&pcb_aux->finaliza_por_segmentation_fault, p, sizeof(bool));	
	p += sizeof(bool);

	memcpy(&pcb_aux->finaliza_por_error_instruccion, p, sizeof(bool));	
	p += sizeof(bool);

	memcpy(&pcb_aux->finaliza_por_error_de_ejecucion, p, sizeof(bool));	
	p += sizeof(bool);

    memcpy(&pcb_aux->registro_para_bloqueo, p, sizeof(uint32_t));	
	p += sizeof(uint32_t);

    memcpy(&pcb_aux->unidades_de_trabajo, p, sizeof(uint32_t));	
	p += sizeof(uint32_t);

    uint32_t tamanio_dispositivo_bloqueo;
    memcpy(&tamanio_dispositivo_bloqueo, p, sizeof(uint32_t));	
	p += sizeof(uint32_t);

    char* dispositivo_bloqueo = malloc(tamanio_dispositivo_bloqueo);
	memcpy(dispositivo_bloqueo, p, tamanio_dispositivo_bloqueo);
    pcb_aux->dispositivo_bloqueo = dispositivo_bloqueo;
	p += tamanio_dispositivo_bloqueo;
    
    memcpy(&pcb_aux->estado_anterior, p, sizeof(t_estado_pcb));	
	p += sizeof(t_estado_pcb);

    memcpy(&pcb_aux->estado_actual, p, sizeof(t_estado_pcb));	
	p += sizeof(t_estado_pcb);

    memcpy(&cantidad_segmentos, p, sizeof(uint32_t));	
	p += sizeof(uint32_t);

    for(uint32_t i = 0; i<cantidad_segmentos; i++) {
		uint32_t tamanio_segmento;
        uint32_t id_tabla_paginas;
		memcpy(&tamanio_segmento, p, sizeof(uint32_t));
		p += sizeof(uint32_t);
        memcpy(&id_tabla_paginas, p, sizeof(uint32_t));
		p += sizeof(uint32_t);
		t_pcb_segmentos* segmento_pcb = malloc(sizeof(t_pcb_segmentos));
        segmento_pcb->id_tabla_paginas = id_tabla_paginas;
        segmento_pcb->tamanio_segmento = tamanio_segmento;
        list_add(segmentos_aux, segmento_pcb);
    }

	memcpy(&cantidad_instrucciones, p, sizeof(uint32_t));	
	p += sizeof(uint32_t);
	
	for(uint32_t i = 0; i<cantidad_instrucciones; i++) {
		uint32_t tamanio_instruccion;
		memcpy(&tamanio_instruccion, p, sizeof(uint32_t));
		p += sizeof(uint32_t);
		char* instruccion = malloc(tamanio_instruccion);
		memcpy(instruccion, p, tamanio_instruccion);
		p += tamanio_instruccion;
		list_add(instrucciones_aux, instruccion);
    }
	pcb_aux->instrucciones = instrucciones_aux;
    pcb_aux->tabla_segmentos = segmentos_aux;
    *pcb = pcb_aux;
}

bool send_pcb(t_log* logger, int fd, t_pcb* pcb) {
    uint32_t size;
    void* stream = serializar_pcb(logger, &size, pcb);
    if (send(fd, stream, size, 0) == -1) {
        free(stream);
        return false;
    }
    free(stream);
    return true;
}

bool recv_pcb(t_log* logger,int fd, t_pcb** pcb) {
    // tamanio total del stream
    uint32_t size;
    if (recv(fd, &size, sizeof(uint32_t), 0) != sizeof(uint32_t)) {
        return false;
    }
    // recibe TODO el stream
    void* stream = malloc(size);
    if (recv(fd, stream, size, 0) != size) {
        free(stream);
        return false;
    }

    t_pcb* pcb_aux; // el malloc lo realiza la funcion deserializar
	
    deserializar_pcb(logger, stream, &pcb_aux);

    *pcb = pcb_aux;

    free(stream);
    return true;
}

void liberar_pcb(t_pcb* proceso) {

	free(proceso->dispositivo_bloqueo);

    list_destroy_and_destroy_elements(proceso->instrucciones,free);
    list_destroy_and_destroy_elements(proceso->tabla_segmentos,free);
    free(proceso);
}

static void enviar_mensaje_consola(char* mensaje, int socket_cliente, op_code codigo_operacion)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

		paquete->codigo_operacion = codigo_operacion;
		paquete->buffer = malloc(sizeof(t_buffer));
		paquete->buffer->size = strlen(mensaje) + 1;
		paquete->buffer->stream = malloc(paquete->buffer->size);
		memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

		int bytes = paquete->buffer->size + 2*sizeof(int);

		void* a_enviar = serializar_paquete(paquete, bytes);

		send(socket_cliente, a_enviar, bytes, 0);

		free(a_enviar);

}

static char* recibir_mensaje_consola(int socket_cliente, t_log* logger)
{
	void * buffer;
	int size;

	recv(socket_cliente, &size, sizeof(int), MSG_WAITALL);

	log_info(logger,"size recibir mensaje consola: %d", size);

	buffer = malloc(size);
	recv(socket_cliente, buffer,size, MSG_WAITALL);

	return buffer;
}

char* finalizar_proceso_consola(int socket_cliente, t_log* logger){

	log_info(logger,"REQUEST FINALIZAR_PROCESO en CONSOLA");
	enviar_mensaje_consola("",socket_cliente,CONSOLA_EXIT);
	char* rta = recibir_mensaje_consola(socket_cliente, logger);
	log_info(logger,"RESPUESTA FINALIZAR_PROCESO en CONSOLA: %s" , rta );
	return rta;
}

bool send_op_code(t_log* logger, uint32_t fd, op_code cod_op) {
	return send(fd, &cod_op, sizeof(op_code), 0) != -1;
}

bool recv_op_code(t_log* logger, uint32_t fd, op_code* cod_op) {
	return recv(fd, cod_op, sizeof(op_code), MSG_WAITALL) == -1;
}
