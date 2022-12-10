/*
 * tlb.h
 *
 *  Created on: 4 oct. 2022
 *      Author: utnso
 */

#ifndef TLB_H_
#define TLB_H_

#include<commons/collections/list.h>
#include<commons/log.h>
#include "config_cpu.h"
#include "../../shared/include/client_memoria.h"

typedef struct
{
	int32_t segment;
	int32_t size;
	int32_t table_page_id;
}t_segment_registry;

typedef struct
{
	int32_t pid;
	int32_t segment;
	int32_t page;
	int32_t frame;
	int32_t last_use;
}t_tlb_entry;

void init_tlb();
void update_fifo_pointer();
void update_tlb(int32_t pid, int32_t segment, int32_t page, int32_t frame) ;
void finalize_process_tlb(int32_t pid);

int32_t consult_tlb(int32_t pid, int32_t segment, int32_t page);
void imprimir_tlb();
void delete_entry_tlb(uint32_t pid, uint32_t segment, uint32_t page);

#endif /* TLB_H_ */
