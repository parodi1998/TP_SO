

#include "../include/mmu.h"
#include "../include/tlb.h"
#include "../../shared/include/client_memoria.h"
#include "../../shared/include/sockets.h"
#include "../include/config_cpu.h"
#include "../include/instrucciones.h"

uint32_t TAMANIO_PAGINA;
uint32_t CANT_ENTRADAS_POR_TABLA;
uint32_t TAM_MAX_SEGMENTO;
int CONEXION_MEMORIA;

int SEGMENTATION_FAULT = -2;
int PAGE_FAULT = -1;

uint32_t segmento_page_fault = 0;
uint32_t pagina_page_fault = 0;

pthread_t hilo_tlb;
pthread_mutex_t mutex_tlb;



static bool conectar_cpu_a_memoria(int* fd) {
    *fd = crear_conexion(get_log(), "CPU_CLIENTE_MEMORIA", get_ip_memoria(),  get_puerto_memoria());
    return fd != 0;
}

bool iniciar_mmu(){

	CONEXION_MEMORIA = 0;
	if(!conectar_cpu_a_memoria(&CONEXION_MEMORIA)) {
        return false;
    }

	log_info(get_log(),"CONECTADO A MEMORIA, SOCKET: %d",CONEXION_MEMORIA);
	char* config_string = recibir_config_para_mmu(CONEXION_MEMORIA,get_log());
	char** parts = string_split(config_string,"|");
	CANT_ENTRADAS_POR_TABLA = (volatile uint32_t) atoi( parts[0]);
	TAMANIO_PAGINA = (volatile uint32_t) atoi( parts[1]);
	TAM_MAX_SEGMENTO = CANT_ENTRADAS_POR_TABLA * TAMANIO_PAGINA;
	free(parts);
	free(config_string);
	log_info(get_log(),"MMU INICIADA CORRECTAMENTE");
	init_tlb();
	return true;
}


int get_socket(){
	return CONEXION_MEMORIA;
}


t_translation_response_mmu* traducir_direccion_logica(int32_t pid,t_list* tabla_segmentos,int32_t dir_logica, int32_t es_escritura){
	pthread_mutex_lock(&mutex_tlb);
	t_translation_response_mmu* respuesta = malloc(sizeof(t_translation_response_mmu));

	respuesta->fue_page_fault = false;
	respuesta->fue_segmentation_fault = false;
	respuesta->pid = pid;

	//RETORNA LA DIR_FISICA
	log_info(get_log(),"Entre a traducir_direccion_logica");
	int32_t num_segmento = floor(dir_logica / TAM_MAX_SEGMENTO);
	int32_t desplazamiento_segmento = dir_logica % TAM_MAX_SEGMENTO;
	int32_t num_pagina = floor(desplazamiento_segmento  / TAMANIO_PAGINA);
	int32_t desplazamiento_pagina = desplazamiento_segmento % TAMANIO_PAGINA;

	respuesta->segmento = num_segmento;
	respuesta->pagina = num_pagina;

	bool tiene_num_segmento(t_segmento* aux){
		return aux->nro == num_segmento;
	}

	t_segmento* segmento = list_find(tabla_segmentos,(void*)tiene_num_segmento);
	if(segmento == NULL){
		log_info(get_log(),"segmento == NULL");
		respuesta->fue_segmentation_fault = true;
		pthread_mutex_unlock(&mutex_tlb);
		return respuesta;
	}

	if(desplazamiento_segmento >= segmento->tam){
		log_info(get_log(),"desplazamiento_segmento >= segmento->tam");
		respuesta->fue_segmentation_fault = true;
		pthread_mutex_unlock(&mutex_tlb);
		return respuesta;
	}



	int32_t limite_paginas_segmento = ceil((double) segmento->tam/ (double) TAMANIO_PAGINA);
	log_info(get_log(),"limite_paginas_segmento : %d",limite_paginas_segmento);
	if(num_pagina >= limite_paginas_segmento){
		log_info(get_log(),"num_pagina > paginas_del_segmento");
		respuesta->fue_segmentation_fault = true;
		pthread_mutex_unlock(&mutex_tlb);
		return respuesta;
	}





	uint32_t tlb_result = consult_tlb(pid,num_segmento, num_pagina); 
	uint32_t direccion_fisica = tlb_result;
	
	if(tlb_result == -1) {
		log_info(get_log(),"PID: %d - TLB MISS - Segmento: %d - Pagina: %d",pid,num_segmento,num_pagina);
		direccion_fisica = find_frame_in_memory_module(pid, num_segmento, num_pagina, es_escritura);
		if(direccion_fisica == PAGE_FAULT){
			respuesta->fue_page_fault = true;
			pthread_mutex_unlock(&mutex_tlb);
			verificar_pedidos_tlb();
			return respuesta;
		}
		respuesta->direccion_fisica = (direccion_fisica * TAMANIO_PAGINA) + desplazamiento_pagina;
		pthread_mutex_unlock(&mutex_tlb);
		return respuesta;
	}else{
		log_info(get_log(),"PID: %d - TLB HIT - Segmento: <%d> - Pagina: %d",pid,num_segmento,num_pagina);
		respuesta->direccion_fisica = (direccion_fisica * TAMANIO_PAGINA) + desplazamiento_pagina;
		pthread_mutex_unlock(&mutex_tlb);
		return respuesta;
	}
}


int32_t find_frame_in_memory_module(int32_t pid, int32_t segment,int32_t page, int32_t es_escritura) {

char* response_from_module = traducir_memoria(CONEXION_MEMORIA,get_log(),pid,segment,page,es_escritura);


char** parts = string_split(response_from_module, "|");

//verifico si hubo page fault
if (strcmp(parts[0],"1") == 0) {
	//hubo pagefault
	log_info(get_log(),"Fue PAGE_FAULT en pid: %d segmento: %d pagina: %d", pid, segment, page);
	return PAGE_FAULT;
} else {
	int32_t frame = atoi(parts[1]);
	//actualizo tlb
	update_tlb(pid,segment,page,frame);
	return frame;
}

}

void verificar_pedidos_tlb(){
	//funcion asincronica que actualiza la tlb quitando la entrada victima
	pthread_create(&hilo_tlb, NULL, (void*) recibir_actualizacion_tlb, NULL);
	pthread_detach(hilo_tlb);
}

void recibir_actualizacion_tlb(){
	pthread_mutex_lock(&mutex_tlb);
	bool active = true;
	int size;
	void* buffer;
	while(active){
		if(recv(CONEXION_MEMORIA, &size, sizeof(int), MSG_WAITALL) != -1){
			log_info(get_log(),"recibiendo info de memoria");
			buffer = malloc(size);
			recv(CONEXION_MEMORIA, buffer,size, MSG_WAITALL);
			log_info(get_log(),"RESPUESTA ACTUALIZAR TLB %s",(char*)buffer);
			if(strcmp((char*)buffer,"OK") == 0){
				active = false;
				free(buffer);
				pthread_mutex_unlock(&mutex_tlb);
				break;
			}else{
				char** array = string_split((char*)buffer,"|");
				uint32_t pid = (volatile uint32_t) atoi( array[0]);
				uint32_t segmento = (volatile uint32_t) atoi( array[1]);
				uint32_t pagina = (volatile uint32_t) atoi( array[2]);
				delete_entry_tlb(pid, segmento, pagina);
				active = false;
				free(buffer);
				pthread_mutex_unlock(&mutex_tlb);
				break;
			}
		}
	}
}




