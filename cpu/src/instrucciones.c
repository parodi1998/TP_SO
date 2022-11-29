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
char* lista_ope[6];
t_registros* registros;

int tipo_operacion = -1;
char* operando_1_NOMBRE = NULL;
char* operando_2_NOMBRE = NULL;
int* operando_1_APUNTA = NULL;
int* operando_2_APUNTA = NULL;

char* io = NULL;
char* io_registro = NULL;
int unidades = 0;

int continuar = 0;

int cantidadCaracteres_Lista;

void* inicializar()
{
	io = NULL;
	unidades = 0;
	io_registro = NULL;
	continuar = 0;
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

	todas_operaciones();
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

  //pthread_t id;

  //int j = 1;
  //pthread_create(&id, NULL, funcion_hilo, &j);

  //int* ptr;

  // Wait for foo() and retrieve value in ptr;
//  pthread_join(id, (void**)&ptr);



void* todas_operaciones(void){

	lista_ope[SET] = "SET____";
	lista_ope[ADD] = "ADD____";
	lista_ope[MOV_IN] = "MOV_IN_";
	lista_ope[MOV_OUT] = "MOV_OUT";
	lista_ope[IO] = "I/O____";
	lista_ope[EXIT] = "EXIT___";

}

void fetch(int numero_instruccion, t_list* instrucciones, char** instruccion){

	// Aca en vez de todo el for podes hacer esto:
	// char* instruccion_en_bruto = list_get(insturcciones, numero_instruccion);
	t_list* instruct = instrucciones;
	for (int i = 1; i < numero_instruccion; i++){
		instruct->head  = instruct->head->next;
	}
	char* instruccion_en_bruto = instruct->head->data;


//	char instruccion_en_bruto[15];
//	int instruccion_buscada = pcb_kernel->program_counter;
//	char * busqueda = pcb_kernel->instrucciones;
//	int caracter = 0;
//	int limite = 1;
//
//
//	while(limite < instruccion_buscada && caracter < cantidadCaracteres_Lista){
//
//		if (busqueda[caracter] == '\n'){
//			limite++;
//		}
//		caracter++;
//	}
//	int caracter_ingresar = 0;
//
//
//
//	while(busqueda[caracter_ingresar + caracter] != '\n' && (caracter_ingresar + caracter) < cantidadCaracteres_Lista){
//
//			instruccion_en_bruto[caracter_ingresar] = busqueda[caracter_ingresar + caracter];
//
//			caracter_ingresar++;
//
//	}
//	instruccion_en_bruto[caracter_ingresar] = '\n';
	*instruccion = instruccion_en_bruto;


}

void buscaOperando(char* nombre, int** numerico){

	int* numerico_aux;

	if (nombre[1] == 'X'){
		switch(nombre[0]) {
		  case 'A':
		    *numerico = &registros->ax;
		    break;
		  case 'B':
		    *numerico = &registros->bx;
		    break;
		  case 'C':
			*numerico = &registros->cx;
		    break;
		  case 'D':
			*numerico = &registros->dx;
		    break;
			}
		}

	sscanf(nombre, "%d", numerico_aux);

	*numerico = numerico_aux;
}

//void accederMemoria(operando* op){
void accederMemoria(int pid, t_list* tlb){
	switch(tipo_operacion){
	case MOV_IN: 
		*operando_2_APUNTA = traducir_direccion_logica(pid, tlb, *operando_2_APUNTA);
		break;
	case MOV_OUT: 
		*operando_1_APUNTA = traducir_direccion_logica(pid, tlb, *operando_1_APUNTA);
		break;
	default: 
		sleep(1000);
		break;

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

void decodificar (char* instruccion_en_bruto, t_list* la_tbl, int pid){

	int a = strlen(instruccion_en_bruto);
	char atributo_1[7];
	char atributo_2[10];
	char atributo_3[10];
	int buscador = 1;

	int i2 = 0;
	int j = 0;
	int k = 0;
	char letra;
	for (int i = 0; i < a; i++){
				if (  (instruccion_en_bruto[i] == ' ') || (instruccion_en_bruto[i] == '\n') )
				{
					buscador++;
				}
				else
				{
					letra = instruccion_en_bruto[i];

					switch(buscador){
					 case 1:
						 if (letra != '\0'){
							 atributo_1[i] = letra;
						 i2++;
						 }
						 break;
					 case 2:
						 atributo_2[j] = letra;
						 j++;
						 break;
					 case 3:
						 atributo_3[k] = letra;
						 k++;
					break;
					}
				}
			}

	if(i2 <= 6){
		for(i2; i2<=6; i2++){
			atributo_1[i2]= '_';
		}
	}
	if(j <= 9){
		for(j; j<=9; j++){
			atributo_2[j]= ' ';
		}
	}
	if(k <= 9){
		for(k; k<=9; k++){
			atributo_3[k]= ' ';
		}
	}
	tipo_operacion = -1;

	for (int l = SET; l <= EXIT; l++){
		if (comparacion(atributo_1, lista_ope[l]) == 0){
			tipo_operacion = l;
			break;
		}
	}



	operando_1_NOMBRE = atributo_2;
	operando_2_NOMBRE = atributo_3;

	buscaOperando(operando_1_NOMBRE, &operando_1_APUNTA);
	buscaOperando(operando_2_NOMBRE, &operando_2_APUNTA);

	accederMemoria(pid, la_tbl);


}

//int ins_set(instruccion* instruct){
// Revisar aca que logica se estaba buscando:
// aca estas diciendo: el contenido de lo apuntado por operando_1_APUNTA es igual a operando_2_APUNTA, que es un puntero.
// En teoria esto esta mal, porque estas haciendo int = int*
int ins_set(void){
	*operando_1_APUNTA = operando_2_APUNTA;

	return OPTIMO;
}

int ins_add2(int* a, int* b){
	int c = 0;
	c = *a;
	c += *b;



	return c;
}

// Lo mismo de arriba, pero esta vez int* = int. No lo puedo corregir porque no se cual era la idea original.
int ins_add(void){
	operando_2_APUNTA = ins_add2(operando_1_APUNTA, operando_2_APUNTA);

	return ins_set();

}

int ins_mov_in(void){

	return OPTIMO;
}

int ins_mov_out(void){
	return OPTIMO;
}

// Lo mismo de arriba, pero ahora es un int = int** por el &. En el else, es un int = int*
int ins_io(void){

	io = operando_1_NOMBRE;
	if (operando_2_NOMBRE[1] == 'X'){
		io_registro = operando_2_NOMBRE;
		unidades = &operando_2_APUNTA;
	}
	else
	{
		unidades = operando_2_APUNTA;
	}

	return BLOQUEO;
}

int  ins_exit(void){
	return FINALIZADO;
}

//int ejecutar(instruccion* instruct){
int ejecutar(void){
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
		estado = ins_mov_in();
		break;
	case MOV_OUT:
		estado = ins_mov_out();
		break;
	case IO:
		estado = ins_io();
		break;
	case EXIT:
		estado = ins_exit();
		break;
	default:
		printf("Error");
		estado = INTERRUPCION;
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

	int devuelve = OPTIMO;

	//fetch
	char *instruccion_en_bruto;
	fetch(contexto->program_counter, instrucciones, &instruccion_en_bruto);


	//decodificar
	decodificar(instruccion_en_bruto, contexto->tabla_segmentos, pid);

	//ejecutar
	devuelve = ejecutar();

	//check interrupt
	devuelve = check_interrupt(devuelve);

	//actualizar ciclo
	contexto->program_counter ++;

	return devuelve;


}
