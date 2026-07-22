#include <stdio.h>
#include <stdlib.h>
#include "menu_historico.h"
#include "historico.h"

static void limpar_buffer_historico(void)
{
    int caractere;

    do {
        caractere = getchar();
    } while (
        caractere != '\n' &&
        caractere != EOF
    );
}

static int ler_inteiro_historico(
    const char *mensagem,
    int *valor
)
{
    printf("%s", mensagem);

    if (scanf("%d", valor) != 1) {
        limpar_buffer_historico();
        return 0;
    }

    limpar_buffer_historico();
    return 1;
}

static void adicionar_sincronizacao_teste(
    EstadoAplicacao *aplicacao
)
{
    Sincronizacao *registo;
    int resultado;

    registo = historico_criar_registo(
        SINCRONIZACAO_SUCESSO,
        10,
        3,
        2,
        4,
        1,
        2
    );

    if (registo == NULL) {
        printf("Falha ao criar o registo.\n");
        return;
    }

    resultado = historico_empilhar(
        &aplicacao->historico,
        registo
    );

    if (!resultado) {
        free(registo);
        printf("Falha ao inserir no historico.\n");
        return;
    }

    printf("Sincronizacao de teste adicionada.\n");
}

static void consultar_ultima(
    const EstadoAplicacao *aplicacao
)
{
    const Sincronizacao *ultima;

    ultima = historico_consultar_ultima(
        aplicacao->historico
    );

    if (ultima == NULL) {
        printf("O historico esta vazio.\n");
        return;
    }

    historico_listar_ultimas(
        aplicacao->historico,
        1
    );
}

void menu_historico_executar(
    EstadoAplicacao *aplicacao
)
{
    int opcao;
    int quantidade;

    do {
        printf("\n--- Historico de sincronizacoes ---\n");
        printf("1. Adicionar sincronizacao de teste\n");
        printf("2. Consultar ultima sincronizacao\n");
        printf("3. Listar ultimas N sincronizacoes\n");
        printf("4. Mostrar total de sincronizacoes\n");
        printf("0. Voltar\n");

        if (
            !ler_inteiro_historico(
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
                adicionar_sincronizacao_teste(aplicacao);
                break;

            case 2:
                consultar_ultima(aplicacao);
                break;

            case 3:
                if (
                    !ler_inteiro_historico(
                        "Quantidade: ",
                        &quantidade
                    )
                ) {
                    printf("Quantidade invalida.\n");
                    break;
                }

                historico_listar_ultimas(
                    aplicacao->historico,
                    quantidade
                );
                break;

            case 4:
                printf(
                    "Total de sincronizacoes: %d\n",
                    historico_contar(aplicacao->historico)
                );
                break;

            case 0:
                break;

            default:
                printf("Opcao invalida.\n");
                break;
        }

    } while (opcao != 0);
}