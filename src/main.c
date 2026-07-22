#include <stdio.h>

#include "estado.h"
#include "menu.h"

int main(void)
{
    EstadoAplicacao aplicacao;

    printf("============================================\n");
    printf(" Sistema de Monitorizacao do Centro de Dados\n");
    printf("============================================\n\n");

    estado_inicializar(&aplicacao);

    menu_executar(&aplicacao);

    estado_libertar(&aplicacao);

    printf("\nMemoria libertada. Aplicacao terminada.\n");

    return 0;
}