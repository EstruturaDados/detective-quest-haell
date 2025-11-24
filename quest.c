#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// -------------------------------------------------------------
// ESTRUTURAS DE DADOS
// -------------------------------------------------------------
//

// Nó da árvore da mansão
typedef struct sala {
    char nome[50];
    char pista[100];       // pista opcional associada ao cômodo
    struct sala *esq;      // caminho à esquerda
    struct sala *dir;      // caminho à direita
} Sala;

// Nó da árvore BST de pistas coletadas
typedef struct pistaNode {
    char pista[100];
    struct pistaNode *esq;
    struct pistaNode *dir;
} PistaNode;


//
// -------------------------------------------------------------
// FUNÇÕES DA MANSÃO
// -------------------------------------------------------------
//

// criarSala() — cria dinamicamente um cômodo com ou sem pista
Sala* criarSala(const char *nome, const char *pista) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));
    if (!nova) {
        printf("Erro ao alocar memória para sala!\n");
        exit(1);
    }

    strcpy(nova->nome, nome);

    if (pista != NULL)
        strcpy(nova->pista, pista);
    else
        strcpy(nova->pista, "");

    nova->esq = NULL;
    nova->dir = NULL;

    return nova;
}


//
// -------------------------------------------------------------
// FUNÇÕES DA BST DE PISTAS
// -------------------------------------------------------------
//

// inserirPista() — insere pista coletada na árvore BST (ordenada)
PistaNode* inserirPista(PistaNode *raiz, const char *pista) {
    if (raiz == NULL) {
        PistaNode *novo = (PistaNode*) malloc(sizeof(PistaNode));
        if (!novo) {
            printf("Erro ao alocar memória para pista!\n");
            exit(1);
        }
        strcpy(novo->pista, pista);
        novo->esq = novo->dir = NULL;
        return novo;
    }

    if (strcmp(pista, raiz->pista) < 0)
        raiz->esq = inserirPista(raiz->esq, pista);
    else if (strcmp(pista, raiz->pista) > 0)
        raiz->dir = inserirPista(raiz->dir, pista);

    return raiz;
}

// exibirPistas() — imprime as pistas em ordem alfabética (em-order)
void exibirPistas(PistaNode *raiz) {
    if (raiz == NULL) return;

    exibirPistas(raiz->esq);
    printf(" - %s\n", raiz->pista);
    exibirPistas(raiz->dir);
}


//
// -------------------------------------------------------------
// FUNÇÃO DE EXPLORAÇÃO DA MANSÃO
// -------------------------------------------------------------
//

// explorarSalasComPistas() — controla navegação, coleta pistas automaticamente
void explorarSalasComPistas(Sala *atual, PistaNode **arvorePistas) {
    char escolha;

    while (1) {
        printf("\nVocê está na sala: **%s**\n", atual->nome);

        // Coleta automática da pista, caso exista
        if (strlen(atual->pista) > 0) {
            printf("Pista encontrada! -> \"%s\"\n", atual->pista);
            *arvorePistas = inserirPista(*arvorePistas, atual->pista);
        } else {
            printf("Não há pistas neste cômodo.\n");
        }

        // Menu de navegação
        printf("\nEscolha um caminho:\n");
        if (atual->esq != NULL) printf("  (e) Ir para a esquerda\n");
        if (atual->dir != NULL) printf("  (d) Ir para a direita\n");
        printf("  (s) Sair da exploração\n");

        printf("Opção: ");
        scanf(" %c", &escolha);

        if (escolha == 'e' || escolha == 'E') {
            if (atual->esq != NULL) atual = atual->esq;
            else printf("Não há sala à esquerda!\n");

        } else if (escolha == 'd' || escolha == 'D') {
            if (atual->dir != NULL) atual = atual->dir;
            else printf("Não há sala à direita!\n");

        } else if (escolha == 's' || escolha == 'S') {
            printf("\nExploração encerrada!\n");
            return;

        } else {
            printf("Opção inválida! Tente novamente.\n");
        }
    }
}


//
// -------------------------------------------------------------
// MAIN — monta a mansão e inicia o jogo
// -------------------------------------------------------------
//
int main() {
    // Criando as salas da mansão com pistas associadas
    Sala *hall        = criarSala("Hall de Entrada",      "Pegadas de botas sujas");
    Sala *salaEstar   = criarSala("Sala de Estar",        "Um lenço perfumado");
    Sala *cozinha     = criarSala("Cozinha",              "Uma faca fora do lugar");
    Sala *biblioteca  = criarSala("Biblioteca",           "Um livro antigo aberto");
    Sala *jardim      = criarSala("Jardim",               "Pegadas recentes na terra");
    Sala *corredor    = criarSala("Corredor",             NULL);
    Sala *porao       = criarSala("Porão",                "Um cadeado quebrado");

    // Montando a árvore da mansão
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

    // Raiz da árvore de pistas coletadas
    PistaNode *pistas = NULL;

    printf("=== Detective Quest — Coleta de Pistas (Nível Aventureiro) ===\n");

    // Iniciar exploração
    explorarSalasComPistas(hall, &pistas);

    // Exibir pistas coletadas
    printf("\n=== PISTAS COLETADAS ===\n");
    if (pistas == NULL) {
        printf("Nenhuma pista coletada.\n");
    } else {
        exibirPistas(pistas);
    }

    return 0;
}
