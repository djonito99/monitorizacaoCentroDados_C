#ifndef PERSISTENCIA_H
#define PERSISTENCIA_H

#include "estado.h"

#define FICHEIRO_SENSORES "data/sensores.bin"
#define FICHEIRO_ALERTAS "data/alertas.bin"
#define FICHEIRO_HISTORICO "data/historico.bin"

int persistencia_guardar_sensores(
    const Sensor *sensores
);

int persistencia_carregar_sensores(
    Sensor **sensores
);

int persistencia_guardar_alertas(
    const Alerta *alertas
);

int persistencia_carregar_alertas(
    Alerta **alertas,
    FilaAlertas *fila,
    int *proximo_id
);

int persistencia_guardar_historico(
    const Sincronizacao *historico
);

int persistencia_carregar_historico(
    Sincronizacao **historico
);

int persistencia_guardar_tudo(
    const EstadoAplicacao *aplicacao
);

int persistencia_carregar_tudo(
    EstadoAplicacao *aplicacao
);

#endif