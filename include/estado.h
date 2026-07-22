#ifndef ESTADO_H
#define ESTADO_H

#include "sensores.h"
#include "alertas.h"
#include "historico.h"

typedef struct EstadoAplicacao {
    Sensor *sensores;

    Alerta *alertas;
    FilaAlertas fila_alertas_pendentes;

    Sincronizacao *historico;

    int proximo_id_alerta;
} EstadoAplicacao;

void estado_inicializar(EstadoAplicacao *aplicacao);
void estado_libertar(EstadoAplicacao *aplicacao);

#endif