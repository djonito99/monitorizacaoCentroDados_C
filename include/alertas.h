#ifndef ALERTAS_H
#define ALERTAS_H

#include "sensores.h"

#define TAM_DATA_ALERTA 32
#define TAM_DESCRICAO_ALERTA 128

typedef enum {
    ALERTA_PENDENTE = 0,
    ALERTA_EM_CURSO = 1,
    ALERTA_CONCLUIDO = 2
} EstadoAtendimento;

typedef struct Alerta {
    int id;

    char data_criacao[TAM_DATA_ALERTA];
    char codigo_sensor[TAM_CODIGO];
    char descricao[TAM_DESCRICAO_ALERTA];
    char estado_observado[TAM_ESTADO];

    int severidade;

    EstadoAtendimento atendimento;

    char data_conclusao[TAM_DATA_ALERTA];

    /*
     * Ligação da lista geral de alertas.
     */
    struct Alerta *proximo_lista;

    /*
     * Ligação utilizada apenas pela fila de pendentes.
     */
    struct Alerta *proximo_fila;
} Alerta;

typedef struct {
    Alerta *inicio;
    Alerta *fim;
} FilaAlertas;

void fila_alertas_inicializar(FilaAlertas *fila);

int fila_alertas_esta_vazia(const FilaAlertas *fila);

void fila_alertas_enfileirar(
    FilaAlertas *fila,
    Alerta *alerta
);

Alerta *fila_alertas_desenfileirar(
    FilaAlertas *fila
);

Alerta *alerta_criar(
    int id,
    const char *codigo_sensor,
    const char *descricao,
    const char *estado_observado,
    int severidade
);

int alerta_existe_duplicado_ativo(
    const Alerta *lista,
    const char *codigo_sensor,
    const char *estado_observado,
    int severidade
);

int alerta_registar(
    Alerta **lista,
    FilaAlertas *fila,
    int id,
    const char *codigo_sensor,
    const char *descricao,
    const char *estado_observado,
    int severidade
);

Alerta *alerta_processar_proximo(
    FilaAlertas *fila
);

Alerta *alerta_pesquisar_por_id(
    Alerta *lista,
    int id
);

int alerta_concluir(
    Alerta *lista,
    int id
);

void alertas_listar(
    const Alerta *lista
);

void fila_alertas_listar(       //funcao para listar a fila FIFO
    const FilaAlertas *fila
);

void alertas_listar_por_estado(
    const Alerta *lista,
    EstadoAtendimento estado
);

int alertas_contar_por_estado(
    const Alerta *lista,
    EstadoAtendimento estado
);

const char *alerta_estado_para_texto(
    EstadoAtendimento estado
);

void alertas_libertar(
    Alerta **lista,
    FilaAlertas *fila
);

#endif