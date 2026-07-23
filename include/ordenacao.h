#ifndef ORDENACAO_H
#define ORDENACAO_H

#include "sensores.h"

/* Apresenta os sensores ordenados por:
 * 1. severidade decrescente;
 * 2. código crescente em caso de empate.
 * A lista ligada original não é alterada.
 */
void sensores_listar_ordenados(
    const Sensor *lista
);

#endif