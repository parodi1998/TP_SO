#include "../include/memory_file_management.h"
#include "../include/memory_configuration_manager.h"
#include "../include/server.h"
#include <math.h>


//VARIABLES GLOBALES
void* MEMORY_BLOCK;
char* PATH_SWAP_BASE;
int SWAP_FILE;
void* MEMORY_BLOCK_SECONDARY;
int TABLE_ID_GENERATOR = 0;

//mutex
pthread_mutex_t mutex_clock;
pthread_mutex_t mutex_swap;
pthread_mutex_t mutex_frames;
pthread_mutex_t mutex_frames_swap;
pthread_mutex_t mutex_table_pages;
pthread_mutex_t mutex_memory_block;

t_list* TABLE_PAGES;
t_list* FRAMES;
t_list* FRAMES_SWAP;
int32_t POSITION_CLOCK;
void initialize_memory_file_management() {
	PATH_SWAP_BASE = swap_path();
	initialize_memory();
}

void initialize_memory() {
	uint32_t size = memory_size_getter();
	MEMORY_BLOCK = malloc(size);
	init_structs_and_lists();
	create_swap_file();
}

void save_value_in_memory(uint32_t pid ,uint32_t address, void* value, uint32_t size) {
	pthread_mutex_lock(&mutex_memory_block);
	memcpy(MEMORY_BLOCK + address, value, size);
	pthread_mutex_unlock(&mutex_memory_block);
	log_info(get_logger(),"RETARDO MEMORIA START");
	usleep(memory_time());
	log_info(get_logger(),"RETARDO MEMORIA END");
	log_info(get_logger(),"PID: %d - Acción: ESCRIBIR - Dirección física: %d",pid,address);
}

void* read_value_in_memory(uint32_t pid ,uint32_t address, uint32_t size) {
	void* value = malloc(size);
	pthread_mutex_lock(&mutex_memory_block);
	memcpy(value, MEMORY_BLOCK + address, size);
	pthread_mutex_unlock(&mutex_memory_block);
	log_info(get_logger(),"RETARDO MEMORIA START");
	usleep(memory_time());
	log_info(get_logger(),"RETARDO MEMORIA END");
	log_info(get_logger(), "PID: %d - Acción: LEER - Dirección física: %d",pid, address);
	return value;
}

void save_content_in_swap_file(uint32_t address, uint32_t size,uint32_t logical_address) {

	pthread_mutex_lock(&mutex_swap);

	memcpy(MEMORY_BLOCK_SECONDARY + logical_address, MEMORY_BLOCK + address,size);
	msync(MEMORY_BLOCK_SECONDARY, swap_size_getter(), MS_SYNC);
	pthread_mutex_unlock(&mutex_swap);
	log_info(get_logger(),"RETARDO SWAP START");
	usleep(swap_time());
	log_info(get_logger(),"RETARDO SWAP END");
}

void create_swap_file() {
	uint32_t size = swap_size_getter();
	SWAP_FILE = open(PATH_SWAP_BASE, O_CREAT | O_RDWR, 07777);
	if (SWAP_FILE == -1) {
		log_info(get_logger(), "No se pudo crear el archivo de swap %s",PATH_SWAP_BASE);
	} else {
		//inicio de la memoria secundaria con mmap
		MEMORY_BLOCK_SECONDARY = mmap(NULL, size, PROT_WRITE | PROT_READ,
		MAP_SHARED | MAP_FILE, SWAP_FILE, 0);
		ftruncate(SWAP_FILE, size);
		msync(MEMORY_BLOCK_SECONDARY, size, MS_SYNC);
		log_info(get_logger(), "SWAP FILE CREATED");
	}
}

void load_file_into_memory(uint32_t address, uint32_t size,
		uint32_t logical_address) {
	pthread_mutex_lock(&mutex_swap);

	memcpy(MEMORY_BLOCK + address, MEMORY_BLOCK_SECONDARY + logical_address,size);
	pthread_mutex_unlock(&mutex_swap);
	log_info(get_logger(),"RETARDO SWAP START");
	usleep(swap_time());
	log_info(get_logger(),"RETARDO SWAP END");
}

