#include <stdio.h>
#include <string.h>

#include "menu_alertas.h"
#include "alertas.h"

static void limpar_buffer_alertas(void)
{
    int caractere;

    do {
        caractere = getchar();
    } while (
        caractere != '\n' &&
        caractere != EOF
    );
}

static void ler_texto_alertas(
    const char *mensagem,
    char *destino,
    size_t tamanho
)
{
    size_t comprimento;

    if (mensagem != NULL) {
        printf("%s", mensagem);
    }

    if (
        fgets(
            destino,
            (int)tamanho,
            stdin
        ) == NULL
    ) {
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

static int ler_inteiro_alertas(
    const char *mensagem,
    int *valor
)
{
    int resultado;

    printf("%s", mensagem);

    resultado = scanf("%d", valor);

    if (resultado != 1) {
        limpar_buffer_alertas();
        return 0;
    }

    limpar_buffer_alertas();
    return 1;
}

static void registar_alerta_manual(
    EstadoAplicacao *aplicacao
)
{
    char codigo[TAM_CODIGO];
    char descricao[TAM_DESCRICAO_ALERTA];
    char estado[TAM_ESTADO];

    int severidade;
    int resultado;
    int id;

    ler_texto_alertas(
        "Codigo do sensor: ",
        codigo,
        sizeof(codigo)
    );

    if (codigo[0] == '\0') {
        printf("O codigo nao pode estar vazio.\n");
        return;
    }

    ler_texto_alertas(
        "Descricao do alerta: ",
        descricao,
        sizeof(descricao)
    );

    ler_texto_alertas(
        "Estado observado: ",
        estado,
        sizeof(estado)
    );

    if (
        !ler_inteiro_alertas(
            "Severidade: ",
            &severidade
        )
    ) {
        printf("Severidade invalida.\n");
        return;
    }

    if (severidade < 0) {
        printf(
            "A severidade nao pode ser negativa.\n"
        );
        return;
    }

    id = aplicacao->proximo_id_alerta;

    resultado = alerta_registar(
        &aplicacao->alertas,
        &aplicacao->fila_alertas_pendentes,
        id,
        codigo,
        descricao,
        estado,
        severidade
    );

    if (resultado == 1) {
        aplicacao->proximo_id_alerta++;

        printf(
            "Alerta registado com o ID %d.\n",
            id
        );
    } else if (resultado == -1) {
        printf(
            "Ja existe um alerta ativo com os mesmos dados.\n"
        );
    } else {
        printf(
            "Nao foi possivel registar o alerta.\n"
        );
    }
}

static void processar_proximo_alerta(
    EstadoAplicacao *aplicacao
)
{
    Alerta *alerta;

    alerta = alerta_processar_proximo(
        &aplicacao->fila_alertas_pendentes
    );

    if (alerta == NULL) {
        printf(
            "Nao existem alertas pendentes.\n"
        );
        return;
    }

    printf("\nAlerta colocado Em Curso:\n");
    printf("ID: %d\n", alerta->id);
    printf(
        "Sensor: %s\n",
        alerta->codigo_sensor
    );
    printf(
        "Descricao: %s\n",
        alerta->descricao
    );
    printf(
        "Severidade: %d\n",
        alerta->severidade
    );
}

static void concluir_alerta_em_curso(
    EstadoAplicacao *aplicacao
)
{
    int id;
    int resultado;

    if (
        !ler_inteiro_alertas(
            "ID do alerta a concluir: ",
            &id
        )
    ) {
        printf("ID invalido.\n");
        return;
    }

    resultado = alerta_concluir(
        aplicacao->alertas,
        id
    );

    if (resultado == 1) {
        printf("Alerta concluido.\n");
    } else if (resultado == -1) {
        printf(
            "O alerta existe, mas nao esta Em Curso.\n"
        );
    } else {
        printf("Alerta nao encontrado.\n");
    }
}

static void pesquisar_alerta(
    EstadoAplicacao *aplicacao
)
{
    int id;
    Alerta *alerta;

    if (
        !ler_inteiro_alertas(
            "ID do alerta: ",
            &id
        )
    ) {
        printf("ID invalido.\n");
        return;
    }

    alerta = alerta_pesquisar_por_id(
        aplicacao->alertas,
        id
    );

    if (alerta == NULL) {
        printf("Alerta nao encontrado.\n");
        return;
    }

    printf("\nAlerta encontrado:\n");
    printf("ID: %d\n", alerta->id);
    printf(
        "Sensor: %s\n",
        alerta->codigo_sensor
    );
    printf(
        "Descricao: %s\n",
        alerta->descricao
    );
    printf(
        "Estado observado: %s\n",
        alerta->estado_observado
    );
    printf(
        "Severidade: %d\n",
        alerta->severidade
    );
    printf(
        "Atendimento: %s\n",
        alerta_estado_para_texto(
            alerta->atendimento
        )
    );
}

static void mostrar_totais_alertas(
    const EstadoAplicacao *aplicacao
)
{
    printf(
        "Pendentes: %d\n",
        alertas_contar_por_estado(
            aplicacao->alertas,
            ALERTA_PENDENTE
        )
    );

    printf(
        "Em Curso: %d\n",
        alertas_contar_por_estado(
            aplicacao->alertas,
            ALERTA_EM_CURSO
        )
    );

    printf(
        "Concluidos: %d\n",
        alertas_contar_por_estado(
            aplicacao->alertas,
            ALERTA_CONCLUIDO
        )
    );
}

void menu_alertas_executar(
    EstadoAplicacao *aplicacao
)
{
    int opcao;

    if (aplicacao == NULL) {
        return;
    }

    do {
        printf("\n--- Gestao de alertas ---\n");
        printf("1. Registar alerta manual\n");
        printf("2. Processar proximo alerta\n");
        printf("3. Concluir alerta Em Curso\n");
        printf("4. Listar todos os alertas\n");
        printf("5. Listar alertas pendentes\n");
        printf("6. Listar alertas Em Curso\n");
        printf("7. Listar alertas concluidos\n");
        printf("8. Pesquisar alerta por ID\n");
        printf("9. Mostrar total das alertas\n");
        printf("0. Voltar ao menu principal\n");

        if (
            !ler_inteiro_alertas(
                "\nOpcao: ",
                &opcao
            )
        ) {
            printf("Opcao invalida.\n");
            opcao = -1;
            continue;
        }

        switch (opcao) {
            case 1:
                registar_alerta_manual(aplicacao);
                break;

            case 2:
                processar_proximo_alerta(aplicacao);
                break;

            case 3:
                concluir_alerta_em_curso(aplicacao);
                break;

            case 4:
                alertas_listar(aplicacao->alertas);
                break;

            case 5:
                alertas_listar_por_estado(
                    aplicacao->alertas,
                    ALERTA_PENDENTE
                );
                break;

            case 6:
                alertas_listar_por_estado(
                    aplicacao->alertas,
                    ALERTA_EM_CURSO
                );
                break;

            case 7:
                alertas_listar_por_estado(
                    aplicacao->alertas,
                    ALERTA_CONCLUIDO
                );
                break;

            case 8:
                pesquisar_alerta(aplicacao);
                break;

            case 9:
                mostrar_totais_alertas(aplicacao);
                break;

            case 0:
                break;

            default:
                printf("Opcao invalida.\n");
                break;
        }

    } while (opcao != 0);
}