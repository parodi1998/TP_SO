

#include "../include/mmu.h"
#include "../include/tlb.h"
#include "../include/client_memoria.h"
#include "../include/instrucciones.h"

uint32_t TAMANIO_PAGINA;
uint32_t CANT_ENTRADAS_POR_TABLA;
uint32_t TAM_MAX_SEGMENTO;


void iniciar_mmu(){
	char* config_string = recibir_config_para_mmu();
	char** parts = string_split("|",config_string);
	CANT_ENTRADAS_POR_TABLA = (volatile uint32_t) atoi( parts[0]);
	TAMANIO_PAGINA = (volatile uint32_t) atoi( parts[1]);
	free(config_string);
	TAM_MAX_SEGMENTO = CANT_ENTRADAS_POR_TABLA * TAMANIO_PAGINA;
}


int32_t traducir_direccion_logica(int32_t pid,t_list* tabla_segmentos,int32_t dir_logica){
	//RETORNA LA DIR_FISICA
	int32_t num_segmento = floor(dir_logica / TAM_MAX_SEGMENTO);
	int32_t desplazamiento_segmento = dir_logica % TAM_MAX_SEGMENTO;
	int32_t num_pagina = floor(desplazamiento_segmento  / TAMANIO_PAGINA);
	int32_t desplazamiento_pagina = desplazamiento_segmento % TAMANIO_PAGINA;

	bool tiene_num_segmento(t_segmento* aux){
		return aux->nro == num_segmento;
	}

	t_segmento* segmento = list_find(tabla_segmentos,(void*)tiene_num_segmento);
	if(segmento == NULL){
		return -1;
	}

	if(desplazamiento_segmento > segmento->tam){
		return -1;
	}

	return consult_tlb(pid,num_segmento, num_pagina);


}