void init_structs_and_lists() {

	int cant_frames = memory_size_getter() / page_size_getter();

	int cant_frames_swap = swap_size_getter() / page_size_getter();

	log_info(get_logger(), "Hay %d frames de %d bytes en memoria principal",
			cant_frames, page_size_getter());

	//Creamos lista de tablas de paginas

	TABLE_PAGES = list_create();

	//Creamos lista de frames de memoria real
	FRAMES = list_create();

	FRAMES_SWAP = list_create();

	t_frame* frame = NULL;

	for (int i = 0; i < cant_frames; i++) {

		frame = malloc(sizeof(t_frame));
		frame->is_free = true;
		frame->id = i;
		frame->pid = -1;

		list_add(FRAMES, frame);
	}

	//creamos lista de frames de memoria virtual

	t_frame_swap* frame_swap = NULL;

	for (int i = 0; i < cant_frames_swap; i++) {

		frame_swap = malloc(sizeof(t_frame_swap));

		frame_swap->is_free = true;
		frame_swap->pos = i;

		list_add(FRAMES_SWAP, frame_swap);
	}

	POSITION_CLOCK = 0;

	log_info(get_logger(), "Estructuras administrativas inicializadas");
}

int32_t initialize_process(uint32_t pid, uint32_t segment_ID, uint32_t size) {
	//verifico si ya se ocuparon la cantidad maxima admitida de procesos en memoria
	int number_of_pages_to_create = ceil((double) size / (double) page_size_getter());


	if(number_of_pages_to_create > entries_per_table_getter()){
		log_info(get_logger(),"Error: se supero el limite de entradas por tabla permitido para el PID %d, Segmento: %d",pid, segment_ID);
		return FAILURE;
	}


	log_info(get_logger(),"Se encontro espacio libre para el PID %d, SEGMENTO %d",pid,segment_ID);

	bool is_free_and_unasigned(t_frame* frame) {
		return frame->is_free && frame->pid == -1;
	}
	pthread_mutex_lock(&mutex_frames);
	t_list* free_frames_MP = list_filter(FRAMES, (void*) is_free_and_unasigned);
	if (list_size(free_frames_MP) < frames_per_table_getter()) {

		log_info(get_logger(),"Error: se supero el nivel de multiprogramacion, no hay frames libres en MP para el PID %d, Segmento: %d",pid, segment_ID);
		list_destroy(free_frames_MP);
		pthread_mutex_unlock(&mutex_frames);
		return FAILURE;
	}

	pthread_mutex_unlock(&mutex_frames);
	list_destroy(free_frames_MP);
	//creacion de tabla de paginas

	book_frames(pid);

	table_page* new_table = malloc(sizeof(table_page));
	new_table->pages = list_create();
	new_table->PID = pid;
	new_table->ID = TABLE_ID_GENERATOR++;
	new_table->segment_ID = segment_ID;

	pthread_mutex_lock(&mutex_table_pages);
	list_add(TABLE_PAGES, new_table);
	pthread_mutex_unlock(&mutex_table_pages);

	//Crea las pages y las guarda en memoria
	for (int i = 0; i < number_of_pages_to_create; i++) {

		t_page* page = malloc(sizeof(t_page));
		page->pid = pid;
		page->segment = segment_ID;
		page->id = i;
		page->frame = -1;
		page->modified = false;
		page->locked = false;
		page->present = false;
		page->used = true;
		page->pos_swap = -1;

		list_add(new_table->pages, page);
		log_info(get_logger(), "Se creo la pagina %d con frame %d para el PID %d", i,page->frame, pid);
		log_info(get_logger(),"PID: <%d> - Segmento: <%d> - TAMAÑO: <%d> paginas",pid,segment_ID,number_of_pages_to_create);
		page->locked = false;
	}

	//retornamos el id de la pagina de primer nivel
	log_info(get_logger(), "Se crea la tabla de paginas ID: %d  al PID: %d, SEGMENTO %d",new_table->ID, pid,segment_ID);

	return new_table->ID;

}

