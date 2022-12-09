/*
 * instrucciones.c
 *
 *  Created on: Oct 15, 2022
 *      Author: utnso
 */

#include "../include/instrucciones.h"

int i = 2;
int tiempo_demora;
int interrumpido = 0;
t_registros* registros;

int tipo_operacion = -1;

char* nombre_instruccion = NULL;
char* operando_1_NOMBRE = NULL;
char* operando_2_NOMBRE = NULL;
int* operando_1_APUNTA = NULL;
int* operando_2_APUNTA = NULL;

int direccion_logica = 0;
int direccion_fisica = 0;
char* io = NULL;
char* io_registro = NULL;
int unidades = 0;
t_translation_response_mmu*  traduccion_mmu = NULL;

int continuar = 0;

int cantidadCaracteres_Lista;

void* inicializar()
{
	unidades = 0;
	io = NULL;
	io_registro = NULL;
	continuar = 0;
}

void limpiarParametros(void){
	nombre_instruccion = NULL;
	operando_1_NOMBRE = NULL;
	operando_2_NOMBRE = NULL;
	operando_1_APUNTA = NULL;
	operando_2_APUNTA = NULL;
}

int sePuedeConvertirEnInt(char* palabra){

    	int valor;
    	sscanf(palabra, "%d", &valor);

    	int sePuede;
    	if (sizeof(valor) <= 4)
    		sePuede = 1;
    	else{
    		sePuede = 0;
    	}
    	return sePuede;
    }



void* seguir_instrucciones(t_contexto_ejecucion* contexto, t_list* instrucciones, int pid){
	inicializar();
	int devolucion = OPTIMO;
	
	//cantidadCaracteres_Lista = strlen(instrucciones);
	registros = &(contexto->reg_general);

	do {
		devolucion = ciclo_instrucciones(contexto, instrucciones, pid);
	} while (devolucion == OPTIMO);
	

	contexto->io_unidades = unidades;
	contexto->io_dispositivo = io;
	contexto->estado = devolucion;
	contexto->io_registro = io_registro;
}




void fetch(int numero_instruccion, t_list* instrucciones, char** instruccion){

	char* instruccion_en_bruto = list_get(instrucciones, numero_instruccion);
	
	*instruccion = instruccion_en_bruto;
}

int* buscaOperando(char* nombre){

	int numerico;

	if (nombre[1] == 'X'){
		switch(nombre[0]) {
		  case 'A':
		    return &registros->ax;
		    break;
		  case 'B':
		    return &registros->bx;
		    break;
		  case 'C':
			return &registros->cx;
		    break;
		  case 'D':
			return &registros->dx;
		    break;
			}
		}

	sscanf(nombre, "%d", &numerico);

return numerico;
}

//void accederMemoria(operando* op){
void accederMemoria(int pid, t_list* tabla_segmentos){
	switch(tipo_operacion){
	case MOV_IN: 
		sscanf(operando_2_NOMBRE, "%d", &direccion_logica);
		traduccion_mmu = traducir_direccion_logica(pid, tabla_segmentos, direccion_logica, 0);
		validarTraduccionMemoria(traduccion_mmu,MOV_IN);
		free(traduccion_mmu);
		break;
	case MOV_OUT: 
		sscanf(operando_1_NOMBRE, "%d", &direccion_logica);
		traduccion_mmu = traducir_direccion_logica(pid, tabla_segmentos, direccion_logica, 1);
		validarTraduccionMemoria(traduccion_mmu,MOV_OUT);
		free(traduccion_mmu);
		break;
	default: 
	//sleep((float)(get_retardo_instruccion()/1000));
		break;

	}
}

