#include "../include/conexiones_cpu.h"
#include "../../shared/include/protocolo.h"


int kernel = 1;
int interrumpir = 0;
sem_t* comunicacion_kernel;
int alfa = 0;

pthread_t hilo_dispatch;
pthread_t hilo_interrupt;

char * concatenar(char* palabra1, char* palabra2)
{
	 char *palabraFinal = string_new();
	string_append(&palabraFinal, palabra1);
	string_append(&palabraFinal, palabra2);
	return palabraFinal;
}

//Acá recibe instrucciones del Kernel
//void recibir_Instrucciones_Kernel (int conexion_Kernel_Dispatch)
//{
	//char* mensajeRecibido = NULL;
	
//	while(&kernel == 1)
//	{	
	//mensajeRecibido	= recibirMensaje(conexion_Kernel_Dispatch);
	//if (mensajeRecibido != NULL)
	//{
		//PROCESAR INSTRUCCIONES
	//	printf('Instrucciones Recibidas');
	//	//		sem_wait(&comunicacion_kernel_dispatch);
	//}
	//else
	//usleep(1000);

	//SE ACLARA QUE SE CODEÓ PARA QUE RECIBA UNA SERIE DE INSTRUCCIONES POR VEZ. SE PUEDE UTILIZAR UN PROCESO DE HILOS Y COLAS PARA QUE RECIBA MÁS.


//	}

//}

//Acá recibe interrupciones del Kernel
//void recibir_Interrupciones_Kernel (int conexion_Kernel_Interrupt)
//{
	
//	while(&kernel == 1)
//	{
//	//mensajeRecibido	= recibirMensaje(conexion_Kernel_Interrupt);
	//if (mensajeRecibido != NULL)
	//{
		//PROCESAR INTERRUPCIONES
	//	printf('Interrupción');
		///SI ES NECESARIO, ABORTA->
		//&kernel = 0;
	//}
	//else
	//usleep(1000);
//	}

//}

//Acá recibe la informacion de la Memoria
//void recibir_Memoria (int conexion_Memoria)
//{
//	while (1==1)
//	{
//		sem_wait(&comunicacion_memoria);
//	}
//}

void esperarInterrupcion(){
	while(interrumpir == 0){
		int socket_interrupcion;
		//socket_interrupcion = cpu_conexion_server(2, get_puerto_escucha_interrupt());
		if (socket_interrupcion > 0)
		{
			interrumpe();
			interrumpir = 1;
		}
	}
}

t_contexto_ejecucion iniciar_proceso(t_pcb* pcb1){
	//pthread_t thread1, thread2;
	//int  iret1, iret2;

	/* Create independent threads each of which will execute function */

	//FUNCION DE PRUEBA
	t_contexto_ejecucion contexto_ejecucion;
	contexto_ejecucion.estado = OPTIMO;
	//contexto_ejecucion.instrucciones = pcb1.instrucciones;
	contexto_ejecucion.program_counter = pcb1->program_counter;
	contexto_ejecucion.reg_general.ax = pcb1->registro_AX;
	contexto_ejecucion.reg_general.bx = pcb1->registro_BX;
	contexto_ejecucion.reg_general.cx = pcb1->registro_CX;
	contexto_ejecucion.reg_general.dx = pcb1->registro_DX;

	log_info(get_log(),"ya cargue el contexto");

	// 	iret1 = pthread_create( &thread1, NULL, esperarInterrupcion, NULL);
	//	iret2 = pthread_create( &thread2, NULL, seguir_instrucciones, (&contexto_ejecucion, pcb1.tabla_segmentos, pcb1.id_proceso));
	seguir_instrucciones(&contexto_ejecucion, pcb1->instrucciones, pcb1->id_proceso);

	log_info(get_log(),"contexto_ejecucion.program_counter: %d", contexto_ejecucion.program_counter);
	   // pthread_join( thread2, NULL);
	interrumpir = 1;
	  // pthread_join( thread1, NULL);
	return contexto_ejecucion;
}

static void actualizar_pcb(t_contexto_ejecucion contexto, t_pcb* proceso) {
	proceso->program_counter = contexto.program_counter;
	switch (contexto.estado) {
			case BLOQUEO:
                break;
			case INTERRUPCION:
                break;
			case PAGE_DEFAULT:
                break;
			case FINALIZADO:
				proceso->debe_ser_finalizado = true;
                break;
			default:
				break; 
		}
}

void comunicacion_cpu_kernel_distpach() {
	t_pcb* proceso;
	op_code cod_op;
	t_list* instrucciones;
    t_list* segmentos;
	t_contexto_ejecucion contexto;

	while(fd_client_kernel_dispatch != -1) {
		

		if(recv(fd_client_kernel_dispatch, &cod_op, sizeof(op_code), MSG_WAITALL) == -1)
			cod_op = -1;

		switch (cod_op) {
			case PCB_KERNEL:
				if(!recv_pcb(get_log(), fd_client_kernel_dispatch, &proceso)) {
                    log_error(get_log(),"Hubo un error al recibir el proceso de kernel");
                } else {
					contexto = iniciar_proceso(proceso);
					// estaria bueno meter un wait
					actualizar_pcb(contexto, proceso);
					// estaria bueno meter un wait
					if(!send_pcb(get_log(), fd_client_kernel_dispatch, proceso)) {
						log_error(get_log(),"Hubo un error enviando el proceso al kernel");
					} else {
						log_info(get_log(),"El proceso fue enviado a kernel");
					}
				}
                break;
			default:
				break; 
		}
	}
}

int start()
{
	//pthread_create(&hilo_interrupt, NULL, (void*) procesar_conexion, NULL);
    //pthread_detach(hilo_interrupt);

	pthread_create(&hilo_dispatch, NULL, (void*) comunicacion_cpu_kernel_distpach, NULL);
    pthread_join(hilo_dispatch, NULL);

	//ciclo_recibir_instruccines();
	//return alfa;
}
