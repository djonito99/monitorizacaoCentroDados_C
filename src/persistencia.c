#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "persistencia.h"

/*
 * Estruturas utilizadas exclusivamente nos ficheiros binários.
 * Não contêm ponteiros.
 */

typedef struct {
    char data_hora[TAM_DATA_HORA];
    char codigo[TAM_CODIGO];
    char descricao[TAM_DESCRICAO];
    char tipo[TAM_TIPO];

    double valor;

    char unidade[TAM_UNIDADE];
    char estado[TAM_ESTADO];

    int severidade;

    char mensagem[TAM_MENSAGEM];
} SensorDisco;

typedef struct {
    int id;

    char data_criacao[TAM_DATA_ALERTA];
    char codigo_sensor[TAM_CODIGO];
    char descricao[TAM_DESCRICAO_ALERTA];
    char estado_observado[TAM_ESTADO];

    int severidade;
    int atendimento;

    char data_conclusao[TAM_DATA_ALERTA];
} AlertaDisco;

typedef struct {
    char data_hora[TAM_DATA_HISTORICO];
    int resultado;

    int leituras_recebidas;
    int sensores_inseridos;
    int sensores_atualizados;
    int sensores_mantidos;
    int registos_rejeitados;
    int alertas_gerados;
} SincronizacaoDisco;

/*
 * Cabeçalho comum aos ficheiros binários.
 */
typedef struct {
    char assinatura[8];
    unsigned int versao;
    unsigned int quantidade;
} CabecalhoBinario;

#define VERSAO_FICHEIROS 1U

#define ASSINATURA_SENSORES "SNSR001"
#define ASSINATURA_ALERTAS "ALRT001"
#define ASSINATURA_HISTORICO "HIST001"

/*
 * Copia texto sem ultrapassar a dimensão do destino.
 */
static void copiar_texto_persistencia(
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

    snprintf(
        destino,
        tamanho_destino,
        "%s",
        origem
    );
}

static int cabecalho_valido(
    const CabecalhoBinario *cabecalho,
    const char *assinatura
)
{
    if (cabecalho == NULL || assinatura == NULL) {
        return 0;
    }

    if (
        strcmp(
            cabecalho->assinatura,
            assinatura
        ) != 0
    ) {
        return 0;
    }

    if (cabecalho->versao != VERSAO_FICHEIROS) {
        return 0;
    }

    return 1;
}

static unsigned int contar_sensores_unsigned(
    const Sensor *lista
)
{
    unsigned int quantidade;

    quantidade = 0;

    while (lista != NULL) {
        quantidade++;
        lista = lista->proximo;
    }

    return quantidade;
}

static unsigned int contar_alertas_unsigned(
    const Alerta *lista
)
{
    unsigned int quantidade;

    quantidade = 0;

    while (lista != NULL) {
        quantidade++;
        lista = lista->proximo_lista;
    }

    return quantidade;
}

static unsigned int contar_historico_unsigned(
    const Sincronizacao *topo
)
{
    unsigned int quantidade;

    quantidade = 0;

    while (topo != NULL) {
        quantidade++;
        topo = topo->proximo;
    }

    return quantidade;
}

