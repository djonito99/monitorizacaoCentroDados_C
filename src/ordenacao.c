#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ordenacao.h"

static Sensor **criar_vetor_sensores(
    const Sensor *lista,
    int *quantidade
)
{
    const Sensor *atual;
    Sensor **vetor;
    int total;
    int indice;

    if (quantidade == NULL) {
        return NULL;
    }

    total = sensores_contar(lista);
    *quantidade = total;

    if (total == 0) {
        return NULL;
    }

    vetor = malloc(
        (size_t)total * sizeof(Sensor *)
    );

    if (vetor == NULL) {
        *quantidade = 0;
        return NULL;
    }

    atual = lista;
    indice = 0;

    while (atual != NULL) {
        /*
         * Guardamos apenas o endereço do sensor.
         * Não copiamos nem alteramos a lista.
         */
        vetor[indice] = (Sensor *)atual;

        indice++;
        atual = atual->proximo;
    }

    return vetor;
}

static int sensor_deve_aparecer_antes(
    const Sensor *sensor_a,
    const Sensor *sensor_b
)
{
    /*
     * Maior severidade aparece primeiro.
     */
    if (sensor_a->severidade > sensor_b->severidade) {
        return 1;
    }

    if (sensor_a->severidade < sensor_b->severidade) {
        return 0;
    }

    /*
     * Em caso de empate, código crescente.
     */
    return strcmp(
        sensor_a->codigo,
        sensor_b->codigo
    ) < 0;
}

static void ordenar_selection_sort(
    Sensor **vetor,
    int quantidade
)
{
    int i;
    int j;
    int melhor;
    Sensor *temporario;

    for (i = 0; i < quantidade - 1; i++) {
        melhor = i;

        for (j = i + 1; j < quantidade; j++) {
            if (
                sensor_deve_aparecer_antes(
                    vetor[j],
                    vetor[melhor]
                )
            ) {
                melhor = j;
            }
        }

        if (melhor != i) {
            temporario = vetor[i];
            vetor[i] = vetor[melhor];
            vetor[melhor] = temporario;
        }
    }
}

void sensores_listar_ordenados(
    const Sensor *lista
)
{
    Sensor **vetor;
    int quantidade;
    int i;

    vetor = criar_vetor_sensores(
        lista,
        &quantidade
    );

    if (quantidade == 0) {
        printf("Nao existem sensores registados.\n");
        return;
    }

    if (vetor == NULL) {
        printf(
            "Nao foi possivel reservar memoria "
            "para a ordenacao.\n"
        );
        return;
    }

    ordenar_selection_sort(
        vetor,
        quantidade
    );

    printf("\n");
    printf("============================================================\n");
    printf("       SENSORES ORDENADOS POR SEVERIDADE E CODIGO\n");
    printf("============================================================\n");

    for (i = 0; i < quantidade; i++) {
        printf(
            "%d | %-15s | %-20s | %.2f %-8s | %s\n",
            vetor[i]->severidade,
            vetor[i]->codigo,
            vetor[i]->tipo,
            vetor[i]->valor,
            vetor[i]->unidade,
            vetor[i]->estado
        );
    }

    free(vetor);
}