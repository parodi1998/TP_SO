#include "memory_file_management.h"
#include "config_manager/memory_configuration_manager.h"
#include <math.h>

//VARIABLES GLOBALES
void* MEMORY_BLOCK;
t_log* LOGGER;
char* PATH_SWAP_BASE;
FILE* SWAP_FILE;
void* MEMORY_BLOCK_SECONDARY;
int TABLE_ID_GENERATOR = 0;

//mutex
pthread_mutex_t mutex_clock;
pthread_mutex_t mutex_swap;
pthread_mutex_t mutex_frames;
pthread_mutex_t mutex_frames_swap;
pthread_mutex_t mutex_table_pages;
t_list* TABLE_PAGES;
t_list* FRAMES;
t_list* FRAMES_SWAP;
int32_t POSITION_CLOCK;
void initialize_memory_file_management() {
	LOGGER = get_logger();
	PATH_SWAP_BASE = swap_path();
	initialize_memory();

	 initialize_process(1,1,page_size_getter() * 4);
	 int result_num = translate_logical_address(1, 1,0);
	 log_info(LOGGER,"Result: %d",result_num);
	 /*
	 save_value_in_memory(uint32_t address, void* value,uint32_t size);
	 write_value_in_pid(1,"jojitosasdasdas", strlen("jojitosasdasdas"),page_size_getter()-3);

	 read_value_in_memory(uint32_t address, uint32_t size)
	 void* result = read_value_in_pid( 1, page_size_getter() -3, 15);
	 log_info(LOGGER,"Result: %s",(char*)result);
	*/
}

void initialize_memory() {
	uint32_t size = memory_size_getter();
	MEMORY_BLOCK = malloc(size);
	init_structs_and_lists();
	create_swap_file();
}

void save_value_in_memory(uint32_t address, void* value,uint32_t size) {
	memcpy(MEMORY_BLOCK + address, value, size);
	log_info(LOGGER, "Se guardo en la direccion %d , tamanio %d",address, size);
}

void* read_value_in_memory(uint32_t address, uint32_t size) {
	void* value = malloc(size);
	memcpy(value, MEMORY_BLOCK + address, size);
	log_info(LOGGER, "Se obtuvo el valor en la direccion %d",address);
	return value;
}

void save_content_in_swap_file(uint32_t address, uint32_t size,uint32_t logical_address) {

	pthread_mutex_lock(&mutex_swap);
	/*
	 SWAP_FILE = open(PATH_SWAP_BASE,O_CREAT|O_RDWR, 07777);
	 if(SWAP_FILE == -1){
	 log_info(LOGGER,"No se pudo abrir el archivo de swap %s",PATH_SWAP_BASE);
	 }else{
	 log_info(LOGGER,"Se abre el archivo swap %s",PATH_SWAP_BASE);
	 //inicio de la memoria secundaria con mmap
	 MEMORY_BLOCK_SECONDARY = mmap(NULL, size,PROT_WRITE|PROT_READ,MAP_SHARED|MAP_FILE,SWAP_FILE, 0);
	 ftruncate(SWAP_FILE, size);
	 memcpy(MEMORY_BLOCK_SECONDARY+ logical_address,MEMORY_BLOCK + address,size);
	 msync(MEMORY_BLOCK_SECONDARY, size, MS_SYNC);
	 close(SWAP_FILE) ;
	 log_info(LOGGER,"Se carga la pagina de memoria desde el puntero %d hasta %d a swap desde el puntero %d hasta %d",address,(address+size),logical_address,(logical_address+size));
	 usleep(swap_time());
	 }
	 pthread_mutex_unlock(&mutex_swap);
	 free(PATH_SWAP_BASE);
	 */
	memcpy(MEMORY_BLOCK_SECONDARY + logical_address, MEMORY_BLOCK + address,size);
	msync(MEMORY_BLOCK_SECONDARY, size, MS_SYNC);
	pthread_mutex_unlock(&mutex_swap);
	log_info(LOGGER,"Se carga la pagina de memoria desde el puntero %d hasta %d a swap desde el puntero %d hasta %d",address, (address + size), logical_address,(logical_address + size));
	usleep(swap_time());
}

