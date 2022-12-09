#ifndef KERNEL_LOGS_H_
#define KERNEL_LOGS_H_

#include "commons.h"

/**
 * Logs obligatiorios
 */

void log_proceso_en_new(t_log* logger, t_pcb* proceso);
void log_procesos_en_ready(t_log* logger, t_list* procesos_fifo, t_list* procesos_rr, char* algoritmo);
void log_proceso_cambio_de_estado(t_log* logger, t_pcb* proceso);
void actualizar_estado_proceso(t_log* logger, t_pcb* proceso, t_estado_pcb nuevo_estado);
void log_motivo_de_bloqueo(t_log* logger, t_pcb* proceso, char* dispositivo);
void log_proceso_desalojado_por_quantum(t_log* logger, t_pcb* proceso);

#endif