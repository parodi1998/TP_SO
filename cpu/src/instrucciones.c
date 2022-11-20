/*
 * instrucciones.c
 *
 *  Created on: Oct 15, 2022
 *      Author: utnso
 */

#include "instrucciones.h"

int i = 2;
int tiempo_demora;
int interrumpido = 0;
char* lista_ope[6];
registros* contexto;

int tipo_operacion = -1;
char* operando_1_NOMBRE = NULL;
char* operando_2_NOMBRE = NULL;
int* operando_1_APUNTA = NULL;
int* operando_2_APUNTA = NULL;

char* io = NULL;
int unidades = 0;

int continuar = 0;

int cantidadCaracteres_Lista;

void* inicializar()
{
	io = NULL;
	unidades = 0;
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

void* seguir_instrucciones(t_contexto_ejecucion* pcb_kernel){
	inicializar();
	int devolucion = OPTIMO;

	todas_operaciones();
	cantidadCaracteres_Lista = strlen(pcb_kernel->instrucciones);
	contexto = &(pcb_kernel->reg_general);
	do {
		devolucion = ciclo_instrucciones(pcb_kernel);

	} while (devolucion == OPTIMO);


	pcb_kernel->io_unidades = unidades;
	pcb_kernel->io_dispositivo = io;
	pcb_kernel->estado = devolucion;

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

void fetch(t_contexto_ejecucion* pcb_kernel, char** instruccion){
	char instruccion_en_bruto[15];
	int instruccion_buscada = pcb_kernel->program_counter;
	char * busqueda = pcb_kernel->instrucciones;
	int caracter = 0;
	int limite = 1;


	while(limite < instruccion_buscada && caracter < cantidadCaracteres_Lista){

		if (busqueda[caracter] == '\n'){
			limite++;
		}
		caracter++;
	}
	int caracter_ingresar = 0;



	while(busqueda[caracter_ingresar + caracter] != '\n' && (caracter_ingresar + caracter) < cantidadCaracteres_Lista){

			instruccion_en_bruto[caracter_ingresar] = busqueda[caracter_ingresar + caracter];

			caracter_ingresar++;

	}
	instruccion_en_bruto[caracter_ingresar] = '\n';
	*instruccion = instruccion_en_bruto;


}

int* buscaOperando(char* nombre){

	int numerico;
	if (nombre[1] == 'X'){
		switch(nombre[0]) {
		  case 'A':
		    return &contexto->ax;
		    break;
		  case 'B':
		    return &contexto->bx;
		    break;
		  case 'C':
			return &contexto->cx;
		    break;
		  case 'D':
			return &contexto->dx;
		    break;
			}
		}
	sscanf(nombre, "%d", &numerico);

return numerico;
}

//void accederMemoria(operando* op){
void accederMemoria(int* op){
	//CAMBIAR LUEGO
	//op->apunta = contexto->ax;
	op = contexto->ax;
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



void decodificar (char* instruccion_en_bruto){

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

	int accede_Memoria = 0;

	operando_1_APUNTA = buscaOperando(operando_1_NOMBRE);
	operando_2_APUNTA = buscaOperando(operando_2_NOMBRE);

	if (accede_Memoria == 0){
		sleep(tiempo_demora);
	}


}

//int ins_set(instruccion* instruct){
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

int ins_add(void){
	operando_2_APUNTA = ins_add2(operando_1_APUNTA, operando_2_APUNTA);

	return ins_set();

}

int ins_io(void){

	io = operando_1_NOMBRE;
	unidades = operando_2_APUNTA;

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


int ciclo_instrucciones(t_contexto_ejecucion* pcb_kernel)
{

	int devuelve = OPTIMO;

	//fetch
	char *instruccion_en_bruto;
	fetch(pcb_kernel, &instruccion_en_bruto);


	//decodificar
	decodificar(instruccion_en_bruto);

	//ejecutar
	devuelve = ejecutar();

	//check interrupt
	devuelve = check_interrupt(devuelve);

	//actualizar ciclo
	pcb_kernel->program_counter ++;

	return devuelve;


}
