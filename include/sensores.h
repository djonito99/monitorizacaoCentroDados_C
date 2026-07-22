#ifndef SENSORES_H
#define SENSORES_H

#define TAM_DATA_HORA 32
#define TAM_CODIGO 32
#define TAM_DESCRICAO 128
#define TAM_TIPO 32
#define TAM_UNIDADE 16
#define TAM_ESTADO 32
#define TAM_MENSAGEM 256

typedef struct Sensor {
    char data_hora[TAM_DATA_HORA];
    char codigo[TAM_CODIGO];
    char descricao[TAM_DESCRICAO];
    char tipo[TAM_TIPO];

    double valor;

    char unidade[TAM_UNIDADE];
    char estado[TAM_ESTADO];

    int severidade;

    char mensagem[TAM_MENSAGEM];

    struct Sensor *proximo;     //guarda o endereco do proximo sensor.
} Sensor;

/*
 * Resultado de uma operacao de insercao ou atualizacao.
 */
typedef enum {
    SENSOR_ERRO = -1,
    SENSOR_MANTIDO = 0,
    SENSOR_INSERIDO = 1,
    SENSOR_ATUALIZADO = 2
} ResultadoSensor;

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
);

Sensor *sensor_pesquisar_por_codigo(
    Sensor *lista,
    const char *codigo
);

ResultadoSensor sensor_inserir_ou_atualizar(
    Sensor **lista,
    const Sensor *leitura
);

void sensores_listar(const Sensor *lista);

void sensores_filtrar_por_tipo(
    const Sensor *lista,
    const char *tipo
);

void sensores_filtrar_por_estado(
    const Sensor *lista,
    const char *estado
);

int sensores_contar(const Sensor *lista);

void sensores_libertar(Sensor **lista);

#endif