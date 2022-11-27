/*
 * tlb.c
 *
 *  Created on: 4 oct. 2022
 *      Author: utnso
 */
#include "../include/tlb.h"
#include "../include/config_cpu.h"
#include "../../shared/include/client_memoria.h"

t_list* SEGMENT_TABLE;

t_list* TLB;

pthread_mutex_t mutexTiempoTLB;
pthread_mutex_t mutexTLB;

int32_t TIEMPO_TLB;
int32_t PUNTERO_FIFO;

int32_t ENTRADAS_TLB;
char* REEMPLAZO_TLB;

int SOCKET_CLIENTE;
void init_tlb(int socket_cliente) {
	SOCKET_CLIENTE = socket_cliente;
	SEGMENT_TABLE = list_create();
	TLB = list_create();
	TIEMPO_TLB = 0;
	PUNTERO_FIFO = 0;
	ENTRADAS_TLB = get_entradas_tlb();
	REEMPLAZO_TLB = get_reemplazo_tlb();
	log_info(get_log(), "TLB inicializada correctamente");

}

int32_t consult_tlb(int32_t pid, int32_t segment, int32_t page){
	//busco si existe la entrada

	bool is_the_entry(t_tlb_entry* entry_aux) {
			return entry_aux->pid == pid && entry_aux->segment == segment
					&& entry_aux->page == page;
		}


		t_tlb_entry* record = list_find(TLB,(void*)is_the_entry);

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
	t_tlb_entry* record = list_find(TLB,(void*)is_the_entry );
	if(record == NULL){

		if(TLB->elements_count < ENTRADAS_TLB){
			record =  malloc(sizeof(t_tlb_entry));
			record->pid = pid;
			record->segment = segment;
			record->page = page;
			record->frame = frame;
			record->last_use = 0;

			list_add(TLB,(void*) record);
			return;
		}

		t_tlb_entry* victim;

		if(REEMPLAZO_TLB == "LRU"){

			bool sort_by_lru(t_tlb_entry* record_aux1,t_tlb_entry* record_aux2 ){
						return record_aux1->last_use > record_aux2->last_use;
			}
			update_lru();
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
		update_lru();
		record->frame = frame;
		record->last_use = 0;
	}


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

void find_frame_in_memory_module(int32_t pid, int32_t segment,int32_t page, int32_t es_escritura) {

char* response_from_module = traducir_memoria(SOCKET_CLIENTE,get_log(),pid,segment,page,es_escritura);


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
	int32_t frame = atoi(parts[0]);
	//actualizo tlb
	update_tlb(pid,segment,page,frame);
}

}

void finalize_process_tlb(int32_t pid){
	//eliminar todos los registros pertenecientes a la tlb
	bool has_pid(t_tlb_entry* aux){
		return aux->pid == pid;
	}

	void destroy_tlb_entry(t_tlb_entry* aux){
		free(aux);
	}

	list_remove_and_destroy_all_by_condition(TLB,(void*)has_pid,(void*)destroy_tlb_entry);
}