void create_swap_file() {
	uint32_t size = swap_size_getter();
	SWAP_FILE = open(PATH_SWAP_BASE, O_CREAT | O_RDWR, 07777);
	if (SWAP_FILE == -1) {
		log_info(LOGGER, "No se pudo crear el archivo de swap %s",
				PATH_SWAP_BASE);
	} else {
		//inicio de la memoria secundaria con mmap
		MEMORY_BLOCK_SECONDARY = mmap(NULL, size, PROT_WRITE | PROT_READ,
		MAP_SHARED | MAP_FILE, SWAP_FILE, 0);
		ftruncate(SWAP_FILE, size);
		msync(MEMORY_BLOCK_SECONDARY, size, MS_SYNC);
		close(SWAP_FILE);
		log_info(LOGGER, "SWAP FILE CREATED");
	}
}

void load_file_into_memory(uint32_t address, uint32_t size,uint32_t logical_address) {
	pthread_mutex_lock(&mutex_swap);
	/*
	 SWAP_FILE = open(PATH_SWAP_BASE,O_CREAT|O_RDWR, 07777);
	 if(SWAP_FILE == -1){
	 log_info(LOGGER,"No se pudo abrir el archivo de swap %s",PATH_SWAP_BASE);
	 }else{
	 MEMORY_BLOCK_SECONDARY = mmap(NULL, size,PROT_WRITE|PROT_READ,MAP_SHARED|MAP_FILE,SWAP_FILE, 0);
	 ftruncate(SWAP_FILE, size);
	 memcpy(MEMORY_BLOCK + address,MEMORY_BLOCK_SECONDARY+ logical_address,size);
	 msync(MEMORY_BLOCK_SECONDARY, size, MS_SYNC);
	 close(SWAP_FILE);
	 log_info(LOGGER,"Se cargo la pagina de swap desde puntero %d hasta %d a memoria desde el puntero %d hasta %d",logical_address,(logical_address+size),address,(size+address));
	 usleep(swap_time());
	 }
	 pthread_mutex_unlock(&mutex_swap);
	 free(PATH_SWAP_BASE);
	 */
	memcpy(MEMORY_BLOCK + address, MEMORY_BLOCK_SECONDARY + logical_address,size);
	msync(MEMORY_BLOCK_SECONDARY, size, MS_SYNC);
	pthread_mutex_unlock(&mutex_swap);
	log_info(LOGGER,
			"Se cargo la pagina de swap desde puntero %d hasta %d a memoria desde el puntero %d hasta %d",logical_address, (logical_address + size), address,(size + address));
	usleep(swap_time());
}