int persistencia_guardar_sensores(
    const Sensor *sensores
)
{
    FILE *ficheiro;
    CabecalhoBinario cabecalho;
    SensorDisco registo;
    const Sensor *atual;

    ficheiro = fopen(
        FICHEIRO_SENSORES,
        "wb"
    );

    if (ficheiro == NULL) {
        return 0;
    }

    memset(&cabecalho, 0, sizeof(cabecalho));

    copiar_texto_persistencia(
        cabecalho.assinatura,
        sizeof(cabecalho.assinatura),
        ASSINATURA_SENSORES
    );

    cabecalho.versao = VERSAO_FICHEIROS;
    cabecalho.quantidade =
        contar_sensores_unsigned(sensores);

    if (
        fwrite(
            &cabecalho,
            sizeof(cabecalho),
            1,
            ficheiro
        ) != 1
    ) {
        fclose(ficheiro);
        return 0;
    }

    atual = sensores;

    while (atual != NULL) {
        memset(&registo, 0, sizeof(registo));

        copiar_texto_persistencia(
            registo.data_hora,
            sizeof(registo.data_hora),
            atual->data_hora
        );

        copiar_texto_persistencia(
            registo.codigo,
            sizeof(registo.codigo),
            atual->codigo
        );

        copiar_texto_persistencia(
            registo.descricao,
            sizeof(registo.descricao),
            atual->descricao
        );

        copiar_texto_persistencia(
            registo.tipo,
            sizeof(registo.tipo),
            atual->tipo
        );

        registo.valor = atual->valor;

        copiar_texto_persistencia(
            registo.unidade,
            sizeof(registo.unidade),
            atual->unidade
        );

        copiar_texto_persistencia(
            registo.estado,
            sizeof(registo.estado),
            atual->estado
        );

        registo.severidade = atual->severidade;

        copiar_texto_persistencia(
            registo.mensagem,
            sizeof(registo.mensagem),
            atual->mensagem
        );

        if (
            fwrite(
                &registo,
                sizeof(registo),
                1,
                ficheiro
            ) != 1
        ) {
            fclose(ficheiro);
            return 0;
        }

        atual = atual->proximo;
    }

    if (fclose(ficheiro) != 0) {
        return 0;
    }

    return 1;
}

int persistencia_carregar_sensores(
    Sensor **sensores
)
{
    FILE *ficheiro;
    CabecalhoBinario cabecalho;
    SensorDisco registo;

    Sensor *novo;
    Sensor *ultimo;

    unsigned int i;

    if (sensores == NULL) {
        return 0;
    }

    ficheiro = fopen(
        FICHEIRO_SENSORES,
        "rb"
    );

    /*
     * O ficheiro ainda não existir não é um erro fatal.
     * Significa apenas que ainda não existem dados guardados.
     */
    if (ficheiro == NULL) {
        return 1;
    }

    if (
        fread(
            &cabecalho,
            sizeof(cabecalho),
            1,
            ficheiro
        ) != 1
    ) {
        fclose(ficheiro);
        return 0;
    }

    if (
        !cabecalho_valido(
            &cabecalho,
            ASSINATURA_SENSORES
        )
    ) {
        fclose(ficheiro);
        return 0;
    }

    ultimo = NULL;

    for (i = 0; i < cabecalho.quantidade; i++) {
        if (
            fread(
                &registo,
                sizeof(registo),
                1,
                ficheiro
            ) != 1
        ) {
            sensores_libertar(sensores);
            fclose(ficheiro);
            return 0;
        }

        novo = sensor_criar(
            registo.data_hora,
            registo.codigo,
            registo.descricao,
            registo.tipo,
            registo.valor,
            registo.unidade,
            registo.estado,
            registo.severidade,
            registo.mensagem
        );

        if (novo == NULL) {
            sensores_libertar(sensores);
            fclose(ficheiro);
            return 0;
        }

        /*
         * Inserção no fim para preservar a ordem do ficheiro.
         */
        if (*sensores == NULL) {
            *sensores = novo;
        } else {
            ultimo->proximo = novo;
        }

        ultimo = novo;
    }

    fclose(ficheiro);
    return 1;
}

