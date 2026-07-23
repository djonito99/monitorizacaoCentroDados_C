#ifndef API_H
#define API_H

#include <stddef.h>

#define API_ENDPOINT_SENSORES \
    "https://sensorlab.innominatum.pt/v1/sensors"   //endpoint

#define API_FICHEIRO_TEMPORARIO \
    "data/resposta_temporaria.json"

typedef struct {
    char *conteudo;
    size_t tamanho;
} RespostaApi;

 // Coloca a resposta num estado inicial seguro.
void api_resposta_inicializar(
    RespostaApi *resposta
);

/* Executa o pedido ao endpoint através do comando curl
 * e carrega o conteúdo recebido para memória dinâmica.
 *
 * Devolve:
 * 1 - sucesso;
 * 0 - falha.
 */
int api_obter_resposta(
    RespostaApi *resposta
);

 // Liberta o conteudo dinamico da resposta.
void api_libertar_resposta(
    RespostaApi *resposta
);

#endif