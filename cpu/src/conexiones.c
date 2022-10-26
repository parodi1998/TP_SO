#include "conexiones.h"

#include<commons/collections/list.h>

int kernel = 1;
sem_t* comunicacion_kernel;

char * concatenar(char* palabra1, char* palabra2)
{
	 char *palabraFinal = string_new();
	string_append(&palabraFinal, palabra1);
	string_append(&palabraFinal, palabra2);
	return palabraFinal;
}

cpu_config* cargarConfiguracion(t_config* rutaConfiguracion, t_log* logger){
	cpu_config* configuracion_Cpu = malloc(sizeof(cpu_config));
	
	configuracion_Cpu->entrada = malloc(sizeof(int));
	int in_entrada = config_get_int_value(rutaConfiguracion, "ENTRADAS_TLB");
	if (in_entrada == NULL)
	{
		log_info(logger, "ERROR");
	}
	else{
		configuracion_Cpu->entrada = in_entrada;
		log_info(logger, "Entrada: %d \n", configuracion_Cpu->entrada);
	}
	configuracion_Cpu->entrada = config_get_int_value(rutaConfiguracion, "ENTRADAS_TLB");
	
	configuracion_Cpu->reemplazo = malloc(sizeof(int));
	char* in_reemplazo = config_get_string_value(rutaConfiguracion, "REEMPLAZO_TLB");
	if (in_reemplazo == NULL)
	{
		log_info(logger, "ERROR");
	}
	else
	{
		configuracion_Cpu->reemplazo = in_reemplazo;
		log_info(logger, "REEMPLAZO_TLB: %s \n", configuracion_Cpu->reemplazo);
	}

	configuracion_Cpu->retardo = malloc(sizeof(int));
	int in_retardo;
	in_retardo = config_get_int_value(rutaConfiguracion, "RETARDO_INSTRUCCION");
	if (in_retardo == NULL)
		{
			log_info(logger, "ERROR");
		}
		else
		{
			configuracion_Cpu->retardo = in_retardo;
			log_info(logger, "RETARDO_INSTRUCCION: %d \n", configuracion_Cpu->retardo);
		}

	configuracion_Cpu->ip_memoria = malloc(sizeof(int));
	char* in_ip_memoria;
	in_ip_memoria = config_get_string_value(rutaConfiguracion, "IP_MEMORIA");
	if(in_ip_memoria == NULL)
	{
		log_info(logger, "ERROR");
	}
	else
	{
		configuracion_Cpu->ip_memoria = in_ip_memoria;
		log_info(logger, "IP_MEMORIA: %s \n", configuracion_Cpu->ip_memoria);
	}
	
	configuracion_Cpu->puerto_memoria = malloc(sizeof(int));
	int in_puerto_memoria;
	in_puerto_memoria = config_get_int_value(rutaConfiguracion, "PUERTO_MEMORIA");
	if(in_puerto_memoria == NULL)
		{
			log_info(logger, "ERROR");
		}
		else
		{
			configuracion_Cpu->puerto_memoria = in_puerto_memoria;
			log_info(logger, "PUERTO_MEMORIA: %d \n", configuracion_Cpu->puerto_memoria);
		}
	
	configuracion_Cpu->puerto_escucha_dispatch = malloc(sizeof(int));
	int puerto_escucha_dispatch;
	puerto_escucha_dispatch = config_get_int_value(rutaConfiguracion, "PUERTO_ESCUCHA_DISPATCH");
	if(puerto_escucha_dispatch == NULL)
			{
				log_info(logger, "ERROR");
			}
			else
			{
				configuracion_Cpu->puerto_escucha_dispatch = puerto_escucha_dispatch;
				log_info(logger, "PUERTO_ESCUCHA_DISPATCH: %d \n", configuracion_Cpu->puerto_memoria);
			}
	configuracion_Cpu->puerto_escucha_interrupt = malloc(sizeof(int));
	int in_puerto_escucha_interrupt;
	in_puerto_escucha_interrupt = config_get_int_value(rutaConfiguracion, "PUERTO_ESCUCHA_INTERRUPT");
	if (in_puerto_escucha_interrupt == NULL)
	{
		log_info(logger, "ERROR");
	}
	else
	{
		configuracion_Cpu->puerto_escucha_interrupt = in_puerto_escucha_interrupt;
		log_info(logger, "PUERTO_ESCUCHA_INTERRUPT: %d \n", configuracion_Cpu->puerto_escucha_interrupt);

	}
	
	return configuracion_Cpu;
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

int start(int arg)
{

	int conexion_Memoria;
	int conexion_Kernel_Dispatch;
	int conexion_Kernel_Interrupt;
//	lista_operaciones* lista;
//	lista = todas_operaciones(void);
	
	cpu_config* configuracion_Cpu;
	
	char* valor;
	t_log* logger;
	t_config* config;

	/* ---------------- LOGGING ---------------- */
	logger = iniciar_logger();

	log_info(logger, "Se inició el logger");


//	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

	config = iniciar_config();
	log_info(logger, "Se cargó la configuración");
	configuracion_Cpu = cargarConfiguracion(config, logger);


	//CONECTAR AL KERNEL
	//conexion_Kernel_Dispatch = crearConexion(IP, configuracion_Cpu->puerto_escucha_dispatch);
	//sem_init(comunicacion_kernel_dispatch, 1, 0);
	//pthread_t * hilo_instrucciones_kernel;
    	//pthread_create(hilo_instrucciones_kernel, NULL, recibir_Instrucciones_Kernel, &conexion_Kernel_Dispatch);
	
	//conexion_Kernel_Interrupt = crearConexion(IP, configuracion_Cpu->puerto_escucha_interrupt);
	//sem_init(comunicacion_kernel_interrupt, 1, 0);
	//pthread_t * hilo_interrupciones_kernel;
    	//pthread_create(hilo_interrupciones_kernel, NULL, recibir_Interrupciones_Kernel, &conexion_Kernel_Interrupt);


	//CONECTAR A MEMORIA
	log_info(logger, "Cargar memoria");
	//conexion_Memoria = crear_conexion(configuracion_Cpu->ip_memoria, htons(configuracion_Cpu->puerto_memoria));
	//conexion_Memoria = crear_conexion("127.0.0.1", "8092");
	//conexion_Memoria = crearConexionAuxiliar("127.0.0.1", 8050, logger);


	char* mensajeEnviar = "Acá voy";
	char* mensajeRecibido;


int regreso;

	//leer_consola();
	//enviar a memoria
	if (arg == 1)
	{
		//HANDSHAKE CON MEMORIA
		int cliente = generarCliente(4, configuracion_Cpu->ip_memoria, "8002");
		regreso = cliente;
	}
	else
	{
		//RECIBIR SEÑAL DE KERNEL
		int server = conexion_Server(4, "8002");
		regreso = server;
	}

		//recibir de memoria


		 //enviar a kernel
		// 		int conexion_kernel = iniciar_servidor2(logger, "cpu-kernel", configuracion_Cpu->ip_memoria, configuracion_Cpu->puerto_memoria);
	//	 		log_info(logger, "Conexion creada");
	//	 		int esperaKernel = esperar_cliente2(logger, "cpu-kernel", conexion_Memoria);
	//	 		enviar_mensaje2(mensajeEnviar, espera);
	//	 		liberar_conexion2(conexion_kernel);
		 		//recibir de memoria
	//	 		int socket_ClienteKernel = crear_conexion2(logger, "kernel-cpu", configuracion_Cpu->ip_memoria, configuracion_Cpu->puerto_memoria);
	//	 		 recibir_mensaje2(logger, socket_ClienteKernel);
	//	 		liberar_conexion2(socket_ClienteKernel);


	log_info(logger, "OK");
	//printf(mensajeRecibido);

	//sem_init(comunicacion_memoria, 1, 0);
	//pthread_t * hilo_memoria;
    	//pthread_create(hilo_memoria, NULL, comunicacion_memoria, &conexion_Memoria);

	//pthreadjoin(comunicacion_kernel_interrupt, Kernel);

	//terminar_programa(conexion_Memoria, logger, config);
	log_destroy(logger); //BORRAR LUEGO
	return regreso;
}

t_log* iniciar_logger(void)
{
	t_log* nuevo_logger;

	if ((nuevo_logger = log_create("cpu.log", "CPU", 1, LOG_LEVEL_INFO)) == NULL)
	{
		printf("Error al crear logger\n");
		exit(1);
	}
	return nuevo_logger;
}


t_config* iniciar_config(void)
{
	t_config* nuevo_config;
	char *current_dir = getcwd(NULL, 0);
	char* path = getcwd(NULL, 0);
	path = concatenar(path, "/cpu.config");

	if ((nuevo_config = config_create(path)) == NULL)
	{
		printf("No pude leer la configuración\n");
		exit(1);
	}
	return nuevo_config;
}




void terminar_programa(int conexion, t_log* logger, t_config* config)
{

	if (logger != NULL)
	{
		log_destroy(logger);
	}
	if (config != NULL)
	{
		config_destroy(config);
	}
//	if (conexion != NULL)
//	{
//		conexion_destroy(conexion);
//	}

}
void leer_consola()
{
	printf("Leyendo");
	char* leido;

	while (1){
		leido = readline(">");
		if (leido != NULL)
		{
			break;
		}


		printf("%s\n", leido);
	//	if (leido == 'a')
	//		printf("bien \n");
	//	else if (leido == 'b')
	//		printf("rebien \n");

	}

	//free(leido);


}

int crearConexionAuxiliar(char* ip, int puerto, t_log* logger)
{
	struct sockaddr_in server;

	int fd;

	server.sin_family = AF_INET;
	server.sin_port = htons(puerto);
//	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_addr.s_addr = ip;
	bzero(&(server.sin_zero), 8);

	if ((fd = socket(AF_INET, SOCK_STREAM,0) ) <0){
		log_info(logger, "ERROR 1");
		exit(-1);
	}

	if(bind (fd, (struct sockaddr*)&server, sizeof(struct sockaddr*))==-1){
		for(puerto=1; puerto < 1000000; puerto++)
		{
			server.sin_port = htons(puerto);
			if (bind (fd, (struct sockaddr*)&server, sizeof(struct sockaddr*))==-1)
			{
				log_info(logger, "ERROR puerto: %d", puerto);
			}
			else
			{
				puerto = 1000000;
			}
		}
		if(bind (fd, (struct sockaddr*)&server, sizeof(struct sockaddr*))==-1)
		{log_info(logger, "ERROR 2");
		exit(-1);}
		else
		{
			log_info(logger, "EXITO");
		}

	}

	if(listen(fd,5)==-1){
		log_info(logger, "ERROR 3");
		exit(-1);
	}

	return fd;
}

//PEGAR FUNCIONES:
int iniciar_servidor2(t_log* logger, const char* name, char* ip, char* puerto) {
    int socket_servidor;
    struct addrinfo hints, *servinfo;

    // Inicializando hints
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // Recibe los addrinfo
    getaddrinfo(ip, puerto, &hints, &servinfo);

    bool conecto = false;

    // Itera por cada addrinfo devuelto
    for (struct addrinfo *p = servinfo; p != NULL; p = p->ai_next) {
        socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (socket_servidor == -1) // fallo de crear socket
            continue;

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            // Si entra aca fallo el bind
            close(socket_servidor);
            continue;
        }
        // Ni bien conecta uno nos vamos del for
        conecto = true;
        break;
    }

    if(!conecto) {
        free(servinfo);
        return 0;
    }

    listen(socket_servidor, SOMAXCONN); // Escuchando (hasta SOMAXCONN conexiones simultaneas)

    // Aviso al logger
    log_info(logger, "Escuchando en %s:%s (%s)\n", ip, puerto, name);

    freeaddrinfo(servinfo);

    return socket_servidor;
}