int persistencia_guardar_alertas(
    const Alerta *alertas
)
{
    FILE *ficheiro;
    CabecalhoBinario cabecalho;
    AlertaDisco registo;
    const Alerta *atual;

    ficheiro = fopen(
        FICHEIRO_ALERTAS,
        "wb"
    );

    if (ficheiro == NULL) {
        return 0;
    }

    memset(&cabecalho, 0, sizeof(cabecalho));

    copiar_texto_persistencia(
        cabecalho.assinatura,
        sizeof(cabecalho.assinatura),
        ASSINATURA_ALERTAS
    );

    cabecalho.versao = VERSAO_FICHEIROS;
    cabecalho.quantidade =
        contar_alertas_unsigned(alertas);

    if (
        fwrite(
            &cabecalho,
            sizeof(cabecalho),
            1,
            ficheiro
        ) != 1
    ) {
        fclose(ficheiro);
        return 0;
    }

    atual = alertas;

    while (atual != NULL) {
        memset(&registo, 0, sizeof(registo));

        registo.id = atual->id;

        copiar_texto_persistencia(
            registo.data_criacao,
            sizeof(registo.data_criacao),
            atual->data_criacao
        );

        copiar_texto_persistencia(
            registo.codigo_sensor,
            sizeof(registo.codigo_sensor),
            atual->codigo_sensor
        );

        copiar_texto_persistencia(
            registo.descricao,
            sizeof(registo.descricao),
            atual->descricao
        );

        copiar_texto_persistencia(
            registo.estado_observado,
            sizeof(registo.estado_observado),
            atual->estado_observado
        );

        registo.severidade = atual->severidade;
        registo.atendimento = (int)atual->atendimento;

        copiar_texto_persistencia(
            registo.data_conclusao,
            sizeof(registo.data_conclusao),
            atual->data_conclusao
        );

        if (
            fwrite(
                &registo,
                sizeof(registo),
                1,
                ficheiro
            ) != 1
        ) {
            fclose(ficheiro);
            return 0;
        }

        atual = atual->proximo_lista;
    }

    if (fclose(ficheiro) != 0) {
        return 0;
    }

    return 1;
}

static int reconstruir_fila_alertas_pendentes(
    Alerta *lista,
    FilaAlertas *fila
)
{
    Alerta **pendentes;
    Alerta *atual;

    int quantidade;
    int indice;

    if (fila == NULL) {
        return 0;
    }

    fila_alertas_inicializar(fila);

    quantidade = alertas_contar_por_estado(
        lista,
        ALERTA_PENDENTE
    );

    if (quantidade == 0) {
        return 1;
    }

    pendentes = malloc(
        (size_t)quantidade * sizeof(Alerta *)
    );

    if (pendentes == NULL) {
        return 0;
    }

    atual = lista;
    indice = 0;

    while (atual != NULL) {
        if (
            atual->atendimento ==
            ALERTA_PENDENTE
        ) {
            pendentes[indice] = atual;
            indice++;
        }

        atual = atual->proximo_lista;
    }

    /*
     * A lista geral tem os alertas mais recentes primeiro.
     * A fila deve ficar dos mais antigos para os mais recentes.
     */
    for (indice = quantidade - 1; indice >= 0; indice--) {
        fila_alertas_enfileirar(
            fila,
            pendentes[indice]
        );
    }

    free(pendentes);
    return 1;
}

