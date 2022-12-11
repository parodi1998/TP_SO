/*
 * tlb.c
 *
 *  Created on: 4 oct. 2022
 *      Author: utnso
 */
#include "../include/tlb.h"

t_list* SEGMENT_TABLE;

t_list* TLB;

pthread_mutex_t mutexTiempoTLB;
pthread_mutex_t mutexTLB;

int32_t TIEMPO_TLB;
int32_t PUNTERO_FIFO;
int32_t PUNTERO_LRU;

int32_t ENTRADAS_TLB;
char* REEMPLAZO_TLB;

int TLB_MISS =-1;

void init_tlb() {
	SEGMENT_TABLE = list_create();
	TLB = list_create();
	TIEMPO_TLB = 0;
	PUNTERO_FIFO = 0;
	PUNTERO_LRU = 0;
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
			record->last_use = PUNTERO_LRU++;

			return record->frame;

		}
		//no encontré,
		return TLB_MISS;

}


void update_tlb(int32_t pid, int32_t segment, int32_t page, int32_t frame) {

	bool is_the_entry(t_tlb_entry* entry_aux) {
		return entry_aux->pid == pid && entry_aux->segment == segment
				&& entry_aux->page == page;
	}
	if(ENTRADAS_TLB != 0){

	//busco si existe la entrada
	t_tlb_entry* record = list_find(TLB,(void*)is_the_entry );
	if(record == NULL){

		if(TLB->elements_count < ENTRADAS_TLB){
			record =  malloc(sizeof(t_tlb_entry));
			record->pid = pid;
			record->segment = segment;
			record->page = page;
			record->frame = frame;
			record->last_use = PUNTERO_LRU++;

			list_add(TLB,(void*) record);
			imprimir_tlb();
			return;
		}

		t_tlb_entry* victim;

		if(REEMPLAZO_TLB == "LRU"){

			bool sort_by_lru(t_tlb_entry* record_aux1,t_tlb_entry* record_aux2 ){
						return record_aux1->last_use < record_aux2->last_use;
			}
			t_list* list_sorted_by_time = list_sorted(TLB,(void*) sort_by_lru);
			victim = list_get(list_sorted_by_time,0);

			victim->pid = pid;
			victim->segment = segment;
			victim->page = page;
			victim->frame = frame;
			victim->last_use =PUNTERO_LRU++;

			list_destroy(list_sorted_by_time);
		}else{
			victim = list_get(TLB,PUNTERO_FIFO);
			victim->pid = pid;
			victim->segment = segment;
			victim->page = page;
			victim->frame = frame;
			victim->last_use =PUNTERO_LRU++;

			update_fifo_pointer();

		}


	}else{
		record->frame = frame;
		record->last_use = PUNTERO_LRU++;
	}
	imprimir_tlb();
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

void finalize_process_tlb(int32_t pid){
	//eliminar todos los registros pertenecientes a la tlb
	log_info(get_log(),"TLB - DELETE ALL - PID: %d",pid);
	bool has_pid(t_tlb_entry* aux){
		return aux->pid == pid;
	}

	void destroy_tlb_entry(t_tlb_entry* aux){
		free(aux);
	}

	list_remove_and_destroy_all_by_condition(TLB,(void*)has_pid,(void*)destroy_tlb_entry);
	imprimir_tlb();
}

void delete_entry_tlb(uint32_t pid, uint32_t segment, uint32_t page){

	log_info(get_log(),"TLB- DELETE ENTRY - PID: %d - SEGMENTO: %d - PAGINA: %d",pid,segment,page);

	bool deleted_success = false;

	void destroy_tlb_entry(t_tlb_entry* aux){
			free(aux);
		}

	t_tlb_entry* record =  NULL;


	for(int i=0;i<TLB->elements_count;i++){
		record = list_get(TLB,i);
		if(record->pid == pid && record->segment == segment && record->page == page){

			list_remove_and_destroy_element(TLB, i, (void*) destroy_tlb_entry);
			deleted_success = true;

			//verifico si el indice de FIFO apunta aca
			if(PUNTERO_FIFO == i){

				if(PUNTERO_FIFO == 0){
					PUNTERO_FIFO = TLB->elements_count -1;
				}else{
					PUNTERO_FIFO--;
				}
			}


		}
	}

	if(!deleted_success){
		log_info(get_log(),"No existe entrada TLB para eliminar");
	}else{
		log_info(get_log(),"Se elimino la entrada TLB");
	}
	imprimir_tlb();

}


void imprimir_tlb(){
	t_tlb_entry* entry = NULL;

	log_info(get_log(),"ESTADO TLB");
	log_info(get_log(),"///////////////////////////////");
	for(int i=0;i<TLB->elements_count;i++){
		entry = list_get(TLB,i);
		log_info(get_log(),"%d|PID:%d|SEGMENTO:%d|PAGINA:%d|MARCO:%d",i,entry->pid,entry->segment,entry->page,entry->frame);
	}
	log_info(get_log(),"///////////////////////////////");
}