void init_structs_and_lists() {

	int cant_frames = memory_size_getter() / page_size_getter();

	int cant_frames_swap = swap_size_getter() / page_size_getter();

	log_info(LOGGER, "Hay %d frames de %d bytes en memoria principal",
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

	log_info(LOGGER, "Estructuras administrativas inicializadas");
}

int32_t initialize_process(uint32_t pid, uint32_t segment_ID, uint32_t size) {
	//verifico si ya se ocuparon la cantidad maxima admitida de procesos en memoria
	int number_of_pages_to_create = ceil(
			(double) size / (double) page_size_getter());

	log_info(LOGGER,"Se encontro espacio libre en la tabla de 1er nivel para el PID %d",pid);

	bool is_free_and_unasigned(t_frame* frame) {
		return frame->is_free && frame->pid == -1;
	}
	pthread_mutex_lock(&mutex_frames);
	t_list* free_frames_MP = list_filter(FRAMES, (void*) is_free_and_unasigned);
	if (list_size(free_frames_MP) < frames_per_table_getter()) {

		log_info(LOGGER,"Error: se supero el nivel de multiprogramacion, no hay frames libes en MP para el PID %d, Segmento: %d",pid, segment_ID);
		list_destroy(free_frames_MP);
		return FAILURE;
	}
	//Reservamos los frames al PID
	for (int i = 0; i < frames_per_table_getter(); i++) {
		t_frame* frame_libre = list_get(free_frames_MP, i);
		frame_libre->pid = pid;
	}
	pthread_mutex_unlock(&mutex_frames);
	list_destroy(free_frames_MP);
	log_info(LOGGER, "Se reservaron frames para el PID %d", pid);
	//creacion de tabla de paginas

	table_page* new_table = malloc(sizeof(table_page));
	new_table->pages = list_create();
	new_table->PID = pid;
	new_table->ID = TABLE_ID_GENERATOR++;
	new_table->segment_ID = segment_ID;

	pthread_mutex_lock(&mutex_table_pages);
	list_add(TABLE_PAGES, new_table);
	pthread_mutex_unlock(&mutex_table_pages);

	int buffer_size = number_of_pages_to_create * page_size_getter();
	void* buffer_process = malloc(buffer_size);

	//Crea las pages y las guarda en memoria
	for (int i = 0; i < number_of_pages_to_create; i++) {

		t_page* page = malloc(sizeof(t_page));
		page->pid = pid;
		page->id = i;
		page->frame = -1;
		page->modified = true;
		page->locked = true;
		page->present = false;
		page->used = true;
		page->pos_swap = -1;

		list_add(new_table->pages, page);
		log_info(LOGGER, "Se creo la pagina %d con frame %d para el PID %d", i,
				page->frame, pid);
		memcpy(MEMORY_BLOCK + page->frame * page_size_getter(),
				buffer_process + i * page_size_getter(), page_size_getter());
		page->locked = false;
	}
	free(buffer_process);

	//retornamos el id de la pagina de primer nivel
	log_info(LOGGER, "Se crea la tabla de paginas ID: %d  al PID: %d",new_table->ID, pid);

	book_frames(pid);

	return new_table->ID;

}

//Funcion principal para guardar datos en el proceso previamente creado
/*
 int32_t write_value_in_pid(uint32_t pid,uint32_t segment_ID, void* value, uint32_t size,uint32_t logical_address) {
 table_page* table = find_table_with_pid(pid,segment_ID);
 if (table == NULL) {
 log_info(LOGGER, "No existe tabla para el PID solicitado");
 return FAILURE;
 }

 uint32_t start_page = floor(
 (double) logical_address / (double) page_size_getter());
 uint32_t end_page = floor(
 ((double) logical_address + size) / (double) page_size_getter());
 //me aseguro que la pagina de fin no sea una falsa alarma. Caso ejemplo: guardar en direccion logica donde abarque hasta el ultimo espacio libre de la pagina
 if ((logical_address + size) % page_size_getter() == 0) {
 end_page = start_page;
 }
 log_info(LOGGER, "start_page %d", start_page);
 log_info(LOGGER, "end_page %d", end_page);
 if (start_page > table->pages->elements_count
 || logical_address + size
 > table->pages->elements_count * page_size_getter()) {
 log_info(LOGGER, "La direccion logica es invalida");
 return FAILURE;
 }

 bool same_id(t_page* page_aux) {
 return page_aux->id == start_page;
 }

 t_page* page = find_page(pid, start_page);

 uint32_t offset = logical_address - page_size_getter() * page->id;
 page->locked = true;
 uint32_t real_address_start = page->frame * page_size_getter() + offset;
 if (start_page == end_page) {
 //escribo en solo una pagina
 save_value_in_memory(real_address_start, value, pid, size);
 page->locked = false;
 page->modified = true;
 } else {
 //El memwrite se hace en varias paginas
 uint32_t size_start = page_size_getter() - offset;
 save_value_in_memory(real_address_start, value, pid, size_start);
 page->locked = false;
 page->modified = true;
 size -= page_size_getter() - offset;

 int i;
 uint32_t offset_aux = size_start;
 //Entra al for solo si tiene que copiar paginas enteras
 for (i = 1; i <= end_page - start_page - 1; i++) {
 page = find_page(pid, start_page + i);
 page->locked = true;
 save_value_in_memory(page->frame * page_size_getter(),
 value + offset_aux, pid, size);
 page->locked = false;
 page->modified = true;
 size -= page_size_getter();
 offset_aux += page_size_getter();
 }
 //ultima pagina

 page = find_page(pid, start_page + i);
 page->locked = true;
 save_value_in_memory(page->frame * page_size_getter(),
 value + offset_aux, pid, size);
 page->locked = false;
 page->modified = true;

 }

 log_info(LOGGER, "Escritura exitosa del valor %d para el PID %d", value,
 pid);
 return SUCCESS;

 }
 */
/*
 void* read_value_in_pid(uint32_t pid, uint32_t logical_address, uint32_t size) {
 table_page* table = find_table_with_pid(pid);
 if (table == NULL) {
 log_info(LOGGER, "No existe tabla para el PID solicitado");
 return FAILURE;
 }

 uint32_t start_page = floor(
 (double) logical_address / (double) page_size_getter());
 uint32_t end_page = floor(
 ((double) logical_address + size) / (double) page_size_getter());
 //me aseguro que la pagina de fin no sea una falsa alarma. Caso ejemplo: guardar en direccion logica donde abarque hasta el ultimo espacio libre de la pagina
 if ((logical_address + size) % page_size_getter() == 0) {
 end_page = start_page;
 }

 log_info(LOGGER, "start_page %d", start_page);
 log_info(LOGGER, "end_page %d", end_page);
 if (start_page > table->pages->elements_count
 || logical_address + size
 > table->pages->elements_count * page_size_getter()) {
 log_info(LOGGER, "La direccion logica es invalida");
 return FAILURE;
 }

 bool same_id(t_page* page_aux) {
 return page_aux->id == start_page;
 }

 t_page* page = find_page(pid, start_page);

 uint32_t offset = logical_address - page_size_getter() * page->id;
 page->locked = true;
 uint32_t real_address_start = page->frame * page_size_getter() + offset;
 void* value;
 if (start_page == end_page) {
 //leo en solo una pagina

 value = read_value_in_memory(real_address_start, pid, size);
 page->locked = false;
 } else {
 value = malloc(size);
 void* aux_stream = read_value_in_memory(real_address_start, pid,
 page_size_getter() - offset);
 page->locked = false;
 size -= page_size_getter() - offset;
 uint32_t offset_auxiliar = page_size_getter() - offset;
 memcpy(value, aux_stream, offset_auxiliar);
 free(aux_stream);
 //Entra al for solo si tiene que copiar paginas enteras, entre la primera y n-1 pagina
 int i;
 for (i = 1; i <= end_page - start_page - 1; i++) {
 page = find_page(pid, start_page + i);
 page->locked = true;
 aux_stream = read_value_in_memory(page->frame * page_size_getter(),
 pid, page_size_getter());
 page->locked = false;
 memcpy(value + offset_auxiliar, aux_stream, page_size_getter());
 free(aux_stream);
 offset_auxiliar += page_size_getter();
 size -= page_size_getter();
 }
 page = find_page(pid, start_page + i);
 page->locked = true;
 aux_stream = read_value_in_memory(page->frame * page_size_getter(), pid,
 size);
 page->locked = false;
 memcpy(value + offset_auxiliar, aux_stream, size);
 free(aux_stream);
 }

 log_info(LOGGER, "Lectura exitosa para el PID %d", pid);
 return value;

 }*/

int32_t suspend_process(uint32_t pid, uint32_t segment) {
	log_info(LOGGER, "Se suspende el proceso %d , segmento &d...", pid,
			segment);
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

			log_info(LOGGER,
					"[Proceso %d]Se vacia con exito la pagina %d y se libera frame nro %d",
					pid, page->id, frame->id);

		}
	}

	return SUCCESS;
}