int persistencia_carregar_alertas(
    Alerta **alertas,
    FilaAlertas *fila,
    int *proximo_id
)
{
    FILE *ficheiro;
    CabecalhoBinario cabecalho;
    AlertaDisco registo;

    Alerta *novo;
    Alerta *ultimo;

    unsigned int i;
    int maior_id;

    if (
        alertas == NULL ||
        fila == NULL ||
        proximo_id == NULL
    ) {
        return 0;
    }

    ficheiro = fopen(
        FICHEIRO_ALERTAS,
        "rb"
    );

    if (ficheiro == NULL) {
        return 1;
    }

    if (
        fread(
            &cabecalho,
            sizeof(cabecalho),
            1,
            ficheiro
        ) != 1
    ) {
        fclose(ficheiro);
        return 0;
    }

    if (
        !cabecalho_valido(
            &cabecalho,
            ASSINATURA_ALERTAS
        )
    ) {
        fclose(ficheiro);
        return 0;
    }

    ultimo = NULL;
    maior_id = 0;

    for (i = 0; i < cabecalho.quantidade; i++) {
        if (
            fread(
                &registo,
                sizeof(registo),
                1,
                ficheiro
            ) != 1
        ) {
            alertas_libertar(alertas, fila);
            fclose(ficheiro);
            return 0;
        }

        novo = malloc(sizeof(Alerta));

        if (novo == NULL) {
            alertas_libertar(alertas, fila);
            fclose(ficheiro);
            return 0;
        }

        memset(novo, 0, sizeof(Alerta));

        novo->id = registo.id;

        copiar_texto_persistencia(
            novo->data_criacao,
            sizeof(novo->data_criacao),
            registo.data_criacao
        );

        copiar_texto_persistencia(
            novo->codigo_sensor,
            sizeof(novo->codigo_sensor),
            registo.codigo_sensor
        );

        copiar_texto_persistencia(
            novo->descricao,
            sizeof(novo->descricao),
            registo.descricao
        );

        copiar_texto_persistencia(
            novo->estado_observado,
            sizeof(novo->estado_observado),
            registo.estado_observado
        );

        novo->severidade = registo.severidade;

        if (
            registo.atendimento < ALERTA_PENDENTE ||
            registo.atendimento > ALERTA_CONCLUIDO
        ) {
            free(novo);
            alertas_libertar(alertas, fila);
            fclose(ficheiro);
            return 0;
        }

        novo->atendimento =
            (EstadoAtendimento)registo.atendimento;

        copiar_texto_persistencia(
            novo->data_conclusao,
            sizeof(novo->data_conclusao),
            registo.data_conclusao
        );

        novo->proximo_lista = NULL;
        novo->proximo_fila = NULL;

        if (*alertas == NULL) {
            *alertas = novo;
        } else {
            ultimo->proximo_lista = novo;
        }

        ultimo = novo;

        if (novo->id > maior_id) {
            maior_id = novo->id;
        }
    }

    fclose(ficheiro);

    if (
        !reconstruir_fila_alertas_pendentes(
            *alertas,
            fila
        )
    ) {
        alertas_libertar(alertas, fila);
        return 0;
    }

    *proximo_id = maior_id + 1;

    if (*proximo_id < 1) {
        *proximo_id = 1;
    }

    return 1;
}

int persistencia_guardar_historico(
    const Sincronizacao *historico
)
{
    FILE *ficheiro;
    CabecalhoBinario cabecalho;
    SincronizacaoDisco registo;
    const Sincronizacao *atual;

    ficheiro = fopen(
        FICHEIRO_HISTORICO,
        "wb"
    );

    if (ficheiro == NULL) {
        return 0;
    }

    memset(&cabecalho, 0, sizeof(cabecalho));

    copiar_texto_persistencia(
        cabecalho.assinatura,
        sizeof(cabecalho.assinatura),
        ASSINATURA_HISTORICO
    );

    cabecalho.versao = VERSAO_FICHEIROS;
    cabecalho.quantidade =
        contar_historico_unsigned(historico);

    if (
        fwrite(
            &cabecalho,
            sizeof(cabecalho),
            1,
            ficheiro
        ) != 1
    ) {
        fclose(ficheiro);
        return 0;
    }

    atual = historico;

    while (atual != NULL) {
        memset(&registo, 0, sizeof(registo));

        copiar_texto_persistencia(
            registo.data_hora,
            sizeof(registo.data_hora),
            atual->data_hora
        );

        registo.resultado = (int)atual->resultado;
        registo.leituras_recebidas =
            atual->leituras_recebidas;
        registo.sensores_inseridos =
            atual->sensores_inseridos;
        registo.sensores_atualizados =
            atual->sensores_atualizados;
        registo.sensores_mantidos =
            atual->sensores_mantidos;
        registo.registos_rejeitados =
            atual->registos_rejeitados;
        registo.alertas_gerados =
            atual->alertas_gerados;

        if (
            fwrite(
                &registo,
                sizeof(registo),
                1,
                ficheiro
            ) != 1
        ) {
            fclose(ficheiro);
            return 0;
        }

        atual = atual->proximo;
    }

    if (fclose(ficheiro) != 0) {
        return 0;
    }

    return 1;
}

