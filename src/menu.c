#include <stdio.h>
#include <string.h>

#include "api.h"
#include "json_parser.h"
#include "menu.h"
#include "sensores.h"
#include "menu_alertas.h"
#include "estado.h"         //redundancia mas por precaucao
#include "menu_historico.h"
#include "ordenacao.h"

static void limpar_buffer_entrada(void)
{
    int caractere;

    do {
        caractere = getchar();
    } while (caractere != '\n' && caractere != EOF);
}

static void ler_texto(
    const char *mensagem,
    char *destino,
    size_t tamanho
)
{
    size_t comprimento;

    if (mensagem != NULL) {
        printf("%s", mensagem);
    }

    if (fgets(destino, (int)tamanho, stdin) == NULL) {
        destino[0] = '\0';
        return;
    }

    comprimento = strlen(destino);

    if (
        comprimento > 0 &&
        destino[comprimento - 1] == '\n'
    ) {
        destino[comprimento - 1] = '\0';
    }
}

static void adicionar_sensor_teste(
    EstadoAplicacao *aplicacao
)
{
    Sensor leitura;
    ResultadoSensor resultado;

    memset(&leitura, 0, sizeof(leitura));

    snprintf(
        leitura.data_hora,
        sizeof(leitura.data_hora),
        "%s",
        "2026-07-22 15:00:00"
    );

    snprintf(
        leitura.codigo,
        sizeof(leitura.codigo),
        "%s",
        "TEMP-001"
    );

    snprintf(
        leitura.descricao,
        sizeof(leitura.descricao),
        "%s",
        "Temperatura da sala tecnica"
    );

    snprintf(
        leitura.tipo,
        sizeof(leitura.tipo),
        "%s",
        "TEMPERATURA"
    );

    leitura.valor = 24.5;

    snprintf(
        leitura.unidade,
        sizeof(leitura.unidade),
        "%s",
        "C"
    );

    snprintf(
        leitura.estado,
        sizeof(leitura.estado),
        "%s",
        "NORMAL"
    );

    leitura.severidade = 0;

    snprintf(
        leitura.mensagem,
        sizeof(leitura.mensagem),
        "%s",
        "Temperatura dentro dos limites"
    );

    leitura.proximo = NULL;

    resultado = sensor_inserir_ou_atualizar(
        &aplicacao->sensores,
        &leitura
    );

    if (resultado == SENSOR_INSERIDO) {
        printf("Sensor de teste inserido.\n");
    } else if (resultado == SENSOR_ATUALIZADO) {
        printf("Sensor de teste atualizado.\n");
    } else if (resultado == SENSOR_MANTIDO) {
        printf("Sensor mantido sem alteracoes relevantes.\n");
    } else {
        printf("Erro ao inserir o sensor.\n");
    }
}

static void pesquisar_sensor(
    EstadoAplicacao *aplicacao
)
{
    char codigo[TAM_CODIGO];
    Sensor *sensor;

    ler_texto(
        "Codigo do sensor: ",
        codigo,
        sizeof(codigo)
    );

    sensor = sensor_pesquisar_por_codigo(
        aplicacao->sensores,
        codigo
    );

    if (sensor == NULL) {
        printf("Sensor nao encontrado.\n");
        return;
    }

    printf("\nSensor encontrado:\n");
    printf("Codigo: %s\n", sensor->codigo);
    printf("Descricao: %s\n", sensor->descricao);
    printf("Valor: %.2f %s\n",
           sensor->valor,
           sensor->unidade);
    printf("Estado: %s\n", sensor->estado);
    printf("Severidade: %d\n", sensor->severidade);
}

static void filtrar_sensores_por_tipo(
    const EstadoAplicacao *aplicacao
)
{
    char tipo[TAM_TIPO];

    ler_texto(
        "Tipo do sensor: ",
        tipo,
        sizeof(tipo)
    );

    if (tipo[0] == '\0') {
        printf("O tipo nao pode estar vazio.\n");
        return;
    }

    sensores_filtrar_por_tipo(
        aplicacao->sensores,
        tipo
    );
}