int32_t suspend_process(uint32_t pid, uint32_t segment) {
	log_info(get_logger(), "Se suspende el proceso %d , segmento %d...", pid,segment);
	table_page* table = find_table_with_pid(pid, segment);
	t_page* page;
	t_frame* frame;
	for (int i = 0; i < table->pages->elements_count; i++) {
		page = (t_page*) list_get(table->pages, i);
		page->locked = true;
		if (page->present) {
			//vacio el frame que ocupa
			send_swap(page);
			page->present = false;
			page->locked = false;

			pthread_mutex_lock(&mutex_frames);
			frame = (t_frame*) list_get(FRAMES, page->frame);
			frame->is_free = true;
			frame->pid = -1;
			pthread_mutex_unlock(&mutex_frames);

			log_info(get_logger(),"[Proceso %d]Se vacia con exito la pagina %d y se libera frame nro %d",pid, page->id, frame->id);

		}
	}

	return SUCCESS;
}

int32_t finalize_process(uint32_t pid) {
	log_info(get_logger(), "Comienza la finalizacion del proceso %d ..",pid);
	table_page* table = NULL;
	t_list* list = get_all_table_pages_from_pid(pid);
	t_page* page = NULL;
	t_frame* frame = NULL;

	//elimino los frames libres no asignados
	clean_free_frames_from_pid(pid);

	for(int i=0;i<list->elements_count;i++){
		table = list_get(list,i);



		for (int i = 0; i < table->pages->elements_count; i++) {
			page = (t_page*) list_get(table->pages, i);
			page->locked = true;
			if (page->present) {
				//vacio el frame que ocupa
				page->present = false;
				page->locked = false;

				pthread_mutex_lock(&mutex_frames);
				frame = (t_frame*) list_get(FRAMES, page->frame);
				frame->is_free = true;
				frame->pid = -1;
				pthread_mutex_unlock(&mutex_frames);
				t_frame_swap* frame_swap;
				//vacio (si tiene) el frame ocupado en swap
				if (page->pos_swap != -1) {
					pthread_mutex_lock(&mutex_frames_swap);
					frame_swap = find_frame_swap(page->pos_swap);
					frame_swap->is_free = true;
					pthread_mutex_unlock(&mutex_frames_swap);
				}
					log_info(get_logger(), "[Proceso %d, Segmento %d]Se libera frame nro %d",pid, page->segment, frame->id);
			}
		}

	}
	list_destroy(list);
	log_info(get_logger(), "Se finalizo el proceso %d con exito", pid);

	return SUCCESS;
}

t_list* get_all_table_pages_from_pid(uint32_t pid){
	bool find_table_pages_with_pid(table_page* table_aux){
		return table_aux->PID == pid;
	}
	pthread_mutex_lock(&mutex_table_pages);
	t_list* list = list_filter(TABLE_PAGES,(void*)find_table_pages_with_pid);
	pthread_mutex_unlock(&mutex_table_pages);

	return list;
}

void clean_free_frames_from_pid(uint32_t pid){
	int same_pid_and_free(t_frame* frame_aux) {
			return (frame_aux->pid == pid && frame_aux->is_free);
		}
	pthread_mutex_lock(&mutex_frames);
	t_list* list_frames_unasigned = list_filter(FRAMES, (void*) same_pid_and_free);
	t_frame* frame = NULL;
	for(int i=0;i<list_frames_unasigned->elements_count;i++){
		frame = list_get(list_frames_unasigned,i);
		if(frame->pid == pid){
			frame->is_free = true;
			frame->pid = -1;
			log_info(get_logger(),"liberando frame %d de PID %d",frame->id,pid);
		}
	}
	pthread_mutex_unlock(&mutex_frames);
}




void delete_swap_file(uint32_t pid) {
	char* path = swap_path();
	close(SWAP_FILE);
	pthread_mutex_lock(&mutex_swap);
	log_info(get_logger(), "Eliminando el archivo swap %s", path);
	remove(path);
	pthread_mutex_unlock(&mutex_swap);
	free(path);
}