int persistencia_carregar_historico(
    Sincronizacao **historico
)
{
    FILE *ficheiro;
    CabecalhoBinario cabecalho;
    SincronizacaoDisco registo;

    Sincronizacao *novo;
    Sincronizacao *ultimo;

    unsigned int i;

    if (historico == NULL) {
        return 0;
    }

    ficheiro = fopen(
        FICHEIRO_HISTORICO,
        "rb"
    );

    if (ficheiro == NULL) {
        return 1;
    }

    if (
        fread(
            &cabecalho,
            sizeof(cabecalho),
            1,
            ficheiro
        ) != 1
    ) {
        fclose(ficheiro);
        return 0;
    }

    if (
        !cabecalho_valido(
            &cabecalho,
            ASSINATURA_HISTORICO
        )
    ) {
        fclose(ficheiro);
        return 0;
    }

    ultimo = NULL;

    for (i = 0; i < cabecalho.quantidade; i++) {
        if (
            fread(
                &registo,
                sizeof(registo),
                1,
                ficheiro
            ) != 1
        ) {
            historico_libertar(historico);
            fclose(ficheiro);
            return 0;
        }

        novo = malloc(sizeof(Sincronizacao));

        if (novo == NULL) {
            historico_libertar(historico);
            fclose(ficheiro);
            return 0;
        }

        memset(novo, 0, sizeof(Sincronizacao));

        copiar_texto_persistencia(
            novo->data_hora,
            sizeof(novo->data_hora),
            registo.data_hora
        );

        if (
            registo.resultado != SINCRONIZACAO_FALHA &&
            registo.resultado != SINCRONIZACAO_SUCESSO
        ) {
            free(novo);
            historico_libertar(historico);
            fclose(ficheiro);
            return 0;
        }

        novo->resultado =
            (ResultadoSincronizacao)registo.resultado;

        novo->leituras_recebidas =
            registo.leituras_recebidas;
        novo->sensores_inseridos =
            registo.sensores_inseridos;
        novo->sensores_atualizados =
            registo.sensores_atualizados;
        novo->sensores_mantidos =
            registo.sensores_mantidos;
        novo->registos_rejeitados =
            registo.registos_rejeitados;
        novo->alertas_gerados =
            registo.alertas_gerados;

        novo->proximo = NULL;

        /*
         * Preserva a ordem:
         * o primeiro registo do ficheiro continua no topo.
         */
        if (*historico == NULL) {
            *historico = novo;
        } else {
            ultimo->proximo = novo;
        }

        ultimo = novo;
    }

    fclose(ficheiro);
    return 1;
}

int persistencia_guardar_tudo(
    const EstadoAplicacao *aplicacao
)
{
    int sucesso;

    if (aplicacao == NULL) {
        return 0;
    }

    sucesso = 1;

    if (
        !persistencia_guardar_sensores(
            aplicacao->sensores
        )
    ) {
        sucesso = 0;
    }

    if (
        !persistencia_guardar_alertas(
            aplicacao->alertas
        )
    ) {
        sucesso = 0;
    }

    if (
        !persistencia_guardar_historico(
            aplicacao->historico
        )
    ) {
        sucesso = 0;
    }

    return sucesso;
}

int persistencia_carregar_tudo(
    EstadoAplicacao *aplicacao
)
{
    if (aplicacao == NULL) {
        return 0;
    }

    if (
        !persistencia_carregar_sensores(
            &aplicacao->sensores
        )
    ) {
        return 0;
    }

    if (
        !persistencia_carregar_alertas(
            &aplicacao->alertas,
            &aplicacao->fila_alertas_pendentes,
            &aplicacao->proximo_id_alerta
        )
    ) {
        sensores_libertar(&aplicacao->sensores);
        return 0;
    }

    if (
        !persistencia_carregar_historico(
            &aplicacao->historico
        )
    ) {
        sensores_libertar(&aplicacao->sensores);

        alertas_libertar(
            &aplicacao->alertas,
            &aplicacao->fila_alertas_pendentes
        );

        return 0;
    }

    return 1;
}

