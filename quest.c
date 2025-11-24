#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// -------------------------------------------------------------
// Estrutura da sala da mansão (nó da árvore binária)
// -------------------------------------------------------------
typedef struct sala {
    char nome[50];
    struct sala *esq;   // caminho à esquerda
    struct sala *dir;   // caminho à direita
} Sala;

// -------------------------------------------------------------
// criarSala() — cria dinamicamente uma sala com nome
// -------------------------------------------------------------
Sala* criarSala(const char *nome) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));
    if (nova == NULL) {
        printf("Erro ao alocar memória!\n");
        exit(1);
    }

    strcpy(nova->nome, nome);
    nova->esq = NULL;
    nova->dir = NULL;

    return nova;
}

// -------------------------------------------------------------
// explorarSalas() — permite a navegação interativa pela mansão
// -------------------------------------------------------------
void explorarSalas(Sala *atual) {
    char escolha;

    while (atual != NULL) {
        printf("\nVocê está na sala: **%s**\n", atual->nome);

        // Caso seja folha (não tem caminhos)
        if (atual->esq == NULL && atual->dir == NULL) {
            printf("Você chegou ao fim do caminho! Não há mais portas.\n");
            return;
        }

        printf("Escolha um caminho:\n");
        if (atual->esq != NULL) printf("  (e) Ir para a esquerda\n");
        if (atual->dir != NULL) printf("  (d) Ir para a direita\n");
        printf("  (s) Sair da exploração\n");

        printf("Opção: ");
        scanf(" %c", &escolha);

        if (escolha == 'e' || escolha == 'E') {
            if (atual->esq != NULL)
                atual = atual->esq;
            else
                printf("Não há sala à esquerda!\n");

        } else if (escolha == 'd' || escolha == 'D') {
            if (atual->dir != NULL)
                atual = atual->dir;
            else
                printf("Não há sala à direita!\n");

        } else if (escolha == 's' || escolha == 'S') {
            printf("Exploração encerrada!\n");
            return;

        } else {
            printf("Opção inválida! Tente novamente.\n");
        }
    }
}

// -------------------------------------------------------------
// main() — monta a árvore (mapa da mansão) e inicia a exploração
// -------------------------------------------------------------
int main() {
    // Criando salas da mansão
    Sala *hall        = criarSala("Hall de Entrada");
    Sala *salaEstar   = criarSala("Sala de Estar");
    Sala *cozinha     = criarSala("Cozinha");
    Sala *biblioteca  = criarSala("Biblioteca");
    Sala *jardim      = criarSala("Jardim");
    Sala *corredor    = criarSala("Corredor");
    Sala *porao       = criarSala("Porão");

    // Montando a árvore (mapa fixo)
    //
    //                Hall
    //             /        \
    //      Sala Estar     Cozinha
    //       /     \         /     \
    // Biblioteca Jardim Corredor  Porão
    //

    hall->esq = salaEstar;
    hall->dir = cozinha;

    salaEstar->esq = biblioteca;
    salaEstar->dir = jardim;

    cozinha->esq = corredor;
    cozinha->dir = porao;

    printf("=== Detective Quest — Exploração da Mansão ===\n");

    // Iniciar exploração pelo Hall
    explorarSalas(hall);

    return 0;
}
