#include <stdio.h>
#include "menu.h"

void menu_executar(void)
{
    int opcao;

    do {
        printf("\n--- Menu principal ---\n");
        printf("1. Atualizar agora\n");
        printf("2. Monitorizacao automatica\n");
        printf("3. Listar sensores\n");
        printf("4. Gerir alertas\n");
        printf("5. Consultar historico\n");
        printf("6. Gerar relatorio\n");
        printf("0. Sair\n");

        printf("\nOpcao: ");

        if (scanf("%d", &opcao) != 1) {
            printf("Entrada invalida.\n");

            while (getchar() != '\n') {
                /* Limpa o buffer de entrada. */
            }

            opcao = -1;
            continue;
        }
        //Opcoes disponiveis no menu
        switch (opcao) {
            case 1:
                printf("Atualizacao ainda nao implementada.\n");
                break;

            case 2:
                printf("Monitorizacao automatica ainda nao implementada.\n");
                break;

            case 3:
                printf("Listagem de sensores ainda nao implementada.\n");
                break;

            case 4:
                printf("Gestao de alertas ainda nao implementada.\n");
                break;

            case 5:
                printf("Historico ainda nao implementado.\n");
                break;

            case 6:
                printf("Relatorio ainda nao implementado.\n");
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