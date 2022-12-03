
#ifndef MMU_H_
#define MMU_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <commons/collections/list.h>
#include "tlb.h"

int32_t traducir_direccion_logica(int32_t pid,t_list* tabla_segmentos,int32_t dir_logica);
bool iniciar_mmu();
int find_frame_in_memory_module(int32_t pid, int32_t segment,int32_t page, int32_t es_escritura);
int get_socket();

#endif /* MMU_H_ */
