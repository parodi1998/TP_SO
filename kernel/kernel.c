#include "include/kernel.h"

void inicializar_listas(){
    contador = 0;
	colaNew = queue_create();
	colaReady = list_create();
	listaExit = list_create();
	//lista_instrucciones_kernel = list_create();

}
void inicializar_semaforos(){
    pthread_mutex_t mutexBlck;
    pthread_mutex_t mutexExe;
    pthread_mutex_t mutexExit;
    pthread_mutex_t multiprocesamiento;

    sem_t contadorNew;
    sem_t contadorReady;
    sem_t contadorExe;
    sem_t contadorBlock;
    sem_t multiprogramacion;
    sem_t largoPlazo;
	pthread_mutex_init(&mutexNew, NULL);
	pthread_mutex_init(&mutexRdy, NULL);
	pthread_mutex_init(&mutexBlck, NULL);
	pthread_mutex_init(&mutexExe, NULL);
	pthread_mutex_init(&mutexExit, NULL);
	sem_init(&contadorNew, 0, 0); // Estado New
	sem_init(&contadorReady, 0, 0); // Estado Ready
	sem_init(&contadorExe, 0, 0); // Estado Exe
	//sem_init(&multiprogramacion, 0, grado_multiprogramacion); // Memoria
	sem_init(&contadorBlock, 0, 0);
	sem_init(&largoPlazo, 0, 1);
}
int main(int argc, char** argv){

    inicializar_listas();
    inicializar_semaforos();
    
    if(!iniciar_programa()) {
        terminar_programa();
        return EXIT_SUCCESS;
    }

    int kernel_server_fd = 0;
    if(!iniciar_kernel(&kernel_server_fd)) {
        log_error(logger,"No se pudo generar iniciar el proceso kernel");
        terminar_programa();
        return EXIT_FAILURE;
    }

	log_info(logger, "Servidor listo para recibir al cliente");
    while (server_escuchar(logger, "kernel", kernel_server_fd));    

    liberar_conexion(&kernel_server_fd);
    terminar_programa();

    return EXIT_SUCCESS;
}