void validarTraduccionMemoria(t_translation_response_mmu* response,uint32_t operacion){
	
	if(response->fue_page_fault){
		direccion_fisica = -1;
		segmento_page_fault = response->segmento;
		pagina_page_fault = response->pagina;
		log_info(get_log(),"PAGE FAULT - PID: %d - SEGMENTO: %d - PAGINA : %d",response->pid,response->segmento,response->pagina);
	}
	if(response->fue_segmentation_fault){
		direccion_fisica = -2;
		log_info(get_log(),"SEGMENTATION FAULT - PID: %d - SEGMENTO: %d - PAGINA : %d",response->pid,response->segmento,response->pagina);
	}

	if(!response->fue_segmentation_fault && !response->fue_page_fault){
		direccion_fisica = response->direccion_fisica;
		if(operacion == MOV_IN){
			log_info(get_log(),"despues de traducir memoria MOV IN - direccion_fisica: %d", direccion_fisica);
		}
		if(operacion == MOV_OUT){
			log_info(get_log(),"despues de traducir memoria MOV OUT - direccion_fisica: %d", direccion_fisica);
		}
	}



}

int comparacion(char* valor1, char* valor2){


	int a = 0;
		for (int i = 0; i<=6; i++){
			if (valor1[i] != valor2[i]){
				a = 1;
			break;
			}
		}

		return a;

}

void decodificar (char* instruccion_en_bruto, t_list* tabla_segmentos, int pid){

	tipo_operacion = 0;

	char* parametro_1;
	char* parametro_2;

	char** instruccion_separada = string_split(instruccion_en_bruto, " ");

	int cantidad_atributos = string_array_size(instruccion_separada);
	for (int ciclo = cantidad_atributos; ciclo > 0; ciclo = ciclo - 1)
	{
		switch(ciclo)
		{
		case 1:
			nombre_instruccion = string_array_pop(instruccion_separada);
			break;
		case 2:
			operando_1_NOMBRE = string_array_pop(instruccion_separada);
			operando_1_APUNTA = buscaOperando(operando_1_NOMBRE);
			break;
		case 3:
			operando_2_NOMBRE = string_array_pop(instruccion_separada);
			operando_2_APUNTA = buscaOperando(operando_2_NOMBRE);
			break;
		default:
			ciclo = 0;
			tipo_operacion = -1;
		}
	}


if (tipo_operacion == 0){


	if(string_equals_ignore_case(nombre_instruccion,"SET")) {
		tipo_operacion = SET;
	} else if(string_equals_ignore_case(nombre_instruccion,"ADD")) {
		tipo_operacion = ADD;
	} else if(string_equals_ignore_case(nombre_instruccion,"MOV_IN")) {
		tipo_operacion = MOV_IN;
	} else if(string_equals_ignore_case(nombre_instruccion,"MOV_OUT")) {
		tipo_operacion = MOV_OUT;
	} else if(string_equals_ignore_case(nombre_instruccion,"I/O")) {
		tipo_operacion = IO;
	} else if(string_equals_ignore_case(nombre_instruccion,"EXIT")) {
		tipo_operacion = EXIT;
	} else {
		tipo_operacion = -1;
	}
}

	accederMemoria(pid, tabla_segmentos);
}

//int ins_set(instruccion* instruct){
// Revisar aca que logica se estaba buscando:
// aca estas diciendo: el contenido de lo apuntado por operando_1_APUNTA es igual a operando_2_APUNTA, que es un puntero.
// En teoria esto esta mal, porque estas haciendo int = int*
int ins_set(void){
	*operando_1_APUNTA = operando_2_APUNTA;

	return OPTIMO;
}

int ins_add2(int* valor_1, int* valor_2){
	int valor_sumado = 0;
	valor_sumado = *valor_1;
	valor_sumado += *valor_2;
	return valor_sumado;
}

// Lo mismo de arriba, pero esta vez int* = int. No lo puedo corregir porque no se cual era la idea original.
int ins_add(void){
	operando_2_APUNTA = ins_add2(operando_1_APUNTA, operando_2_APUNTA);

	return ins_set();

}

