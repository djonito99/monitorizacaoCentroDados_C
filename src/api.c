#include <stdio.h>
#include <stdlib.h>

#include "api.h"

static int ler_ficheiro_completo(
    const char *caminho,
    char **conteudo,
    size_t *tamanho
)
{
    FILE *ficheiro;
    long tamanho_ficheiro;
    char *buffer;
    size_t quantidade_lida;

    if (
        caminho == NULL ||
        conteudo == NULL ||
        tamanho == NULL
    ) {
        return 0;
    }

    *conteudo = NULL;
    *tamanho = 0;

    ficheiro = fopen(caminho, "rb");

    if (ficheiro == NULL) {
        return 0;
    }

    if (fseek(ficheiro, 0, SEEK_END) != 0) {
        fclose(ficheiro);
        return 0;
    }

    tamanho_ficheiro = ftell(ficheiro);

    if (tamanho_ficheiro <= 0) {
        fclose(ficheiro);
        return 0;
    }

    if (fseek(ficheiro, 0, SEEK_SET) != 0) {
        fclose(ficheiro);
        return 0;
    }

    buffer = malloc(
        (size_t)tamanho_ficheiro + 1
    );

    if (buffer == NULL) {
        fclose(ficheiro);
        return 0;
    }

    quantidade_lida = fread(
        buffer,
        1,
        (size_t)tamanho_ficheiro,
        ficheiro
    );

    if (
        quantidade_lida !=
        (size_t)tamanho_ficheiro
    ) {
        free(buffer);
        fclose(ficheiro);
        return 0;
    }

    buffer[quantidade_lida] = '\0';

    if (fclose(ficheiro) != 0) {
        free(buffer);
        return 0;
    }

    *conteudo = buffer;
    *tamanho = quantidade_lida;

    return 1;
}

void api_resposta_inicializar(
    RespostaApi *resposta
)
{
    if (resposta == NULL) {
        return;
    }

    resposta->conteudo = NULL;
    resposta->tamanho = 0;
}

int api_obter_resposta(
    RespostaApi *resposta
)
{
    int codigo_saida;
    const char *comando;

    if (resposta == NULL) {
        return 0;
    }

    /*
     * Permite reutilizar uma estrutura já inicializada.
     */
    api_libertar_resposta(resposta);

    /*
     * Remove uma possível resposta temporária antiga.
     * Assim, nunca confundimos uma resposta anterior
     * com o resultado do pedido atual.
     */
    remove(API_FICHEIRO_TEMPORARIO);

#ifdef _WIN32
    comando =
        "curl.exe "
        "--fail "
        "--silent "
        "--show-error "
        "--location "
        "--connect-timeout 10 "
        "--max-time 30 "
        "--output \"" API_FICHEIRO_TEMPORARIO "\" "
        "\"" API_ENDPOINT_SENSORES "\"";
#else
    comando =
        "curl "
        "--fail "
        "--silent "
        "--show-error "
        "--location "
        "--connect-timeout 10 "
        "--max-time 30 "
        "--output \"" API_FICHEIRO_TEMPORARIO "\" "
        "\"" API_ENDPOINT_SENSORES "\"";
#endif

    codigo_saida = system(comando);

    if (codigo_saida != 0) {
        remove(API_FICHEIRO_TEMPORARIO);
        return 0;
    }

    if (
        !ler_ficheiro_completo(
            API_FICHEIRO_TEMPORARIO,
            &resposta->conteudo,
            &resposta->tamanho
        )
    ) {
        remove(API_FICHEIRO_TEMPORARIO);
        return 0;
    }

    if (
        resposta->conteudo == NULL ||
        resposta->tamanho == 0
    ) {
        api_libertar_resposta(resposta);
        remove(API_FICHEIRO_TEMPORARIO);
        return 0;
    }

    return 1;
}

void api_libertar_resposta(
    RespostaApi *resposta
)
{
    if (resposta == NULL) {
        return;
    }

    free(resposta->conteudo);

    resposta->conteudo = NULL;
    resposta->tamanho = 0;
}