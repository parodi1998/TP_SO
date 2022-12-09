
#ifndef MMU_H_
#define MMU_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <commons/collections/list.h>
#include "tlb.h"

extern uint32_t segmento_page_fault;
extern uint32_t pagina_page_fault;

typedef struct
{
	bool fue_page_fault;
	bool fue_segmentation_fault;
	int32_t pid;
	int32_t segmento;
	int32_t pagina;
	int32_t direccion_fisica;
}t_translation_response_mmu;

t_translation_response_mmu* traducir_direccion_logica(int32_t pid,t_list* tabla_segmentos,int32_t dir_logica, int32_t es_escritura);
bool iniciar_mmu();
int32_t find_frame_in_memory_module(int32_t pid, int32_t segment,int32_t page, int32_t es_escritura);
int get_socket();

#endif /* MMU_H_ */
