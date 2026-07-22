#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sensores.h"

 // Copia uma string sem ultrapassar o tamanho do destino.
static void copiar_texto(
    char *destino,
    size_t tamanho_destino,
    const char *origem
)
{
    if (destino == NULL || tamanho_destino == 0) {
        return;
    }

    if (origem == NULL) {
        destino[0] = '\0';
        return;
    }

    snprintf(destino, tamanho_destino, "%s", origem);
}

/*
 * Copia todos os dados de uma leitura para um sensor existente.
 * O ponteiro para o proximo elemento nao e alterado.
 */
static void copiar_dados_sensor(
    Sensor *destino,
    const Sensor *origem
)
{
    if (destino == NULL || origem == NULL) {
        return;
    }

    copiar_texto(
        destino->data_hora,
        sizeof(destino->data_hora),
        origem->data_hora
    );

    copiar_texto(
        destino->codigo,
        sizeof(destino->codigo),
        origem->codigo
    );

    copiar_texto(
        destino->descricao,
        sizeof(destino->descricao),
        origem->descricao
    );

    copiar_texto(
        destino->tipo,
        sizeof(destino->tipo),
        origem->tipo
    );

    destino->valor = origem->valor;

    copiar_texto(
        destino->unidade,
        sizeof(destino->unidade),
        origem->unidade
    );

    copiar_texto(
        destino->estado,
        sizeof(destino->estado),
        origem->estado
    );

    destino->severidade = origem->severidade;

    copiar_texto(
        destino->mensagem,
        sizeof(destino->mensagem),
        origem->mensagem
    );
}

/*
 * Verifica se ocorreu uma alteracao que deve ser contabilizada.
 *
 * Segundo o enunciado, uma mudanca apenas na data e hora nao conta como atualizacao.
 */
static int sensor_tem_alteracao_relevante(
    const Sensor *existente,
    const Sensor *novo
)
{
    if (existente == NULL || novo == NULL) {
        return 0;
    }

    if (existente->valor != novo->valor) {
        return 1;
    }

    if (strcmp(existente->estado, novo->estado) != 0) {
        return 1;
    }

    if (existente->severidade != novo->severidade) {
        return 1;
    }

    if (strcmp(existente->mensagem, novo->mensagem) != 0) {
        return 1;
    }

    return 0;
}

Sensor *sensor_criar(
    const char *data_hora,
    const char *codigo,
    const char *descricao,
    const char *tipo,
    double valor,
    const char *unidade,
    const char *estado,
    int severidade,
    const char *mensagem
)
{
    Sensor *novo;

    novo = malloc(sizeof(Sensor));

    if (novo == NULL) {
        return NULL;
    }

    copiar_texto(
        novo->data_hora,
        sizeof(novo->data_hora),
        data_hora
    );

    copiar_texto(
        novo->codigo,
        sizeof(novo->codigo),
        codigo
    );

    copiar_texto(
        novo->descricao,
        sizeof(novo->descricao),
        descricao
    );

    copiar_texto(
        novo->tipo,
        sizeof(novo->tipo),
        tipo
    );

    novo->valor = valor;

    copiar_texto(
        novo->unidade,
        sizeof(novo->unidade),
        unidade
    );

    copiar_texto(
        novo->estado,
        sizeof(novo->estado),
        estado
    );

    novo->severidade = severidade;

    copiar_texto(
        novo->mensagem,
        sizeof(novo->mensagem),
        mensagem
    );

    novo->proximo = NULL;

    return novo;
}

Sensor *sensor_pesquisar_por_codigo(
    Sensor *lista,
    const char *codigo
)
{
    Sensor *atual;

    if (codigo == NULL) {
        return NULL;
    }

    atual = lista;

    while (atual != NULL) {
        if (strcmp(atual->codigo, codigo) == 0) {
            return atual;
        }

        atual = atual->proximo;
    }

    return NULL;
}

