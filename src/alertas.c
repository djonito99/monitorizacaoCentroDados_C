#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "alertas.h"

static void copiar_texto_alerta(
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

static void obter_data_hora_atual(
    char *destino,
    size_t tamanho_destino
)
{
    time_t instante;
    struct tm *data_local;

    if (destino == NULL || tamanho_destino == 0) {
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
            tamanho_destino,
            "%Y-%m-%d %H:%M:%S",
            data_local
        ) == 0
    ) {
        destino[0] = '\0';
    }
}

void fila_alertas_inicializar(FilaAlertas *fila)
{
    if (fila == NULL) {
        return;
    }

    fila->inicio = NULL;
    fila->fim = NULL;
}

int fila_alertas_esta_vazia(const FilaAlertas *fila)
{
    if (fila == NULL) {
        return 1;
    }

    return fila->inicio == NULL;
}

void fila_alertas_enfileirar(
    FilaAlertas *fila,
    Alerta *alerta
)
{
    if (fila == NULL || alerta == NULL) {
        return;
    }

    alerta->proximo_fila = NULL;

    /*
     * Caso especial: fila vazia.
     */
    if (fila->fim == NULL) {
        fila->inicio = alerta;
        fila->fim = alerta;
        return;
    }

    /*
     * O antigo último passa a apontar para o novo.
     */
    fila->fim->proximo_fila = alerta;

    /*
     * O novo alerta passa a ser o último.
     */
    fila->fim = alerta;
}

Alerta *fila_alertas_desenfileirar(
    FilaAlertas *fila
)
{
    Alerta *primeiro;

    if (fila == NULL || fila->inicio == NULL) {
        return NULL;
    }

    primeiro = fila->inicio;

    fila->inicio = primeiro->proximo_fila;

    /*
     * Se foi removido o único elemento, a fila fica vazia.
     */
    if (fila->inicio == NULL) {
        fila->fim = NULL;
    }

    primeiro->proximo_fila = NULL;

    return primeiro;
}

Alerta *alerta_criar(
    int id,
    const char *codigo_sensor,
    const char *descricao,
    const char *estado_observado,
    int severidade
)
{
    Alerta *novo;

    if (
        codigo_sensor == NULL ||
        codigo_sensor[0] == '\0' ||
        descricao == NULL ||
        estado_observado == NULL
    ) {
        return NULL;
    }

    novo = malloc(sizeof(Alerta));

    if (novo == NULL) {
        return NULL;
    }

    novo->id = id;

    obter_data_hora_atual(
        novo->data_criacao,
        sizeof(novo->data_criacao)
    );

    copiar_texto_alerta(
        novo->codigo_sensor,
        sizeof(novo->codigo_sensor),
        codigo_sensor
    );

    copiar_texto_alerta(
        novo->descricao,
        sizeof(novo->descricao),
        descricao
    );

    copiar_texto_alerta(
        novo->estado_observado,
        sizeof(novo->estado_observado),
        estado_observado
    );

    novo->severidade = severidade;
    novo->atendimento = ALERTA_PENDENTE;

    novo->data_conclusao[0] = '\0';

    novo->proximo_lista = NULL;
    novo->proximo_fila = NULL;

    return novo;
}

int alerta_existe_duplicado_ativo(
    const Alerta *lista,
    const char *codigo_sensor,
    const char *estado_observado,
    int severidade
)
{
    const Alerta *atual;

    if (
        codigo_sensor == NULL ||
        estado_observado == NULL
    ) {
        return 0;
    }

    atual = lista;

    while (atual != NULL) {
        if (
            strcmp(
                atual->codigo_sensor,
                codigo_sensor
            ) == 0 &&
            strcmp(
                atual->estado_observado,
                estado_observado
            ) == 0 &&
            atual->severidade == severidade &&
            (
                atual->atendimento == ALERTA_PENDENTE ||
                atual->atendimento == ALERTA_EM_CURSO
            )
        ) {
            return 1;
        }

        atual = atual->proximo_lista;
    }

    return 0;
}

int alerta_registar(
    Alerta **lista,
    FilaAlertas *fila,
    int id,
    const char *codigo_sensor,
    const char *descricao,
    const char *estado_observado,
    int severidade
)
{
    Alerta *novo;

    if (lista == NULL || fila == NULL) {
        return 0;
    }

    if (
        alerta_existe_duplicado_ativo(
            *lista,
            codigo_sensor,
            estado_observado,
            severidade
        )
    ) {
        return -1;
    }

    novo = alerta_criar(
        id,
        codigo_sensor,
        descricao,
        estado_observado,
        severidade
    );

    if (novo == NULL) {
        return 0;
    }

    /*
     * Insere o alerta no início da lista geral.
     */
    novo->proximo_lista = *lista;
    *lista = novo;

    /*
     * Coloca o mesmo alerta no fim da fila FIFO.
     */
    fila_alertas_enfileirar(fila, novo);

    return 1;
}

