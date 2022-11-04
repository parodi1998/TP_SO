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
lista_operaciones* lista_ope;
registros* contexto;


//FUNCIONES DE PRUEBA
int start_prueba(void){
	int cero = 0;

	Lista_Instrucciones lista1;

	lista1.id = 1;
	lista1.instruccion = "SET AX 100";

	Lista_Instrucciones lista2;

	lista2.id = 2;
	lista2.instruccion = "SET AX 50";
	lista1.lista_siguiente = &lista2;

	Lista_Instrucciones lista3;

	lista3.id = 3;
	lista3.instruccion = "SET BX 10";
	lista2.lista_siguiente = &lista3;

	Lista_Instrucciones lista4;

	lista4.id = 4;
	lista4.instruccion = "ADD AX BX";
	lista3.lista_siguiente = &lista4;

	Lista_Instrucciones lista5;

	lista5.id = 5;
	lista5.instruccion = "EXIT";
	lista4.lista_siguiente = &lista5;
	lista5.lista_siguiente = NULL;

	pcb pcb1;
	pcb1.id = malloc(sizeof(int));
	pcb1.id = 1;
	pcb1.instrucciones = malloc(sizeof(Lista_Instrucciones*));
	pcb1.instrucciones = &lista1;
	pcb1.program_counter = malloc(sizeof(int));
	pcb1.program_counter = 2;
	pcb1.reg_general.ax = malloc(sizeof(int));
	pcb1.reg_general.ax = 2;
	pcb1.reg_general.bx = malloc(sizeof(int));
	pcb1.reg_general.bx = 1;
	pcb1.reg_general.cx = malloc(sizeof(int));
	pcb1.reg_general.cx = 0;
	pcb1.reg_general.dx = malloc(sizeof(int));
	pcb1.reg_general.dx = 0;

	seguir_instrucciones(&pcb1);
	printf("Exitos");
	return cero;
}


void* funcion_hilo(void* p){
  // Print value received as argument:
  printf("Value recevied as argument in starting routine: ");
  printf("%i\n", * (int*)p);

  // Return reference to global variable:
  pthread_exit(&i);
}

int sePuedeConvertirEnInt(char* palabra){
	int valor = atoi( palabra);
	int sePuede = 0;
	if (sizeof(valor) <= 4)
		sePuede = 1;
	else{
		sePuede = 0;
	}
	return sePuede;
}


void* seguir_instrucciones(pcb* pcb_kernel){
	int devolucion = OPTIMO;
	lista_ope = todas_operaciones();
	contexto = &(pcb_kernel->reg_general);
	do {
		devolucion = ciclo_instrucciones(pcb_kernel);

	} while (devolucion == OPTIMO);

printf("OK");

printf("AX: nos queda: %d  ", contexto->ax);
}

  //pthread_t id;

  //int j = 1;
  //pthread_create(&id, NULL, funcion_hilo, &j);

  //int* ptr;

  // Wait for foo() and retrieve value in ptr;
//  pthread_join(id, (void**)&ptr);



char* fetch(pcb* pcb_kernel)
{
	char *instruccion_en_bruto;
	int continuar = 1;
	Lista_Instrucciones* lista_buscar = pcb_kernel->instrucciones;
	int instruccion_buscada = pcb_kernel->program_counter;


	do{

	if (lista_buscar->id != instruccion_buscada){
		if (lista_buscar->lista_siguiente == NULL){
			continuar = 0;
			}
			else
			{
				lista_buscar = lista_buscar->lista_siguiente;
			}
		}
		else{
			instruccion_en_bruto = lista_buscar->instruccion;
			continuar = 0;
		}

	} while (continuar == 1);

		int a = lista_buscar->id;

	return instruccion_en_bruto;
}

lista_operaciones* todas_operaciones(void){
	//char* path_1 = getcwd(NULL, 0);
	//path_1 = concatenar(path_1, "/instrucciones.config");
//	char numero;
	lista_operaciones* lista = malloc(sizeof(lista_operaciones*));

	//for (int i = 0; i <= 5; i++){
		//numero = i + '0';
		//printf("%s \n",numero);
		//lista[i].t_instruccion = numero;
		//lista[i].nombre_instruccion = config_get_string_value(path_1, numero);
	//}

	lista_operaciones listita;
	listita.t_instruccion = 0;
	listita.nombre_instruccion = "SET";
	lista[0] = listita;

	listita.t_instruccion = 1;
		listita.nombre_instruccion = "ADD";
		lista[1] = listita;

		listita.t_instruccion = 2;
			listita.nombre_instruccion = "MOV_IN";
			lista[2] = listita;

			listita.t_instruccion = 3;
				listita.nombre_instruccion = "MOV_OUT";
				lista[3] = listita;

				listita.t_instruccion = 4;
					listita.nombre_instruccion = "I/O";
					lista[4] = listita;

					listita.t_instruccion = 5;
						listita.nombre_instruccion = "EXIT";
						lista[5] = listita;



	return lista;

}

int buscarValorOperacion (char* operacion_charm){
	int numero;
	int encontrado = 0;
	printf("Operacion_charm: %s ++", operacion_charm);
	lista_operaciones  operacion_buscada = lista_ope[i];
	for (int i = 0; i < 10; i++){
		numero = operacion_buscada.t_instruccion;
		if (operacion_charm == operacion_buscada.nombre_instruccion){
			encontrado = 1;
			break;
		}else
		{
			printf("operacion_ buscada: %s ++", operacion_buscada.nombre_instruccion);
		}
	}
	if (encontrado == 0) numero = -1;
	return numero;
}

