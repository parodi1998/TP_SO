#include "../include/archivo.h"

t_list* cargar_instrucciones_de_archivo(FILE* archivo) {
    int buffer = 25;
 	char linea_de_codigo[buffer];
 	t_list* instrucciones = list_create();
    while (feof(archivo) == 0)
    {
        fgets(linea_de_codigo,buffer,archivo);
        linea_de_codigo[strcspn(linea_de_codigo, "\n")] = '\0';
        char* instruccion = string_new();
        string_append(&instruccion, linea_de_codigo);
        list_add(instrucciones, instruccion);
    }
    return instrucciones;
}