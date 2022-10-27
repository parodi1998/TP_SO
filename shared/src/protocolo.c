#include "../include/protocolo.h"

// Para serparar las instrucciones, vamos a usar el caracter |
static char* serializar_instrucciones(t_list* instrucciones) {
    int index;  
    char* instrucciones_string = string_new();
    for(index = 0; index < list_size(instrucciones); index++) {
        string_append(&instrucciones_string,list_get(instrucciones,index));
        string_append(&instrucciones_string,"|");
    }
    return instrucciones_string;
}
/*
static t_list* deserializar_instrucciones(char* instrucciones_string) {
    
}
*/
bool send_instrucciones(int fd, t_list* instrucciones) {
    return enviar_mensaje_bool(INSTRUCCIONES, serializar_instrucciones(instrucciones), fd);
}

bool recv_instrucciones(int fd, t_list** instrucciones) {
    return true;
} 
