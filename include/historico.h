#ifndef HISTORICO_H
#define HISTORICO_H

#define TAM_DATA_HISTORICO 32

typedef enum {
    SINCRONIZACAO_FALHA = 0,
    SINCRONIZACAO_SUCESSO = 1
} ResultadoSincronizacao;

typedef struct Sincronizacao {
    char data_hora[TAM_DATA_HISTORICO];
    ResultadoSincronizacao resultado;

    int leituras_recebidas;
    int sensores_inseridos;
    int sensores_atualizados;
    int sensores_mantidos;
    int registos_rejeitados;
    int alertas_gerados;

    struct Sincronizacao *proximo;
} Sincronizacao;

Sincronizacao *historico_criar_registo(
    ResultadoSincronizacao resultado,
    int leituras_recebidas,
    int sensores_inseridos,
    int sensores_atualizados,
    int sensores_mantidos,
    int registos_rejeitados,
    int alertas_gerados
);

int historico_empilhar(
    Sincronizacao **topo,
    Sincronizacao *registo
);

const Sincronizacao *historico_consultar_ultima(
    const Sincronizacao *topo
);

void historico_listar_ultimas(
    const Sincronizacao *topo,
    int quantidade
);

int historico_contar(
    const Sincronizacao *topo
);

const char *historico_resultado_para_texto(
    ResultadoSincronizacao resultado
);

void historico_libertar(
    Sincronizacao **topo
);

#endif