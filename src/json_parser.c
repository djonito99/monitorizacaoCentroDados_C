#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <string.h>

#include "cJSON.h"

#include "json_parser.h"

typedef enum {
    LEITURA_JSON_INVALIDA = 0,
    LEITURA_JSON_VALIDA = 1,
    LEITURA_JSON_ERRO_MEMORIA = -1
} ResultadoLeituraJson;

static cJSON *obter_campo(
    const cJSON *objeto,
    const char *nome
)
{
    if (
        objeto == NULL ||
        nome == NULL ||
        !cJSON_IsObject(objeto)
    ) {
        return NULL;
    }

    return cJSON_GetObjectItemCaseSensitive(
        objeto,
        nome
    );
}

static int obter_texto_obrigatorio(
    const cJSON *objeto,
    const char *nome,
    size_t tamanho_maximo,
    const char **texto
)
{
    cJSON *campo;
    size_t comprimento;

    if (
        objeto == NULL ||
        nome == NULL ||
        texto == NULL ||
        tamanho_maximo == 0
    ) {
        return 0;
    }

    *texto = NULL;

    campo = obter_campo(objeto, nome);

    if (
        !cJSON_IsString(campo) ||
        campo->valuestring == NULL
    ) {
        return 0;
    }

    comprimento = strlen(campo->valuestring);

    if (
        comprimento == 0 ||
        comprimento >= tamanho_maximo
    ) {
        return 0;
    }

    *texto = campo->valuestring;

    return 1;
}

static int obter_numero_finito(
    const cJSON *objeto,
    const char *nome,
    double *valor
)
{
    cJSON *campo;

    if (
        objeto == NULL ||
        nome == NULL ||
        valor == NULL
    ) {
        return 0;
    }

    campo = obter_campo(objeto, nome);

    if (!cJSON_IsNumber(campo)) {
        return 0;
    }

    if (!isfinite(campo->valuedouble)) {
        return 0;
    }

    *valor = campo->valuedouble;

    return 1;
}

static int obter_inteiro_nao_negativo(
    const cJSON *objeto,
    const char *nome,
    int *valor
)
{
    double numero;

    if (
        valor == NULL ||
        !obter_numero_finito(
            objeto,
            nome,
            &numero
        )
    ) {
        return 0;
    }

    if (
        numero < 0.0 ||
        numero > (double)INT_MAX
    ) {
        return 0;
    }

    /*
     * Impede valores como 1.5 para a severidade.
     */
    if (numero != (double)(int)numero) {
        return 0;
    }

    *valor = (int)numero;

    return 1;
}

static ResultadoLeituraJson interpretar_leitura(
    const cJSON *item,
    Sensor **leitura
)
{
    cJSON *objeto_sensor;
    cJSON *objeto_medicao;

    const char *data_hora;
    const char *codigo;
    const char *descricao;
    const char *tipo;
    const char *unidade;
    const char *estado;
    const char *mensagem;

    double valor;
    int severidade;

    Sensor *novo;

    if (
        item == NULL ||
        leitura == NULL ||
        !cJSON_IsObject(item)
    ) {
        return LEITURA_JSON_INVALIDA;
    }

    *leitura = NULL;

    objeto_sensor = obter_campo(
        item,
        "sensor"
    );

    objeto_medicao = obter_campo(
        item,
        "measurement"
    );

    if (
        !cJSON_IsObject(objeto_sensor) ||
        !cJSON_IsObject(objeto_medicao)
    ) {
        return LEITURA_JSON_INVALIDA;
    }

    if (
        !obter_texto_obrigatorio(
            item,
            "timestamp",
            TAM_DATA_HORA,
            &data_hora
        )
    ) {
        return LEITURA_JSON_INVALIDA;
    }

    if (
        !obter_texto_obrigatorio(
            objeto_sensor,
            "code",
            TAM_CODIGO,
            &codigo
        )
    ) {
        return LEITURA_JSON_INVALIDA;
    }

    if (
        !obter_texto_obrigatorio(
            objeto_sensor,
            "label",
            TAM_DESCRICAO,
            &descricao
        )
    ) {
        return LEITURA_JSON_INVALIDA;
    }

    if (
        !obter_texto_obrigatorio(
            objeto_sensor,
            "kind",
            TAM_TIPO,
            &tipo
        )
    ) {
        return LEITURA_JSON_INVALIDA;
    }

    if (
        !obter_numero_finito(
            objeto_medicao,
            "value",
            &valor
        )
    ) {
        return LEITURA_JSON_INVALIDA;
    }

    if (
        !obter_texto_obrigatorio(
            objeto_medicao,
            "unit",
            TAM_UNIDADE,
            &unidade
        )
    ) {
        return LEITURA_JSON_INVALIDA;
    }

    if (
        !obter_texto_obrigatorio(
            item,
            "status",
            TAM_ESTADO,
            &estado
        )
    ) {
        return LEITURA_JSON_INVALIDA;
    }

    if (
        !obter_inteiro_nao_negativo(
            item,
            "severity",
            &severidade
        )
    ) {
        return LEITURA_JSON_INVALIDA;
    }

    if (
        !obter_texto_obrigatorio(
            item,
            "message",
            TAM_MENSAGEM,
            &mensagem
        )
    ) {
        return LEITURA_JSON_INVALIDA;
    }

    novo = sensor_criar(
        data_hora,
        codigo,
        descricao,
        tipo,
        valor,
        unidade,
        estado,
        severidade,
        mensagem
    );

    if (novo == NULL) {
        return LEITURA_JSON_ERRO_MEMORIA;
    }

    *leitura = novo;

    return LEITURA_JSON_VALIDA;
}