int32_t finalize_process(uint32_t pid, uint32_t segment) {
	log_info(LOGGER, "Comienza la finalizacion del proceso %d, segmento &d ..",pid, segment);
	table_page* table = find_table_with_pid(pid, segment);
	t_page* page;
	t_frame* frame;
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
			if(page->pos_swap != -1){
				pthread_mutex_lock(&mutex_frames_swap);
				frame_swap = find_frame_swap(page->pos_swap);
				frame_swap->is_free = true;
				pthread_mutex_unlock(&mutex_frames_swap);
			}


			log_info(LOGGER, "[Proceso %d, Segmento &d]Se libera frame nro %d",
					pid, segment, page->id, frame->id);
		}
	}
	delete_swap_file(pid);

	log_info(LOGGER, "Se finalizo el proceso %d segmento %d con exito", pid,
			segment);
	return SUCCESS;
}

int32_t restore_process(uint32_t pid, uint32_t segment) {
	//verifico si ya se ocuparon la cantidad maxima admitida de procesos en memoria
	log_info(LOGGER, "Se reestablece el proceso suspendido nro %d, segmento %d",
			pid, segment);
	table_page* table = find_table_with_pid(pid, segment);
	t_page* page;
	bool is_free_and_unasigned(t_frame* frame) {
		return frame->is_free && frame->pid == -1;
	}
	pthread_mutex_lock(&mutex_frames);
	t_list* free_frames_MP = list_filter(FRAMES, (void*) is_free_and_unasigned);
	if (list_size(free_frames_MP) < frames_per_table_getter()) {
		log_info(LOGGER,
				"Error: se supero el nivel de multiprogramacion, no hay frames libes en MP para el PID %d",
				pid);
		list_destroy(free_frames_MP);
		return FAILURE;
	}
	//Reservamos los frames al PID
	for (int i = 0; i < frames_per_table_getter(); i++) {
		t_frame* frame_libre = list_get(free_frames_MP, i);
		frame_libre->pid = pid;
	}
	pthread_mutex_unlock(&mutex_frames);
	list_destroy(free_frames_MP);
	log_info("Se reestablecio el PID %d con exito", pid);
	return SUCCESS;
}

