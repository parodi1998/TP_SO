/*
 * tlb.c
 *
 *  Created on: 4 oct. 2022
 *      Author: utnso
 */
#include "tlb.h"
#include "config_cpu.h"

t_list* SEGMENT_TABLE;

t_list* TLB;

pthread_mutex_t mutexTiempoTLB;
pthread_mutex_t mutexTLB;

int32_t TIEMPO_TLB;
int32_t PUNTERO_FIFO;

t_log* LOGGER_1;

int32_t ENTRADAS_TLB;
char* REEMPLAZO_TLB;

void init_tlb() {
	SEGMENT_TABLE = list_create();
	TLB = list_create();
	TIEMPO_TLB = 0;
	PUNTERO_FIFO = 0;
	LOGGER_1 = get_log();
	ENTRADAS_TLB = get_entradas_tlb();
	REEMPLAZO_TLB = get_reemplazo_tlb();
	log_info(LOGGER_1, "TLB inicializada correctamente");

}

int32_t consult_tlb(int32_t pid, int32_t segment, int32_t page){
	//busco si existe la entrada

	bool is_the_entry(t_tlb_entry* entry_aux) {
			return entry_aux->pid == pid && entry_aux->segment == segment
					&& entry_aux->page == page;
		}


		t_tlb_entry* record = list_get(TLB,(void*)is_the_entry );

		//actualizo el tiempo de consulta y retorno frame
		if(record != NULL){
			update_lru();
			record->last_use = 0;

			return record->frame;

		}
		//no encontré,
		update_lru();
		return -1;

}


void update_tlb(int32_t pid, int32_t segment, int32_t page, int32_t frame) {

	bool is_the_entry(t_tlb_entry* entry_aux) {
		return entry_aux->pid == pid && entry_aux->segment == segment
				&& entry_aux->page == page;
	}

	//busco si existe la entrada
	t_tlb_entry* record = list_get(TLB,(void*)is_the_entry );
	if(record == NULL){

		if(TLB->elements_count < ENTRADAS_TLB){
			record =  malloc(sizeof(t_tlb_entry));
			record->pid = pid;
			record->segment = segment;
			record->page = page;
			record->frame = frame;
			record->last_use = 0;

			list_add(TLB,(void*) record);
		}

		t_tlb_entry* victim;

		if(REEMPLAZO_TLB == "LRU"){

			bool sort_by_lru(t_tlb_entry* record_aux1,t_tlb_entry* record_aux2 ){
						return record_aux1->last_use > record_aux2;
			}

			t_list* list_sorted_by_time = list_sorted(TLB,(void*) sort_by_lru);
			victim = list_get(list_sorted_by_time,0);

			victim->pid = pid;
			victim->segment = segment;
			victim->page = page;
			victim->frame = frame;
			victim->last_use =0;

			list_destroy(list_sorted_by_time);
		}else{
			victim = list_get(TLB,PUNTERO_FIFO);
			victim->pid = pid;
			victim->segment = segment;
			victim->page = page;
			victim->frame = frame;
			victim->last_use =0;

			update_fifo_pointer();

		}


	}else{
		record->frame = frame;
		record->last_use = 0;
	}

	update_lru();


}

void update_fifo_pointer(){

	if(PUNTERO_FIFO < (ENTRADAS_TLB -1)){
			PUNTERO_FIFO++;
	}

	if(PUNTERO_FIFO == (ENTRADAS_TLB -1) ){
		PUNTERO_FIFO =0;
	}
}

void update_lru(){
	t_tlb_entry* record;
	for(int i=0;i<ENTRADAS_TLB;i++){
		record = list_get(TLB,i);

		record->last_use++;
	}
}

void find_frame_in_memory_module(int32_t pid, int32_t segment,int32_t page) {

char* response_from_module = "0|0";




char** parts = string_split(response_from_module, "|");

//verifico si hubo page fault
if (parts[0] == "1") {
	//hubo pagefault

	/*
	 * devolver el contexto de ejecución al Kernel sin actualizar el valor del program counter.
	 * Deberemos indicarle al Kernel qué segmento y número de página
	 * fueron los que generaron el page fault para que éste resuelva el mismo.
	 *
	 */
} else {
	int32_t frame = parts[0];
	//actualizo tlb
	update_tlb(pid,segment,page,frame);
}

}

void write_value_from_memory_module(int32_t address, void* value) {

}

void* read_value_from_memory_module(int32_t address) {
return (void*) "valor";
}




