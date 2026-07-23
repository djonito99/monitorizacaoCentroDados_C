#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include "sensores.h"

typedef struct {
    Sensor *leituras;

    int recebidas;
    int validas;
    int rejeitadas;
} ResultadoJson;

/* Interpreta o elemento readings da resposta.
 *
 * As leituras validas sao colocadas numa lista ligada
 * temporaria, mantendo a ordem recebida da API.
 */
int json_interpretar_leituras(
    const char *json,
    ResultadoJson *resultado
);

 // Liberta a lista temporaria criada pelo parser.
void json_resultado_libertar(
    ResultadoJson *resultado
);

#endif