void delete_swap_file(uint32_t pid) {
	char* path = swap_path();
	pthread_mutex_lock(&mutex_swap);
	log_info(LOGGER, "Eliminando el archivo swap %s", path);
	remove(path);
	pthread_mutex_unlock(&mutex_swap);
	free(path);
}

table_page* find_table_with_pid(int32_t pid, int32_t segment_ID) {

	table_page* table;
	log_info(LOGGER, "Buscamos la tabla del PID %d y segmento %d ...", pid,
			segment_ID);

	bool same_id(table_page* table_aux) {
		return (table_aux->PID == pid && table_aux->segment_ID == segment_ID);
	}

	pthread_mutex_lock(&mutex_table_pages);
	table = list_find(TABLE_PAGES, (void*) same_id);
	pthread_mutex_unlock(&mutex_table_pages);
	log_info(LOGGER, "Encontramos la tabla del PID %d, segmento: %d", pid,
			segment_ID);
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
		log_info(LOGGER, "Se reserva el frame %d", frame->id);
		list_destroy(free_frame_process);
		return id;
	}

	list_destroy(free_frame_process);
	pthread_mutex_unlock(&mutex_frames);

	return execute_swapping(pid);
}

int32_t execute_swapping(uint32_t pid) {

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
		printf("Error ejecutando el algoritmo de reemplazo.");
		exit(1);
	}

	t_page* victim = list_get(pages, pos_victim);

	victim->locked = true;

	log_info(LOGGER, "[REEMPLAZO] Saco NRO_PAG %i del PID %i en FRAME %i",
			victim->id, victim->pid, victim->frame);

	//SI EL BIT DE MODIFICADO ES 1, LA GUARDO EM MV -> PORQUE TIENE CONTENIDO DIFERENTE A LO QUE ESTA EN MV
	if (victim->modified) {
		send_swap(victim);
		victim->modified = false;
	}

	victim->present = false;
	victim->locked = false;

	list_destroy(pages);

	return victim->frame;
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

	log_info(LOGGER, "Tirando la pagina modificada %i en swap, del proceso %i",page->id, page->pid);

	//verifico si ya tiene un frame reservado en swap
	if(page->pos_swap !=-1){
		save_content_in_swap_file(page->frame * page_size_getter(),page_size_getter(),page->pos_swap * page_size_getter() );
	}
	//se le asigna una pagina de swap
	t_frame_swap* frame_swap = get_free_frame_from_swap();
	page->pos_swap = frame_swap->pos;
	frame_swap->is_free = false;

	save_content_in_swap_file(page->frame * page_size_getter(),page_size_getter(), frame_swap->pos * page_size_getter());

}

