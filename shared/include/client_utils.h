#ifndef CLIENT_UTILS_H_
#define CLIENT_UTILS_H_

#include "shared_commons.h"

void enviar_mensaje(char* mensaje, int socket_cliente);
bool enviar_mensaje_bool(op_code codigo,char* mensaje, int socket_cliente);
t_paquete* crear_paquete(void);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void eliminar_paquete(t_paquete* paquete);


#endif