ResultadoSensor sensor_inserir_ou_atualizar(
    Sensor **lista,
    const Sensor *leitura
)
{
    Sensor *existente;
    Sensor *novo;
    int alterado;

    if (lista == NULL || leitura == NULL) {
        return SENSOR_ERRO;
    }

    if (leitura->codigo[0] == '\0') {
        return SENSOR_ERRO;
    }

    existente = sensor_pesquisar_por_codigo(
        *lista,
        leitura->codigo
    );

    /*
     * O sensor ainda nao existe.
     */
    if (existente == NULL) {
        novo = sensor_criar(
            leitura->data_hora,
            leitura->codigo,
            leitura->descricao,
            leitura->tipo,
            leitura->valor,
            leitura->unidade,
            leitura->estado,
            leitura->severidade,
            leitura->mensagem
        );

        if (novo == NULL) {
            return SENSOR_ERRO;
        }

        /*
         * Insercao no inicio da lista.
         */
        novo->proximo = *lista;
        *lista = novo;

        return SENSOR_INSERIDO;
    }

    /*
     * O sensor já existe.
     */
    alterado = sensor_tem_alteracao_relevante(
        existente,
        leitura
    );

    if (alterado) {
        copiar_dados_sensor(existente, leitura);
        return SENSOR_ATUALIZADO;
    }

    /*
     * Apenas a data/hora pode ter mudado.
     * Atualiza o campo, mas nao se contabiliza como atualizacao.
     */
    copiar_texto(
        existente->data_hora,
        sizeof(existente->data_hora),
        leitura->data_hora
    );

    return SENSOR_MANTIDO;
}

void sensores_listar(const Sensor *lista)
{
    const Sensor *atual;

    if (lista == NULL) {
        printf("\nNao existem sensores registados.\n");
        return;
    }

    atual = lista;

    printf("\n");
    printf("============================================================\n");
    printf("                    LISTA DE SENSORES\n");
    printf("============================================================\n");

    while (atual != NULL) {
        printf("Codigo:      %s\n", atual->codigo);
        printf("Descricao:   %s\n", atual->descricao);
        printf("Tipo:        %s\n", atual->tipo);
        printf("Valor:       %.2f %s\n",
               atual->valor,
               atual->unidade);
        printf("Estado:      %s\n", atual->estado);
        printf("Severidade:  %d\n", atual->severidade);
        printf("Mensagem:    %s\n", atual->mensagem);
        printf("Data/hora:   %s\n", atual->data_hora);
        printf("------------------------------------------------------------\n");

        atual = atual->proximo;
    }
}

void sensores_filtrar_por_tipo(
    const Sensor *lista,
    const char *tipo
)
{
    const Sensor *atual;
    int encontrados;

    if (tipo == NULL) {
        return;
    }

    atual = lista;
    encontrados = 0;

    while (atual != NULL) {
        if (strcmp(atual->tipo, tipo) == 0) {
            printf(
                "%s | %s | %.2f %s | %s | severidade %d\n",
                atual->codigo,
                atual->descricao,
                atual->valor,
                atual->unidade,
                atual->estado,
                atual->severidade
            );

            encontrados++;
        }

        atual = atual->proximo;
    }

    if (encontrados == 0) {
        printf("Nao foram encontrados sensores desse tipo.\n");
    }
}

void sensores_filtrar_por_estado(
    const Sensor *lista,
    const char *estado
)
{
    const Sensor *atual;
    int encontrados;

    if (estado == NULL) {
        return;
    }

    atual = lista;
    encontrados = 0;

    while (atual != NULL) {
        if (strcmp(atual->estado, estado) == 0) {
            printf(
                "%s | %s | %.2f %s | severidade %d\n",
                atual->codigo,
                atual->descricao,
                atual->valor,
                atual->unidade,
                atual->severidade
            );

            encontrados++;
        }

        atual = atual->proximo;
    }

    if (encontrados == 0) {
        printf("Nao foram encontrados sensores nesse estado.\n");
    }
}

int sensores_contar(const Sensor *lista)
{
    const Sensor *atual;
    int quantidade;

    atual = lista;
    quantidade = 0;

    while (atual != NULL) {
        quantidade++;
        atual = atual->proximo;
    }

    return quantidade;
}

void sensores_libertar(Sensor **lista)
{
    Sensor *atual;
    Sensor *seguinte;

    if (lista == NULL) {
        return;
    }

    atual = *lista;

    while (atual != NULL) {
        seguinte = atual->proximo;
        free(atual);
        atual = seguinte;
    }

    *lista = NULL;
}