#include <stddef.h>

#include "estado.h"

void estado_inicializar(EstadoAplicacao *aplicacao)
{
    if (aplicacao == NULL) {
        return;
    }

    aplicacao->sensores = NULL;
    aplicacao->alertas = NULL;
    aplicacao->historico = NULL;

    fila_alertas_inicializar(
        &aplicacao->fila_alertas_pendentes
    );

    aplicacao->proximo_id_alerta = 1;
}

void estado_libertar(EstadoAplicacao *aplicacao)
{
    if (aplicacao == NULL) {
        return;
    }

    sensores_libertar(&aplicacao->sensores);

    alertas_libertar(
        &aplicacao->alertas,
        &aplicacao->fila_alertas_pendentes
    );

    historico_libertar(
        &aplicacao->historico
    );
}