Alerta *alerta_processar_proximo(
    FilaAlertas *fila
)
{
    Alerta *alerta;

    alerta = fila_alertas_desenfileirar(fila);

    if (alerta == NULL) {
        return NULL;
    }

    alerta->atendimento = ALERTA_EM_CURSO;

    return alerta;
}

Alerta *alerta_pesquisar_por_id(
    Alerta *lista,
    int id
)
{
    Alerta *atual;

    atual = lista;

    while (atual != NULL) {
        if (atual->id == id) {
            return atual;
        }

        atual = atual->proximo_lista;
    }

    return NULL;
}

int alerta_concluir(
    Alerta *lista,
    int id
)
{
    Alerta *alerta;

    alerta = alerta_pesquisar_por_id(lista, id);

    if (alerta == NULL) {
        return 0;
    }

    /*
     * Apenas um alerta em curso pode ser concluído.
     */
    if (alerta->atendimento != ALERTA_EM_CURSO) {
        return -1;
    }

    alerta->atendimento = ALERTA_CONCLUIDO;

    obter_data_hora_atual(
        alerta->data_conclusao,
        sizeof(alerta->data_conclusao)
    );

    return 1;
}

const char *alerta_estado_para_texto(
    EstadoAtendimento estado
)
{
    switch (estado) {
        case ALERTA_PENDENTE:
            return "Pendente";

        case ALERTA_EM_CURSO:
            return "Em Curso";

        case ALERTA_CONCLUIDO:
            return "Concluido";

        default:
            return "Desconhecido";
    }
}

void alertas_listar(const Alerta *lista)
{
    const Alerta *atual;

    if (lista == NULL) {
        printf("\nNao existem alertas registados.\n");
        return;
    }

    printf("\n");
    printf("============================================================\n");
    printf("                     LISTA DE ALERTAS\n");
    printf("============================================================\n");

    atual = lista;

    while (atual != NULL) {
        printf("ID:               %d\n", atual->id);
        printf("Criado em:        %s\n", atual->data_criacao);
        printf("Sensor:           %s\n", atual->codigo_sensor);
        printf("Descricao:        %s\n", atual->descricao);
        printf("Estado observado: %s\n", atual->estado_observado);
        printf("Severidade:       %d\n", atual->severidade);
        printf(
            "Atendimento:      %s\n",
            alerta_estado_para_texto(atual->atendimento)
        );

        if (
            atual->atendimento == ALERTA_CONCLUIDO &&
            atual->data_conclusao[0] != '\0'
        ) {
            printf(
                "Concluido em:     %s\n",
                atual->data_conclusao
            );
        }

        printf("------------------------------------------------------------\n");

        atual = atual->proximo_lista;       //percorre a lista geral
    }
}

void fila_alertas_listar(
    const FilaAlertas *fila
)
{
    const Alerta *atual;

    if (
        fila == NULL ||
        fila->inicio == NULL
    ) {
        printf("Nao existem alertas pendentes.\n");
        return;
    }

    printf("\n");
    printf("============================================================\n");
    printf("             FILA DE ALERTAS PENDENTES - FIFO\n");
    printf("============================================================\n");

    atual = fila->inicio;

    while (atual != NULL) {
        printf(
            "ID %d | Sensor %s | Severidade %d | %s\n",
            atual->id,
            atual->codigo_sensor,
            atual->severidade,
            alerta_estado_para_texto(
                atual->atendimento
            )
        );

        atual = atual->proximo_fila;    //avanca para o proximo elemento da lista ligada
    }
}

void alertas_listar_por_estado(
    const Alerta *lista,
    EstadoAtendimento estado
)
{
    const Alerta *atual;
    int encontrados;

    atual = lista;
    encontrados = 0;

    while (atual != NULL) {
        if (atual->atendimento == estado) {
            printf(
                "ID %d | Sensor %s | Severidade %d | %s\n",
                atual->id,
                atual->codigo_sensor,
                atual->severidade,
                alerta_estado_para_texto(
                    atual->atendimento
                )
            );

            encontrados++;
        }

        atual = atual->proximo_lista;
    }

    if (encontrados == 0) {
        printf(
            "Nao existem alertas no estado indicado.\n"
        );
    }
}

int alertas_contar_por_estado(
    const Alerta *lista,
    EstadoAtendimento estado
)
{
    const Alerta *atual;
    int quantidade;

    atual = lista;
    quantidade = 0;

    while (atual != NULL) {
        if (atual->atendimento == estado) {
            quantidade++;
        }

        atual = atual->proximo_lista;
    }

    return quantidade;
}

void alertas_libertar(
    Alerta **lista,
    FilaAlertas *fila
)
{
    Alerta *atual;
    Alerta *seguinte;

    if (lista == NULL) {
        return;
    }

    /*
     * A fila apenas contém referências para os mesmos alertas.
     * Não se usa free() através da fila.
     */
    if (fila != NULL) {
        fila->inicio = NULL;
        fila->fim = NULL;
    }

    atual = *lista;

    while (atual != NULL) {
        seguinte = atual->proximo_lista;
        free(atual);
        atual = seguinte;
    }

    *lista = NULL;
}