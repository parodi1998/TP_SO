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

typedef struct
{
	bool page_fault;
	int32_t frame;
}t_translation_response;

void init_tlb();
void update_fifo_pointer();
void update_lru();
void update_tlb(int32_t pid, int32_t segment, int32_t page, int32_t frame) ;
void find_frame_in_memory_module(int32_t pid, int32_t segment,int32_t page, int32_t es_escritura);
void finalize_process_tlb(int32_t pid);

#endif /* TLB_H_ */