table_page* find_table_with_pid(int32_t pid, int32_t segment_ID) {

	table_page* table;
	log_info(get_logger(), "Buscamos la tabla del PID %d y segmento %d ...", pid,segment_ID);

	bool same_id(table_page* table_aux) {
		return (table_aux->PID == pid && table_aux->segment_ID == segment_ID);
	}

	pthread_mutex_lock(&mutex_table_pages);
	table = list_find(TABLE_PAGES, (void*) same_id);
	pthread_mutex_unlock(&mutex_table_pages);
	log_info(get_logger(), "Encontramos la tabla del PID %d, segmento: %d", pid,segment_ID);
	return table;
}

uint32_t get_free_frame(uint32_t pid) {

	pthread_mutex_lock(&mutex_frames);
	t_list* free_frame_process = get_free_frames_from_process(pid);
	t_frame* frame = list_get(free_frame_process, 0);

	if (frame != NULL) {
		frame->is_free = false;
		frame->pid = pid;

		pthread_mutex_unlock(&mutex_frames);
		int id = frame->id;
		log_info(get_logger(), "Se reserva el frame %d", frame->id);
		list_destroy(free_frame_process);
		return id;
	}

	list_destroy(free_frame_process);
	pthread_mutex_unlock(&mutex_frames);

	return execute_swapping(pid)->frame  ;
}

t_page* execute_swapping(uint32_t pid) {

	int pos_victim;
	t_list* pages;

	t_list* process_pages = find_all_pages_from_process(pid);

	int in_mp_and_not_locked(t_page* page) {
		return !page->locked && page->present;
	}

	pages = list_filter(process_pages, (void*) in_mp_and_not_locked);
	list_destroy(process_pages);

	if (string_equals_ignore_case(algorithm(), "CLOCK-M")) {

		bool index_frame(void* ele1, void* ele2) {
			t_page* pag1 = (t_page*) ele1;
			t_page* pag2 = (t_page*) ele2;
			return pag1->frame < pag2->frame;
		}

		list_sort(pages, index_frame);

		pthread_mutex_lock(&mutex_clock);
		pos_victim = clock_M(pages);
		pthread_mutex_unlock(&mutex_clock);
	} else {
		bool index_frame(void* ele1, void* ele2) {
			t_page* pag1 = (t_page*) ele1;
			t_page* pag2 = (t_page*) ele2;
			return pag1->frame < pag2->frame;
		}

		list_sort(pages, index_frame);

		pthread_mutex_lock(&mutex_clock);
		pos_victim = clock_normal(pages);
		pthread_mutex_unlock(&mutex_clock);
	}

	if (pos_victim == -1) {
		log_info(get_logger(),"Error ejecutando el algoritmo de reemplazo.");
		exit(1);
	}

	t_page* victim = list_get(pages, pos_victim);

	victim->locked = true;


	//SI EL BIT DE MODIFICADO ES 1, LA GUARDO EM MV -> PORQUE TIENE CONTENIDO DIFERENTE A LO QUE ESTA EN MV
	if (victim->modified) {
		send_swap(victim);
		victim->modified = false;
	}

	victim->present = false;
	victim->locked = false;


	list_destroy(pages);

	return victim;
}

t_list* get_free_frames_from_process(uint32_t pid) {

	int same_pid_and_free(t_frame* frame_aux) {
		return (frame_aux->pid == pid && frame_aux->is_free);
	}

	return list_filter(FRAMES, (void*) same_pid_and_free);

}

int32_t clock_M(t_list* pages) {
	int selected_page;

	for (int i = 1; i <= 4; i++) {

		for (int j = 0; j < list_size(pages); j++) {

			if (POSITION_CLOCK >= list_size(pages)) {
				POSITION_CLOCK = 0;
			}

			t_page* page = list_get(pages, POSITION_CLOCK);

			if (i == 1 || i == 3) {
				//Buscamos 0,0
				if (page->used == false && page->modified == false) {
					selected_page = POSITION_CLOCK;
					POSITION_CLOCK++;
					return selected_page;
				} else {
					POSITION_CLOCK++;
				}

			}

			if (i == 2 || i == 4) {
				if (page->used == false && page->modified == true) {
					selected_page = POSITION_CLOCK;
					POSITION_CLOCK++;
					return selected_page;
				} else {
					POSITION_CLOCK++;
					page->used = false;
				}
			}
		}
	}
	return FAILURE;
}