void load_page_to_memory(t_page* page) {

	int pos_frame = -1;
	//Busco frame en donde voy a alojar la pagina que me traigo de SWAP: ya sea un frame libre o bien un frame de pag q reempl.

	t_list* process_free_frames = get_free_frames_from_process(page->pid);

	if (list_size(process_free_frames) > 0) {
		t_frame* frame = list_get(process_free_frames, 0);
		pos_frame = frame->id;
		log_info(LOGGER, "Se encontro frame libre para la pagina %d -- PID %d -- FRAME: %d",page->id,page->pid,frame->id );
	} else {
		pos_frame = execute_swapping(page->pid);
		log_info(LOGGER, "Cargando la pagina %d de swap a memoria, PID %d",page->id, page->pid);
		load_file_into_memory(pos_frame * page_size_getter(),page_size_getter(), page->pos_swap * page_size_getter());
	}
	list_destroy(process_free_frames);


	page->present = true;
	page->modified = false;
	page->frame = pos_frame;
}

void end_memory_module() {
	log_info(LOGGER, "FINALIZANDO MODULO DE MEMORIA");
	free(MEMORY_BLOCK);

	void destroy_frames(t_frame* frame_aux) {
		free(frame_aux);
	}

	list_destroy_and_destroy_elements(FRAMES, (void*) destroy_frames);

	void destroy_page(t_page* page_aux) {
		free(page_aux);
	}

	void destroy_table_page_second_level(table_page* table_aux) {
		list_destroy_and_destroy_elements(table_aux->pages,
				(void*) destroy_page);
		free(table_aux);
	}

	list_destroy_and_destroy_elements(TABLE_PAGES,
			(void*) destroy_table_page_second_level);

}

int32_t get_frame_by_id_page_second_level(uint32_t id_page_second_level) {

	pthread_mutex_lock(&mutex_table_pages);
	t_page* page = (t_page*) list_get(TABLE_PAGES, id_page_second_level);
	pthread_mutex_unlock(&mutex_table_pages);

	if (page == NULL) {
		return FAILURE;
	}
	if (!page->present) {
		log_info(LOGGER,
				"PAGE FAULT! La pag %i del proceso %i no se encuentra en memoria",
				id_page_second_level, page->pid);
		load_page_to_memory(page);
		return page->frame;
	}
	return FAILURE;

}

t_frame_swap* get_free_frame_from_swap(){
	bool is_free(t_frame_swap* frame){
		return frame->is_free;
	}


	return (t_frame_swap* ) list_find(FRAMES_SWAP,(void*)is_free);
}

t_frame_swap* find_frame_swap(uint32_t index){
	return list_get(FRAMES_SWAP,index);
}


int32_t translate_logical_address(uint32_t pid, uint32_t segment, uint32_t index_page) {
	table_page* table = find_table_with_pid(pid, segment);
	if (table == NULL) {
		log_info(LOGGER, "No existe tabla para el PID y SEGMENTO solicitado");
		return FAILURE;
	}
	t_page* page = list_get(table->pages,index_page);
	page->used = true;
	//NO esta en memoria
	if (!page->present) {
		log_info(LOGGER,"PAGE FAULT! La pag %i del proceso %i, segmento %d no se encuentra en memoria",index_page, pid,segment);
		load_page_to_memory(page);
	}
	return page->frame;
}

t_list* find_all_pages_from_process(uint32_t pid){
	//busco todas las paginas del PID,
	t_list* list_pages = list_create();

	table_page* table = NULL;
	for(int i=0; i < TABLE_PAGES->elements_count;i++){
		table = list_get(TABLE_PAGES,i);
		if(table->PID == pid){
			list_add_all(list_pages,table->pages);
		}
	}

	return list_pages;

}

void book_frames(uint32_t pid){
	t_list* free_frames_in_process = get_free_frames_from_process(pid);

	t_frame* frame = NULL;
	if(free_frames_in_process->elements_count < frames_per_table_getter()){
		int to_book = frames_per_table_getter() - free_frames_in_process->elements_count;
		t_list* free_frames = get_free_frames();
		for(int i=0 ; i< to_book; i++){
			frame = list_get(free_frames,i);
			frame->pid = pid;
		}
		list_destroy(free_frames);

	}
	list_destroy(free_frames_in_process);


}

t_list* get_free_frames(){
	bool is_free(t_frame* frame){
		return frame->is_free;
	}

	return list_filter(FRAMES,(void*)is_free);
}


