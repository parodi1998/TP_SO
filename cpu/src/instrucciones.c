/*
 * instrucciones.c
 *
 *  Created on: Oct 15, 2022
 *      Author: utnso
 */

#include "instrucciones.h"

int i = 2;

int ax = 0;
int bx = 0;
int cx = 0;
int dx = 0;

void* funcion_hilo(void* p){
  // Print value received as argument:
  printf("Value recevied as argument in starting routine: ");
  printf("%i\n", * (int*)p);

  // Return reference to global variable:
  pthread_exit(&i);
}


void* seguir_instrucciones(void){

	while (1==1){
		//realizar_instrucciones;
	}
}

  //pthread_t id;

  //int j = 1;
  //pthread_create(&id, NULL, funcion_hilo, &j);

  //int* ptr;

  // Wait for foo() and retrieve value in ptr;
//  pthread_join(id, (void**)&ptr);



char* fetch(int numero_instruccion)
{
	char *instruccion_en_bruto;
	//solicitar instruccion
	//recibir instruccion
	return instruccion_en_bruto;
}

lista_operaciones* todas_operaciones(void){
	char* path_1 = getcwd(NULL, 0);
	path_1 = concatenar(path_1, "/instrucciones.config");
	char numero;
	lista_operaciones* lista;

	for (int i = 0; i <= 5; i++){
		numero = i + '0';
		lista[i].t_instruccion = numero;
		lista[i].nombre_instruccion = config_get_string_value(path_1, numero);
	}
	return lista;

}

int buscarValorOperacion (char* operacion_charm, lista_operaciones* lista){
	int numero;
	int encontrado = 0;
	for (int i = 0; i < 10; i++){
		numero = lista[i].t_instruccion;
		if (operacion_charm == lista[i].nombre_instruccion){
			encontrado = 1;
			break;
		}
	}
	if (encontrado == 0) numero = -1;
	return numero;
}


instruccion decodificar(char* instruccion_en_bruto){
	instruccion instruct;
	//char * operacion_char = tomar(instruccion_en_bruto, 0);
	//instruct.operacion = buscarValorOperacion(operacion_char);
	//instruct.operando_1 = tomar(instruccion_en_bruto, 1);
	//instruct.operando_2 = tomar(instruccion_en_bruto, 2);


	return instruct;
}

int ejecutar(instruccion instruct){
	int error = 0;
	switch(instruct.operacion){
	case SET:
		//in_set(instruct);
		break;
	case ADD:
		ins_add(instruct);
		break;
	case MOV_IN:
		ins_mov_in(instruct);
		break;
	default:
		printf("Error");
		error = 1;
	}
	return error;
}

void do_set(char * direccion, int valor){
	//grabar valor en direccion
}

int traer_Dato(char *direccion){
	int dato;  // = buscar direccion
	return dato;
}

void ins_set(instruccion instruct){
	int valor = (int)instruct.operando_2;
	do_set(instruct.operando_1, valor);
}

void ins_add(instruccion instruct){
	int valor =	((int)instruct.operando_1) + ((int)instruct.operando_2);
	//ins_set(instruct.operando_1, valor);
}

void ins_mov_in(instruccion instruct){
	int valor = traer_Dato(instruct.operando_2);
	do_set(instruct.operando_1, valor);
}

//void ins_mov_out(instruccion instruct){

//}

//void ins_io(instruccion instruct){
	//I/O
//}

//void ins_exit(instruccion instruct){
	//EXIT
//}




int realizar_instrucciones(int numero_instruccion)
{
	//fetch
	char *instruccion_en_bruto = fetch(numero_instruccion);

	//decodificar
	instruccion instruct = decodificar(instruccion_en_bruto);

	//ejecutar
	int error = ejecutar(instruct);


	return error;
}