int32_t clock_normal(t_list* pages) {
	int selected_page;

	for (int i = 0; i <= 1; i++) {

		for (int j = 0; j < list_size(pages); j++) {

			if (POSITION_CLOCK >= list_size(pages)) {
				POSITION_CLOCK = 0;
			}

			t_page* page = list_get(pages, POSITION_CLOCK);

			if (page->used == false) {
				selected_page = POSITION_CLOCK;
				POSITION_CLOCK++;
				return selected_page;
			} else {
				page->used = false;
				POSITION_CLOCK++;
			}

		}
	}
	return FAILURE;
}

void send_swap(t_page* page) {

	//verifico si ya tiene un frame reservado en swap
	if (page->pos_swap != -1) {
		save_content_in_swap_file(page->frame * page_size_getter(),
		page_size_getter(), page->pos_swap * page_size_getter());
		log_info(get_logger(),"SWAP OUT -  PID: <%d> - Marco: <%d> - Page Out: <%d>|<%d>",page->pid,page->pos_swap,page->segment,page->id);
	}else{
		//se le asigna una pagina de swap
			t_frame_swap* frame_swap = get_free_frame_from_swap();
			page->pos_swap = frame_swap->pos;
			frame_swap->is_free = false;
			save_content_in_swap_file(page->frame * page_size_getter(),page_size_getter(), frame_swap->pos * page_size_getter());
		log_info(get_logger(),"SWAP OUT -  PID: <%d> - Marco: <%d> - Page Out: <%d>|<%d>",page->pid,page->pos_swap,page->segment,page->id);
	}

}

bool load_page_to_memory(t_page* page) {//RETORNA TRUE SI HUBO PAGE_FAULT, FALSE SINO

	bool pagefault = false;
	int pos_frame = -1;
	//Busco frame en donde voy a alojar la pagina que me traigo de SWAP: ya sea un frame libre o bien un frame de pag q reempl.
	pthread_mutex_lock(&mutex_frames);
	t_list* process_free_frames = get_free_frames_from_process(page->pid);
	pthread_mutex_unlock(&mutex_frames);

	if (list_size(process_free_frames) > 0) {
		t_frame* frame = list_get(process_free_frames, 0);
		pos_frame = frame->id;
		page->frame = pos_frame;
		frame->is_free = false;
		log_info(get_logger(),"Se encontro frame libre para la pagina %d -- PID %d -- FRAME: %d",page->id, page->pid, frame->id);
		page->present = true;
	} else {
		pagefault = true;
		log_info(get_logger(),"No se encontro frame libre para %d del proceso %d, segmento %d no se encuentra en memoria",page->id, page->pid, page->segment);
		page->present = false;
	}
	list_destroy(process_free_frames);


	return pagefault;
}

void end_memory_module(int signal) {
	log_info(get_logger(), "FINALIZANDO MODULO DE MEMORIA");
	free(MEMORY_BLOCK);
	delete_swap_file(SWAP_FILE);

	void destroy_frames(t_frame* frame_aux) {
		free(frame_aux);
	}

	list_destroy_and_destroy_elements(FRAMES, (void*) destroy_frames);

	log_info(get_logger(),"FRAMES ELIMINADOS");

	void destroy_page(t_page* page_aux) {
		free(page_aux);
	}

	void destroy_table_page_second_level(table_page* table_aux) {
		list_destroy_and_destroy_elements(table_aux->pages,(void*) destroy_page);
		free(table_aux);
	}

	list_destroy_and_destroy_elements(TABLE_PAGES,(void*) destroy_table_page_second_level);

	log_info(get_logger(),"TABLAS DE PAGINAS ELIMINADAS");


	log_info(get_logger(),"ELIMINANDO CONFIG");
	destroy_config();
	log_info(get_logger(),"ELIMINANDO LOGGER");
	destroy_logger();

	liberarConexiones();
}

t_frame_swap* get_free_frame_from_swap() {
	bool is_free(t_frame_swap* frame) {
		return frame->is_free;
	}

	return (t_frame_swap*) list_find(FRAMES_SWAP, (void*) is_free);
}

t_frame_swap* find_frame_swap(uint32_t index) {
	return list_get(FRAMES_SWAP, index);
}