int ins_mov_in(int pid){
	//registro en parametro 1 = lo que haya en la direccion fisica guardada en el parámetro 2
	if(direccion_fisica == -1) { // PAGE_FAULT == -1
		log_info(get_log(),"Ocurrio un page fault al hacer MOV_IN");
		return PAGE_DEFAULT;
	} 

	if(direccion_fisica == -2) { // SEGMENTATION_FAULT == -2
		log_info(get_log(),"Ocurrio un segmentation fault al hacer MOV_IN");
		return ERROR_SEGMENTATION_FAULT;
	} 

	char* valor_leido = leer_memoria(get_socket(),get_log(),pid, direccion_fisica,sizeof(int));
	int valor_recibido = atoi(valor_leido);
	
	//RECIBIR
	log_info(get_log(),"TERMINÓ EL MOV IN, VALO RECIBIDO: %s", valor_leido);
	*operando_1_APUNTA = valor_recibido;
	return OPTIMO;


}

int ins_mov_out(int pid){
	//guarda en memoria en la dir fisica del parametro 1= lo que haya en el registro del parámetro 2
	if(direccion_fisica == -1) { // PAGE_FAULT == -1
		log_info(get_log(),"Ocurrio un page fault al hacer MOV_OUT");
		return PAGE_DEFAULT;
	} 

	if(direccion_fisica == -2) { // SEGMENTATION_FAULT == -2
		log_info(get_log(),"Ocurrio un segmentation fault al hacer MOV_IN");
		return ERROR_SEGMENTATION_FAULT;
	} 

	int valor_a_pasar = unidades_en_registro(operando_2_APUNTA);
	char* valor_a_guardar = string_itoa(valor_a_pasar);

	//FUNCION PARA GUARDAR valor_a_guardar
	escribir_memoria(get_socket(),get_log(),pid, direccion_fisica,sizeof(valor_a_guardar),valor_a_guardar);
	log_info(get_log(),"TERMINÓ EL MOV OUT");
	return OPTIMO;
}

int unidades_en_registro (int* registro_buscado){
	int unidades_encontradas = *registro_buscado;
	return unidades_encontradas;
}

// Lo mismo de arriba, pero ahora es un int = int** por el &. En el else, es un int = int*
int ins_io(void){

	io = operando_1_NOMBRE;
	if (operando_2_NOMBRE[1] == 'X'){
		io_registro = operando_2_NOMBRE;
		unidades = unidades_en_registro(operando_2_APUNTA);

	}
	else
	{
		io_registro = NULL;
		unidades = operando_2_APUNTA;
	}

	return BLOQUEO;
}

int  ins_exit(void){
	return FINALIZADO;
}

//int ejecutar(instruccion* instruct){
int ejecutar(int pid){
	int estado = OPTIMO;

	//int opera = instruct->operacion;
	//switch(opera){
	switch(tipo_operacion){
	case SET:
		estado = ins_set();
		break;
	case ADD:
		estado = ins_add();
		break;
	case MOV_IN:
		estado = ins_mov_in(pid);
		break;
	case MOV_OUT:
		estado = ins_mov_out(pid);
		break;
	case IO:
		estado = ins_io();
		break;
	case EXIT:
		estado = ins_exit();
		break;
	default:
		estado = ERROR_INSTRUCCION;
	}
	return estado;
}


void interrumpe(void){
	continuar = 1;
}


int check_interrupt(int devuelve){

	//desactivar semáforo ckeck_kernel
	if (continuar > 0){
		devuelve = INTERRUPCION;
	}
	//activar semáforo check_kernel
	return devuelve;
}


int ciclo_instrucciones(t_contexto_ejecucion* contexto,  t_list* instrucciones, int pid)
{
	//limpia parámetros
	limpiarParametros();

	int devuelve = OPTIMO;

	//fetch
	char *instruccion_en_bruto;
	fetch(contexto->program_counter, instrucciones, &instruccion_en_bruto);

	//decodificar
	decodificar(instruccion_en_bruto, contexto->tabla_segmentos, pid);

	//ejecutar
	devuelve = ejecutar(pid);
	log_info(get_log(),"PID: %d - Ejecutando: %s - %s - %s", pid, nombre_instruccion, operando_1_NOMBRE, operando_2_NOMBRE);
	
	//actualizar ciclo
	if (devuelve != PAGE_DEFAULT){
		contexto->program_counter ++;
	}

	//check interrupt

	if (devuelve == OPTIMO){
		devuelve = check_interrupt(devuelve);
	}

	//devuelve = check_interrupt(devuelve);

	return devuelve;
}

