#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "historico.h"

static void obter_data_hora_atual(
    char *destino,
    size_t tamanho
)
{
    time_t instante;
    struct tm *data_local;

    if (destino == NULL || tamanho == 0) {
        return;
    }

    instante = time(NULL);

    if (instante == (time_t)-1) {
        destino[0] = '\0';
        return;
    }

    data_local = localtime(&instante);

    if (data_local == NULL) {
        destino[0] = '\0';
        return;
    }

    if (
        strftime(
            destino,
            tamanho,
            "%Y-%m-%d %H:%M:%S",
            data_local
        ) == 0
    ) {
        destino[0] = '\0';
    }
}

Sincronizacao *historico_criar_registo(
    ResultadoSincronizacao resultado,
    int leituras_recebidas,
    int sensores_inseridos,
    int sensores_atualizados,
    int sensores_mantidos,
    int registos_rejeitados,
    int alertas_gerados
)
{
    Sincronizacao *novo;

    novo = malloc(sizeof(Sincronizacao));

    if (novo == NULL) {
        return NULL;
    }

    obter_data_hora_atual(
        novo->data_hora,
        sizeof(novo->data_hora)
    );

    novo->resultado = resultado;
    novo->leituras_recebidas = leituras_recebidas;
    novo->sensores_inseridos = sensores_inseridos;
    novo->sensores_atualizados = sensores_atualizados;
    novo->sensores_mantidos = sensores_mantidos;
    novo->registos_rejeitados = registos_rejeitados;
    novo->alertas_gerados = alertas_gerados;

    novo->proximo = NULL;

    return novo;
}

int historico_empilhar(
    Sincronizacao **topo,
    Sincronizacao *registo
)
{
    if (topo == NULL || registo == NULL) {
        return 0;
    }

    registo->proximo = *topo;
    *topo = registo;

    return 1;
}

const Sincronizacao *historico_consultar_ultima(
    const Sincronizacao *topo
)
{
    return topo;
}

const char *historico_resultado_para_texto(
    ResultadoSincronizacao resultado
)
{
    if (resultado == SINCRONIZACAO_SUCESSO) {
        return "Sucesso";
    }

    return "Falha";
}

static void historico_mostrar_registo(
    const Sincronizacao *registo
)
{
    if (registo == NULL) {
        return;
    }

    printf("Data/hora:             %s\n", registo->data_hora);
    printf(
        "Resultado:             %s\n",
        historico_resultado_para_texto(registo->resultado)
    );
    printf(
        "Leituras recebidas:    %d\n",
        registo->leituras_recebidas
    );
    printf(
        "Sensores inseridos:    %d\n",
        registo->sensores_inseridos
    );
    printf(
        "Sensores atualizados:  %d\n",
        registo->sensores_atualizados
    );
    printf(
        "Sensores mantidos:     %d\n",
        registo->sensores_mantidos
    );
    printf(
        "Registos rejeitados:   %d\n",
        registo->registos_rejeitados
    );
    printf(
        "Alertas gerados:       %d\n",
        registo->alertas_gerados
    );
}

void historico_listar_ultimas(
    const Sincronizacao *topo,
    int quantidade
)
{
    const Sincronizacao *atual;
    int contador;

    if (topo == NULL) {
        printf("O historico esta vazio.\n");
        return;
    }

    if (quantidade <= 0) {
        printf("A quantidade deve ser superior a zero.\n");
        return;
    }

    atual = topo;
    contador = 0;

    while (atual != NULL && contador < quantidade) {
        printf("\n--- Sincronizacao %d ---\n", contador + 1);
        historico_mostrar_registo(atual);

        atual = atual->proximo;
        contador++;
    }
}

int historico_contar(
    const Sincronizacao *topo
)
{
    const Sincronizacao *atual;
    int quantidade;

    atual = topo;
    quantidade = 0;

    while (atual != NULL) {
        quantidade++;
        atual = atual->proximo;
    }

    return quantidade;
}

void historico_libertar(
    Sincronizacao **topo
)
{
    Sincronizacao *atual;
    Sincronizacao *seguinte;

    if (topo == NULL) {
        return;
    }

    atual = *topo;

    while (atual != NULL) {
        seguinte = atual->proximo;
        free(atual);
        atual = seguinte;
    }

    *topo = NULL;
}