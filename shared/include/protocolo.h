#ifndef PROTOCOLO_H_
#define PROTOCOLO_H_

#include "client_utils.h"
#include "server_utils.h"

bool send_instrucciones(int fd, t_list* instrucciones);
bool recv_instrucciones(int fd, t_list** instrucciones); 

#endif