int json_interpretar_leituras(
    const char *json,
    ResultadoJson *resultado
)
{
    cJSON *raiz;
    cJSON *readings;
    cJSON *item;

    const char *fim_interpretacao;

    ResultadoLeituraJson resultado_leitura;

    Sensor *leitura;
    Sensor *ultima;

    if (
        json == NULL ||
        resultado == NULL
    ) {
        return 0;
    }

    resultado->leituras = NULL;
    resultado->recebidas = 0;
    resultado->validas = 0;
    resultado->rejeitadas = 0;

    fim_interpretacao = NULL;

    /*
     * O terceiro argumento igual a 1 exige que não existam
     * caracteres adicionais depois do JSON válido.
     */
    raiz = cJSON_ParseWithOpts(
        json,
        &fim_interpretacao,
        1
    );

    if (raiz == NULL) {
        return 0;
    }

    if (!cJSON_IsObject(raiz)) {
        cJSON_Delete(raiz);
        return 0;
    }

    readings = obter_campo(
        raiz,
        "readings"
    );

    if (!cJSON_IsArray(readings)) {
        cJSON_Delete(raiz);
        return 0;
    }

    ultima = NULL;

    cJSON_ArrayForEach(item, readings) {
        resultado->recebidas++;

        leitura = NULL;

        resultado_leitura = interpretar_leitura(
            item,
            &leitura
        );

        if (
            resultado_leitura ==
            LEITURA_JSON_INVALIDA
        ) {
            resultado->rejeitadas++;
            continue;
        }

        if (
            resultado_leitura ==
            LEITURA_JSON_ERRO_MEMORIA
        ) {
            sensores_libertar(
                &resultado->leituras
            );

            cJSON_Delete(raiz);

            resultado->validas = 0;

            return 0;
        }

        /*
         * Inserção no fim para manter a ordem recebida.
         */
        if (resultado->leituras == NULL) {
            resultado->leituras = leitura;
        } else {
            ultima->proximo = leitura;
        }

        ultima = leitura;

        resultado->validas++;
    }

    cJSON_Delete(raiz);

    /*
     * Uma resposta sem leituras válidas não é aceite
     * como sincronização válida.
     */
    if (resultado->validas == 0) {
        sensores_libertar(
            &resultado->leituras
        );

        return 0;
    }

    return 1;
}

void json_resultado_libertar(
    ResultadoJson *resultado
)
{
    if (resultado == NULL) {
        return;
    }

    sensores_libertar(
        &resultado->leituras
    );

    resultado->recebidas = 0;
    resultado->validas = 0;
    resultado->rejeitadas = 0;
}