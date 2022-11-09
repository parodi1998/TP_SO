#ifndef SERVER_UTILS_H_
#define SERVER_UTILS_H_



void* recibir_buffer(int*, int);
t_list* recibir_paquete(int);
void recibir_mensaje(t_log*,int);
bool recibir_operacion(op_code* codigo, int socket_cliente);

#endif