int esABCD(char* valor, int* apunta){
	int cierto;
	if (valor[1] == 'X'){

	switch(valor[0]) {
	  case 'A':
	    cierto = 1;
	    *apunta = contexto->ax;
	    break;
	  case 'B':
	    cierto = 1;
	    *apunta = contexto->bx;
	    break;
	  case 'C':
		cierto = 1;
		*apunta = contexto->cx;
		    break;
	  case 'D':
		cierto = 1;
		*apunta = contexto->dx;
		    break;
	  default:
		    cierto = 0;
	}
	} else cierto = 1;
	return cierto;
}

int llamaMemoria(operando* operacion){
	//1 si tiene que llamar a memoria
	int llama = 0;
	//es un valor?
	if (esABCD(operacion->nombre, &(operacion->apunta)) == 0){
		if (sePuedeConvertirEnInt(operacion->nombre) == 1){
			llama = 1;
			operacion->apunta = (int)(operacion->nombre);
		}
	}

	return llama;
}

void accederMemoria(operando* op){
	//CAMBIAR LUEGO
	op->apunta = contexto->ax;
}




instruccion decodificar(char* instruccion_en_bruto){
	instruccion instruct;
//	char* blanco = " ";
//	int m = strlen(instruccion_en_bruto);
	//char n[m] = (char[m])instruccion_en_bruto;
		//	instruccion_en_bruto;
//	char** palabras_instrucciones = string_array_new();
	//palabras_instrucciones = string_split(instruccion_en_bruto, blanco);
	//char** palabras_instrucciones = string_split(instruccion_en_bruto, blanco);
	//palabras_instrucciones = string_split("Hola", blanco);
	//char* operacion_ext =  strtok(instruccion_en_bruto, blanco);

	//char* operacion_ext =  string_split(instruccion_en_bruto, blanco)[0];
	printf("INS: %s", instruccion_en_bruto);
	int a = strlen(instruccion_en_bruto);
	char atributo_1[20];
	char atributo_2[20];
	char atributo_3[20];
	int buscador = 1;
	int j = 0;
	int k = 0;
	char letra;
	for (int i = 0; i < a; i++){
				if (instruccion_en_bruto[i] == ' ')
				{
					buscador++;
				}
				else
				{
					letra = instruccion_en_bruto[i];
					switch(buscador){
					 case 1:
						 atributo_1[i] = letra;
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
	instruct.operacion = buscarValorOperacion(atributo_1);
	instruct.operando_1.nombre = atributo_2;
	instruct.operando_2.nombre = atributo_3;

	int accede_Memoria = 0;

	if  (llamaMemoria(&(instruct.operando_1)) == 1){
		accede_Memoria = 1;
		accederMemoria(&(instruct.operando_1));
	}

	if  (llamaMemoria(&(instruct.operando_2)) == 1){
			accede_Memoria = 1;
			accederMemoria(&(instruct.operando_2));
		}

	if (accede_Memoria == 0){
		sleep(tiempo_demora);
	}

	return instruct;
}

int ins_set(instruccion* instruct){
	instruct->operando_1.apunta = instruct->operando_2.apunta;
	return OPTIMO;
}

int ins_add(instruccion* instruct){
	instruct->operando_1.apunta += instruct->operando_2.apunta;
	return OPTIMO;
}

//void ins_mov_in(instruccion instruct){
	//int valor = traer_Dato(instruct.operando_2);
	//do_set(instruct.operando_1, valor);
//}

//void ins_mov_out(instruccion instruct){

//}

int ins_io(instruccion* instruct){
	return BLOQUEO;
}

int  ins_exit(instruccion* instruct){
	return FINALIZADO;
}

int ejecutar(instruccion* instruct){
	int estado = OPTIMO;
	int opera = instruct->operacion;
	printf("instruccion: %d", opera);
	switch(opera){
	case SET:
		estado = ins_set(&instruct);
		break;
	case ADD:
		estado = ins_add(&instruct);
		break;
	case IO:
		estado = ins_io(&instruct);
		break;
	case EXIT:
		estado = ins_exit(&instruct);
		break;
	default:
		printf("Error");
		estado = INTERRUPCION;
	}
	return estado;
}





int check_interrupt(int devuelve){

	//desactivar semáforo ckeck_kernel
	if (interrumpido > 0){
		devuelve = INTERRUPCION;
	}
	//activar semáforo check_kernel
	return devuelve;
}


int ciclo_instrucciones(pcb* pcb_kernel)
{
	int devuelve = OPTIMO;
	//fetch
	char *instruccion_en_bruto = fetch(pcb_kernel);

	//decodificar
	instruccion instruct = decodificar(instruccion_en_bruto);
	printf("+Valor: %s", instruct.operando_1.nombre);

	//ejecutar
	printf("valor op: %d", instruct.operacion);
	devuelve = ejecutar(&instruct);

	//check interrupt
	//devuelve = check_interrupt(devuelve);

	//actualizar ciclo
	pcb_kernel->program_counter ++;

	return devuelve;


}
