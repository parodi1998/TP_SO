#ifndef KISS_MEMORY_FILE_MANAGEMENT_H
#define KISS_MEMORY_FILE_MANAGEMENT_H

#include<stdio.h>
#include<stdlib.h>
#include<stdint-gcc.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<string.h>
#include<sys/mman.h>
#include<fcntl.h>
#include<unistd.h>
#include<commons/collections/list.h>
#include<pthread.h>

#define SUCCESS 1
#define FAILURE -1

typedef struct {
	int32_t id;
	int32_t pid;
	int32_t segment;
	int32_t frame;
	bool present;
	bool used;
	bool modified;
	bool locked;
	int32_t pos_swap;
} t_page;

typedef struct {
	int32_t ID;
	int32_t PID;
	int32_t segment_ID;
	t_list* pages;
} table_page;

typedef struct {
	int32_t pid;
	int id;
	bool is_free;
} t_frame;

typedef struct {
	uint32_t pos;
	bool is_free;
}t_frame_swap;

typedef struct{
	int32_t result;
	bool page_fault;
	int32_t frame;
}t_translation_response;

void init_global_variables(void);

void initialize_memory(void);
void init_structs_and_lists(void);

void save_value_in_memory(uint32_t address, void* value,uint32_t size);
//int32_t write_value_in_pid(uint32_t pid, void* value, uint32_t size,uint32_t logical_address);
void* read_value_in_memory(uint32_t address, uint32_t size);
void initialize_memory_file_management();
void save_content_in_swap_file(uint32_t address, uint32_t size,uint32_t logical_address);
void load_file_into_memory(uint32_t address, uint32_t size,uint32_t logical_address);

table_page* find_table_with_pid(int32_t pid,int32_t segment_ID);
void init_structs_and_lists();
int32_t initialize_process(uint32_t pid,uint32_t segment_ID ,uint32_t size);
t_list* get_free_frames_from_process(uint32_t pid);
int32_t clock_M(t_list* pages);
int32_t clock_normal(t_list* pages);
void send_swap(t_page* pid);
int32_t execute_swapping(uint32_t pid);
uint32_t get_free_frame(uint32_t pid);
bool load_page_to_memory(t_page* page);

int32_t suspend_process(uint32_t pid,uint32_t segment);
int32_t restore_process(uint32_t pid,uint32_t segment);

void delete_swap_file(uint32_t pid);
int32_t finalize_process(uint32_t pid,uint32_t segment);
void create_swap_file();

void end_memory_module();
t_translation_response* translate_logical_address(uint32_t pid ,uint32_t segment,uint32_t page);
t_frame_swap* get_free_frame_from_swap();

t_frame_swap* find_frame_swap(uint32_t index);
t_list* find_all_pages_from_process(uint32_t pid);
t_list* get_free_frames();
void book_frames(uint32_t pid);
#endif //KISS_MEMORY_FILE_MANAGEMENT_H