// ESPERAR CONEXION DE CLIENTE EN UN SERVER ABIERTO
int esperar_cliente2(t_log* logger, const char* name, int socket_servidor) {
    struct sockaddr_in dir_cliente;
    socklen_t tam_direccion = sizeof(struct sockaddr_in);

    int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

    log_info(logger, "Cliente conectado (a %s)\n", name);

    return socket_cliente;
}

// CLIENTE SE INTENTA CONECTAR A SERVER ESCUCHANDO EN IP:PUERTO
int crear_conexion2(t_log* logger, const char* server_name, char* ip, char* puerto) {
    struct addrinfo hints, *servinfo;

    // Init de hints
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // Recibe addrinfo
    getaddrinfo(ip, puerto, &hints, &servinfo);

    // Crea un socket con la informacion recibida (del primero, suficiente)
    int socket_cliente = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

    // Fallo en crear el socket
    if(socket_cliente == -1) {
        log_error(logger, "Error creando el socket para %s:%s", ip, puerto);
        return 0;
    }

    // Error conectando
    if(connect(socket_cliente, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
        log_error(logger, "Error al conectar (a %s)\n", server_name);
        freeaddrinfo(servinfo);
        return 0;
    } else
        log_info(logger, "Cliente conectado en %s:%s (a %s)\n", ip, puerto, server_name);

    freeaddrinfo(servinfo);

    return socket_cliente;
}

// CERRAR CONEXION
void liberar_conexion2(int* socket_cliente) {
    close(*socket_cliente);
    *socket_cliente = -1;
}


////////CLIENTE DESDE ACÁ
/////////////////////////

static void* serializar_paquete2(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}

static void crear_buffer2(t_paquete* paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

void enviar_mensaje2(char* mensaje, int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete2(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete2(paquete);
}

t_paquete* crear_paquete2(void)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = PAQUETE;
	crear_buffer2(paquete);
	return paquete;
}

void agregar_a_paquete2(t_paquete* paquete, void* valor, int tamanio)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}

void enviar_paquete2(t_paquete* paquete, int socket_cliente)
{
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete2(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

void eliminar_paquete2(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

/////////////SERVER EMPIEZA ACÁ
////////////////////////////////
////////////////////////////////
bool recibir_operacion2(op_code* cod_op, int socket_cliente) {
	if(recv(socket_cliente, cod_op, sizeof(op_code), MSG_WAITALL) != sizeof(op_code)) {
		return false;
	}
	return true;
}

void* recibir_buffer2(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje2(t_log* logger, int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer2(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje %s", buffer);
	free(buffer);
}

t_list* recibir_paquete2(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer2(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}