t_translation_response* translate_logical_address(uint32_t pid, uint32_t segment,uint32_t index_page,uint32_t is_writting) {

	t_translation_response* response = malloc(sizeof(t_translation_response));
	response->frame = -1;
	response->page_fault = false;

	table_page* table = find_table_with_pid(pid, segment);
	if (table == NULL) {
		log_info(get_logger(), "No existe tabla para el PID y SEGMENTO solicitado");

		response->result= FAILURE;

		return response;
	}
	log_info(get_logger(),"RETARDO MEMORIA START");
	usleep(memory_time());
	log_info(get_logger(),"RETARDO MEMORIA END");
	t_page* page = list_get(table->pages, index_page);
	page->used = true;
	//NO esta en memoria
	if (!page->present) {
		response->page_fault = true;
	}

	response->result= SUCCESS;
	response->frame = page->frame;

	if(is_writting){
		page->modified = true;
	}

	if(response->page_fault){
		log_info(get_logger(),"PID: <%d> - SEGMENT: <%d> - Página: <%d> - PAGE FAULT!!!",pid,segment,index_page);
	}else{
		log_info(get_logger(),"PID: <%d> - SEGMENT: <%d> - Página: <%d> - Marco: <%d>",pid,segment,index_page,response->frame);
	}

	return response;
}

t_list* find_all_pages_from_process(uint32_t pid) {
	//busco todas las paginas del PID,
	t_list* list_pages = list_create();

	table_page* table = NULL;
	pthread_mutex_lock(&mutex_table_pages);
	for (int i = 0; i < TABLE_PAGES->elements_count; i++) {
		table = list_get(TABLE_PAGES, i);
		if (table->PID == pid) {
			list_add_all(list_pages, table->pages);
		}
	}
	pthread_mutex_unlock(&mutex_table_pages);

	return list_pages;

}

void book_frames(uint32_t pid) {
	pthread_mutex_lock(&mutex_frames);
	t_list* free_frames_in_process = get_free_frames_from_process(pid);

	t_frame* frame = NULL;
	if (free_frames_in_process->elements_count < frames_per_table_getter()) {
		int to_book = frames_per_table_getter()- free_frames_in_process->elements_count;
		t_list* free_frames = get_free_frames();
		for (int i = 0; i < to_book; i++) {
			frame = list_get(free_frames, i);
			frame->pid = pid;
			log_info(get_logger(),"Se reserva el frame %d para el pid %d",frame->id,pid);
		}
		list_destroy(free_frames);

	}
	pthread_mutex_unlock(&mutex_frames);
	list_destroy(free_frames_in_process);

}

t_list* get_free_frames() {
	bool is_free(t_frame* frame) {
		return frame->is_free && frame->pid == -1;
	}

	return list_filter(FRAMES, (void*) is_free);
}

char* swap_page(uint32_t pid, uint32_t segment, uint32_t page_number){
	table_page* table = find_table_with_pid(pid,segment);
	t_page* page = list_get(table->pages,page_number);


	bool was_page_fault_finding_free_frame =  load_page_to_memory(page);
	char* response = NULL;
	if(was_page_fault_finding_free_frame){
		t_page* page_victim = execute_swapping(pid);
		response = string_from_format("%d|%d|%d",page_victim->pid,page_victim->segment,page_victim->id);

		int32_t frame = page_victim->frame;

		if(page->pos_swap != -1){
			load_file_into_memory(page->frame * page_size_getter(),page_size_getter(), page->pos_swap * page_size_getter());
			log_info(get_logger(),"SWAP IN -  PID: <%d> - Marco: <%d> - Page In: <%d>|<%d>",pid,page->pos_swap,page->segment,page->id);
		}



		log_info(get_logger(),"REEMPLAZO - PID: <%d> - Marco: <%d> - Page Out: <%d>|<%d> - Page In: %d|%d",pid,frame,page_victim->segment,page_victim->id,segment,page_number);

		page->present = true;
		page->frame = frame;
		page->modified = false;
	}else{
		response = "OK";
	}

	return response;
}

char* config_cpu(){
	return string_from_format("%d|%d",entries_per_table_getter(),page_size_getter());
}