static void filtrar_sensores_por_estado(
    const EstadoAplicacao *aplicacao
)
{
    char estado[TAM_ESTADO];

    ler_texto(
        "Estado do sensor: ",
        estado,
        sizeof(estado)
    );

    if (estado[0] == '\0') {
        printf("O estado nao pode estar vazio.\n");
        return;
    }

    sensores_filtrar_por_estado(
        aplicacao->sensores,
        estado
    );
}

/*testar o modulo da API
static void testar_comunicacao_api(void)
{
    RespostaApi resposta;

    api_resposta_inicializar(&resposta);

    printf("\nA comunicar com a API...\n");

    if (!api_obter_resposta(&resposta)) {
        printf(
            "Falha ao obter a resposta da API.\n"
        );
        return;
    }

    printf(
        "Resposta recebida com sucesso: %lu bytes.\n",  //especificador para size_t reconhecido por GCC/MinGW
        (unsigned long)resposta.tamanho
    );

    printf("\nInicio da resposta:\n");
    printf("%.500s\n", resposta.conteudo);

    api_libertar_resposta(&resposta);
}   */

static void testar_api_e_json(void)
{
    RespostaApi resposta;
    ResultadoJson resultado;

    api_resposta_inicializar(&resposta);

    printf("\nA obter dados da API...\n");

    if (!api_obter_resposta(&resposta)) {
        printf(
            "Falha de comunicacao com a API.\n"
        );
        return;
    }

    printf(
        "Resposta recebida: %lu bytes.\n",      //especificador para size_t reconhecido por GCC/MinGW
        (unsigned long)resposta.tamanho 
    );

    if (
        !json_interpretar_leituras(
            resposta.conteudo,
            &resultado
        )
    ) {
        printf(
            "A resposta foi recebida, mas o JSON "
            "nao contem leituras validas.\n"
        );

        api_libertar_resposta(&resposta);
        return;
    }

    printf("\nResumo do processamento:\n");
    printf(
        "Leituras recebidas:  %d\n",
        resultado.recebidas
    );
    printf(
        "Leituras validas:    %d\n",
        resultado.validas
    );
    printf(
        "Leituras rejeitadas: %d\n",
        resultado.rejeitadas
    );

    sensores_listar(
        resultado.leituras
    );

    json_resultado_libertar(
        &resultado
    );

    api_libertar_resposta(
        &resposta
    );
}

//menu de execucao da aplicacao
void menu_executar(EstadoAplicacao *aplicacao)
{
    int opcao;
    int leitura_valida;

    if (aplicacao == NULL) {
        return;
    }

    do {
        printf("\n--- Menu principal ---\n");
        printf("1. Inserir sensor de teste\n");
        printf("2. Listar sensores\n");
        printf("3. Pesquisar sensor por codigo\n");
        printf("4. Mostrar numero total de sensores\n");
        printf("5. Gerir alertas\n");
        printf("6. Consultar historico\n");
        printf("7. Listar sensores ordenados\n");
        printf("8. Gerir alertas\n");
        printf("9. Consultar historico\n");
        printf("10. Testar API e processamento JSON\n");   //teste temporario da API
        printf("0. Sair\n");

        printf("\nOpcao: ");

        leitura_valida = scanf("%d", &opcao);

        if (leitura_valida != 1) {
            printf("Entrada invalida.\n");
            limpar_buffer_entrada();
            opcao = -1;
            continue;
        }

        limpar_buffer_entrada();

        switch (opcao) {
            case 1:
                adicionar_sensor_teste(aplicacao);
                break;

            case 2:
                sensores_listar(aplicacao->sensores);
                break;

            case 3:
                pesquisar_sensor(aplicacao);
                break;

            case 4:
                printf(
                    "Numero total de sensores: %d\n",
                    sensores_contar(aplicacao->sensores)
                );            
                break;

            case 5:
                filtrar_sensores_por_tipo(aplicacao);
                break;

            case 6:
                filtrar_sensores_por_estado(aplicacao);
                break;

            case 7:
                sensores_listar_ordenados(
                    aplicacao->sensores
                );
                break;

            case 8:
                menu_alertas_executar(aplicacao);
                break;

            case 9:
                menu_historico_executar(aplicacao);
                break;

                case 10:
                    testar_api_e_json();
                    break;

            case 0:
                printf("A terminar a aplicacao.\n");
                break;

            default:
                printf("Opcao invalida.\n");
                break;
        }

    } while (opcao != 0);
}