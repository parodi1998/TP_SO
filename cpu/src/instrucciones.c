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
registros contexto;


void* funcion_hilo(void* p){
  // Print value received as argument:
  printf("Value recevied as argument in starting routine: ");
  printf("%i\n", * (int*)p);

  // Return reference to global variable:
  pthread_exit(&i);
}


void* seguir_instrucciones(pcb pcb_kernel){
	int devolucion = OPTIMO;
	int siguiente_instruccion;
	contexto = &(pcb_kernel.reg_general);
	do {
		devolucion = ciclo_instrucciones(&pcb_kernel);

	} while (devolucion == OPTIMO);


}

  //pthread_t id;

  //int j = 1;
  //pthread_create(&id, NULL, funcion_hilo, &j);

  //int* ptr;

  // Wait for foo() and retrieve value in ptr;
//  pthread_join(id, (void**)&ptr);



char* fetch(pcb pcb_en_cuestion)
{
	char *instruccion_en_bruto;

	int nro_instruccion = pcb_en_cuestion.program_counter;
	Lista_Instrucciones lista = pcb_en_cuestion.instrucciones;
	int continuar = 1;

	do{

		if (lista.id != nro_instruccion){
			lista = lista.lista_siguiente;
		}
		else{
			instruccion_en_bruto = lista.instruccion;
			continuar = 0;
		}

	} while (continuar == 1);


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

int esABCD(char* valor, int apunta){
	int cierto;
	switch(valor) {
	  case "AX":
	    cierto = 1;
	    apunta = &(contexto.ax);
	    break;
	  case "BX":
	    cierto = 1;
	    apunta = &(contexto.bx);
	    break;
	  case "CX":
		cierto = 1;
		apunta = &(contexto.cx);
		    break;
	  case "DX":
		cierto = 1;
		apunta = &(contexto.dx);
		    break;
	  default:
		    cierto = 0;
	}
	return cierto;
}

int llamaMemoria(operando operacion){
	//1 si tiene que llamar a memoria
	int llama = 0;
	//es un valor?
	if (esABCD(operacion.nombre, &(operacion.apunta)) == 0){
		if ((int)operacion.nombre >= 0){
			llama = 1;
			operacion.apunta = (int)(operacion.nombre);
		}
	}

	return llama;
}

void accederMemoria(operando op){
	//CAMBIAR LUEGO
	op.apunta = contexto.ax;
}


instruccion decodificar(char* instruccion_en_bruto){
	instruccion instruct = malloc(sizeof(instruccion));
	char** palabras_instrucciones string_split(instruccion_en_bruto, ' ');
	char * operacion_char = palabras_instrucciones[0];
	int largo = lenght(palabras_instrucciones);
	instruct.operacion = buscarValorOperacion(operacion_char);
	instruct.operando_1.nombre = palabras_instrucciones[1];
	instruct.operando_2.nombre = palabras_instrucciones[2];

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

int ins_set(instruccion instruct){
	instruct.operando_1.apunta = instruct.operando_2.apunta;
	return OPTIMO;
}

int ins_add(instruccion instruct){
	instruct.operando_1.apunta += instruct.operando_2.apunta;
	return OPTIMO;
}

void ins_mov_in(instruccion instruct){
	int valor = traer_Dato(instruct.operando_2);
	do_set(instruct.operando_1, valor);
}

//void ins_mov_out(instruccion instruct){

//}

int ins_io(instruccion instruct){
	return BLOQUEO;
}

int  ins_exit(instruccion instruct){
	return FINALIZADO;
}

int ejecutar(instruccion instruct){
	int estado = OPTIMO;
	switch(instruct.operacion){
	case SET:
		estado = in_set(&instruct);
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


int ciclo_instrucciones(pcb pcb_kernel)
{
	int devuelve = OPTIMO;
	//fetch
	char *instruccion_en_bruto = fetch(pcb_kernel);

	//decodificar
	instruccion instruct = decodificar(instruccion_en_bruto);

	//ejecutar
	devuelve = ejecutar(&instruct);

	//check interrupt
	devuelve = check_interrupt(devuelve);

	//actualizar ciclo
	pcb_kernel.program_counter = pcb_kernel.program_counter + 1;

	return devuelve;
}
