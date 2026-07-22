#include <stdio.h>

#include "estado.h"
#include "menu.h"
#include "persistencia.h"

int main(void)
{
    EstadoAplicacao aplicacao;

    printf("============================================\n");
    printf(" Sistema de Monitorizacao do Centro de Dados\n");
    printf("============================================\n\n");

    estado_inicializar(&aplicacao);

    printf("A carregar os dados guardados...\n");

    if (!persistencia_carregar_tudo(&aplicacao)) {
        printf(
            "Aviso: nao foi possivel carregar corretamente "
            "todos os ficheiros.\n"
        );

        printf(
            "A aplicacao vai continuar com estruturas vazias "
            "ou parcialmente carregadas.\n"
        );
    } else {
        printf("Carregamento concluido.\n");
    }

    menu_executar(&aplicacao);

    printf("\nA guardar os dados...\n");

    if (!persistencia_guardar_tudo(&aplicacao)) {
        printf(
            "Aviso: nao foi possivel guardar todos os dados.\n"
        );
    } else {
        printf("Dados guardados com sucesso.\n");
    }

    estado_libertar(&aplicacao);

    printf("\nMemoria libertada. Aplicacao terminada.\n");

    return 0;
}