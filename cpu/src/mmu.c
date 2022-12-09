

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
//	mockMMU();
	return true;
}

void mockMMU(){
	t_list* tabla_segmento = list_create();
	t_segmento* segmento = NULL;
	segmento = malloc(sizeof(t_segmento));
	segmento->nro = 0;
	segmento->nro_indice_tabla_paginas = 0;
	segmento->tam = 64;

	list_add(tabla_segmento,segmento );

	segmento = malloc(sizeof(t_segmento));
	segmento->nro = 1;
	segmento->nro_indice_tabla_paginas = 1;
	segmento->tam = 128;

	list_add(tabla_segmento,segmento );

	segmento = malloc(sizeof(t_segmento));
	segmento->nro = 2;
	segmento->nro_indice_tabla_paginas = 2;
	segmento->tam = 256;

	list_add(tabla_segmento,segmento );

	uint32_t pid =0;
	uint32_t rta;
	t_translation_response_mmu* response = NULL;
//	response = traducir_direccion_logica(0,tabla_segmento,45,1);
//	log_info(get_log(),"RTA: %d",response->direccion_fisica);
//	response = traducir_direccion_logica(0,tabla_segmento,257,1);
//	log_info(get_log(),"RTA: %d",response->direccion_fisica);


}

int get_socket(){
	return CONEXION_MEMORIA;
}


t_translation_response_mmu* traducir_direccion_logica(int32_t pid,t_list* tabla_segmentos,int32_t dir_logica, int32_t es_escritura){

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
		respuesta->fue_page_fault = true;
		return respuesta;
	}

	if(desplazamiento_segmento > segmento->tam){
		log_info(get_log(),"desplazamiento_segmento > segmento->tam");
		respuesta->fue_page_fault = true;
		return respuesta;;
	}

	log_info(get_log(),"num_segmento: %d", num_segmento);
	log_info(get_log(),"desplazamiento_segmento: %d", desplazamiento_segmento);
	log_info(get_log(),"num_pagina: %d", num_pagina);
	log_info(get_log(),"desplazamiento_pagina: %d", desplazamiento_pagina);

	uint32_t tlb_result = consult_tlb(pid,num_segmento, num_pagina); 
	uint32_t direccion_fisica = tlb_result;
	
	if(tlb_result == -1) {
		log_info(get_log(),"Fue TLB_MISS");
		direccion_fisica = find_frame_in_memory_module(pid, num_segmento, num_pagina, es_escritura);
		if(direccion_fisica == PAGE_FAULT){
			respuesta->fue_page_fault = true;
			return respuesta;
		}
	}
	respuesta->direccion_fisica = (direccion_fisica * TAMANIO_PAGINA) + desplazamiento_pagina;

	return respuesta;
}

// EN CASO DE OBTENER UN TLB MISS, LLAMAR A ESTA FUNCION PARA
// OBTENER EL FRAME DEL MODULO DE MEMORIA O UN PAGE FAULT EN SU DEFECTO

//SI SE OBTIENE UN PF, Se deberá realizar lo siguiente
/*
	 * devolver el contexto de ejecución al Kernel sin actualizar el valor del program counter.
	 * Deberemos indicarle al Kernel qué segmento y número de página
	 * fueron los que generaron el page fault para que éste resuelva el mismo.
	 *
	 */

int32_t find_frame_in_memory_module(int32_t pid, int32_t segment,int32_t page, int32_t es_escritura) {

char* response_from_module = traducir_memoria(CONEXION_MEMORIA,get_log(),pid,segment,page,es_escritura);


char** parts = string_split(response_from_module, "|");

//verifico si hubo page fault
if (parts[0] == "1") {
	//hubo pagefault
	log_info(get_log(),"Fue PAGE_FAULT en pid: %d segmento: %d pagina: %d", pid, segment, page);
	segmento_page_fault = segment;
	pagina_page_fault = page;
	return PAGE_FAULT;
} else {
	int32_t frame = atoi(parts[1]);
	//actualizo tlb
	update_tlb(pid,segment,page,frame);
	return frame;
}

}







