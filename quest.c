/*
 Detective Quest — Nível MASTER
 - Árvore binária de salas (mapa da mansão)
 - BST para pistas coletadas (sem duplicatas)
 - Tabela hash associando pista -> suspeito
 - Ao final: acusação do suspeito e verificação (>= 2 pistas)
 
 Funções principais documentadas conforme solicitado.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NOME 64
#define MAX_PISTA 128
#define HASH_SIZE 101   // número primo simples para hash

// -------------------- Estruturas --------------------

typedef struct sala {
    char nome[MAX_NOME];
    char pista[MAX_PISTA]; // string vazia se não houver pista
    struct sala *esq;
    struct sala *dir;
} Sala;

typedef struct PistaNode {
    char pista[MAX_PISTA];
    struct PistaNode *esq;
    struct PistaNode *dir;
} PistaNode;

typedef struct HashNode {
    char pista[MAX_PISTA];      // chave
    char suspeito[MAX_NOME];    // valor
    struct HashNode *next;      // para encadeamento (chaining)
} HashNode;

// -------------------- Prototipos --------------------

/* criarSala() – cria dinamicamente um cômodo. */
Sala* criarSala(const char *nome, const char *pista);

/* inserirNaHash() – insere associação pista/suspeito na tabela hash. */
void inserirNaHash(HashNode *tabela[], const char *pista, const char *suspeito);

/* encontrarSuspeito() – consulta o suspeito correspondente a uma pista.
   Retorna ponteiro para string interna (não liberar). NULL se não achar. */
const char* encontrarSuspeito(HashNode *tabela[], const char *pista);

/* inserirPista() / adicionarPista() – insere a pista coletada na árvore de pistas (BST)
   sem duplicar. Retorna a raiz atualizada. */
PistaNode* inserirPista(PistaNode *raiz, const char *pista);

/* exibirPistas() — imprime as pistas em ordem alfabética (in-order traversal). */
void exibirPistas(PistaNode *raiz);

/* explorarSalas() – navega pela árvore e ativa o sistema de pistas.
   Recebe a raiz das salas, a referência para a raiz da BST de pistas e a hash. */
void explorarSalas(Sala *inicio, PistaNode **raizPistas, HashNode *tabela[]);

/* verificarSuspeitoFinal() – verifica, a partir da BST de pistas e da hash,
   quantas pistas apontam para o suspeito acusado. Retorna a contagem. */
int verificarSuspeitoFinal(PistaNode *raizPistas, HashNode *tabela[], const char *acusado);

/* Helpers: hash, leitura de linha, comparação case-insensitive, liberar memórias etc. */
unsigned long djb2_hash(const char *str);
void lerLinha(char *buffer, int tamanho);
int igualCaseInsensitive(const char *a, const char *b);

/* Funções para liberar memória (boas práticas) */
void liberarPistas(PistaNode *raiz);
void liberarHash(HashNode *tabela[]);
void liberarSalas(Sala *raiz); // libera em pós-ordem

// -------------------- Implementação --------------------

unsigned long djb2_hash(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = (unsigned char)*str++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

/* criarSala() – cria dinamicamente um cômodo. */
Sala* criarSala(const char *nome, const char *pista) {
    Sala *s = (Sala*) malloc(sizeof(Sala));
    if (!s) {
        fprintf(stderr, "Erro: memória insuficiente ao criar sala.\n");
        exit(EXIT_FAILURE);
    }
    strncpy(s->nome, nome, MAX_NOME-1);
    s->nome[MAX_NOME-1] = '\0';
    if (pista && pista[0] != '\0') {
        strncpy(s->pista, pista, MAX_PISTA-1);
        s->pista[MAX_PISTA-1] = '\0';
    } else {
        s->pista[0] = '\0';
    }
    s->esq = s->dir = NULL;
    return s;
}

/* inserirNaHash() – insere associação pista/suspeito na tabela hash. */
void inserirNaHash(HashNode *tabela[], const char *pista, const char *suspeito) {
    unsigned long h = djb2_hash(pista) % HASH_SIZE;
    // Verifica se já existe a chave para evitar duplicação
    HashNode *cur = tabela[h];
    while (cur) {
        if (strcmp(cur->pista, pista) == 0) {
            // atualiza suspeito se necessário
            strncpy(cur->suspeito, suspeito, MAX_NOME-1);
            cur->suspeito[MAX_NOME-1] = '\0';
            return;
        }
        cur = cur->next;
    }
    // criar novo nó
    HashNode *novo = (HashNode*) malloc(sizeof(HashNode));
    if (!novo) {
        fprintf(stderr, "Erro: memória insuficiente ao criar hash node.\n");
        exit(EXIT_FAILURE);
    }
    strncpy(novo->pista, pista, MAX_PISTA-1);
    novo->pista[MAX_PISTA-1] = '\0';
    strncpy(novo->suspeito, suspeito, MAX_NOME-1);
    novo->suspeito[MAX_NOME-1] = '\0';
    novo->next = tabela[h];
    tabela[h] = novo;
}

/* encontrarSuspeito() – consulta o suspeito correspondente a uma pista. */
const char* encontrarSuspeito(HashNode *tabela[], const char *pista) {
    unsigned long h = djb2_hash(pista) % HASH_SIZE;
    HashNode *cur = tabela[h];
    while (cur) {
        if (strcmp(cur->pista, pista) == 0)
            return cur->suspeito;
        cur = cur->next;
    }
    return NULL;
}

/* inserirPista() – insere pista na BST (sem duplicatas). */
PistaNode* inserirPista(PistaNode *raiz, const char *pista) {
    if (raiz == NULL) {
        PistaNode *novo = (PistaNode*) malloc(sizeof(PistaNode));
        if (!novo) {
            fprintf(stderr, "Erro: memória insuficiente ao criar nó de pista.\n");
            exit(EXIT_FAILURE);
        }
        strncpy(novo->pista, pista, MAX_PISTA-1);
        novo->pista[MAX_PISTA-1] = '\0';
        novo->esq = novo->dir = NULL;
        return novo;
    }
    int cmp = strcmp(pista, raiz->pista);
    if (cmp < 0)
        raiz->esq = inserirPista(raiz->esq, pista);
    else if (cmp > 0)
        raiz->dir = inserirPista(raiz->dir, pista);
    // se igual, não insere (evita duplicata)
    return raiz;
}

/* exibirPistas() — inorder traversal para imprimir em ordem alfabética. */
void exibirPistas(PistaNode *raiz) {
    if (!raiz) return;
    exibirPistas(raiz->esq);
    printf(" - %s\n", raiz->pista);
    exibirPistas(raiz->dir);
}

/* explorarSalas() – navega pela árvore e ativa o sistema de pistas. */
void explorarSalas(Sala *inicio, PistaNode **raizPistas, HashNode *tabela[]) {
    if (!inicio) return;
    Sala *atual = inicio;
    char opc;
    while (1) {
        printf("\nVocê está na sala: %s\n", atual->nome);

        if (strlen(atual->pista) > 0) {
            printf("Pista encontrada: \"%s\"\n", atual->pista);
            // Inserir na BST (ignora duplicatas)
            *raizPistas = inserirPista(*raizPistas, atual->pista);
            // Mostrar suspeito associado (opcional/ajuda)
            const char *sus = encontrarSuspeito(tabela, atual->pista);
            if (sus) printf("(Dica: esta pista está associada a: %s)\n", sus);
        } else {
            printf("Nenhuma pista nesta sala.\n");
        }

        // Mostrar opções
        printf("\nEscolha um caminho:\n");
        if (atual->esq) printf("  (e) Esquerda -> %s\n", atual->esq->nome);
        if (atual->dir) printf("  (d) Direita  -> %s\n", atual->dir->nome);
        printf("  (s) Sair da exploração\n");
        printf("Opção: ");
        if (scanf(" %c", &opc) != 1) {
            // limpeza caso de erro
            while (getchar() != '\n');
            printf("Entrada inválida.\n");
            continue;
        }
        // consumir resto da linha
        while (getchar() != '\n');

        if (opc == 'e' || opc == 'E') {
            if (atual->esq) atual = atual->esq;
            else printf("Não há sala à esquerda!\n");
        } else if (opc == 'd' || opc == 'D') {
            if (atual->dir) atual = atual->dir;
            else printf("Não há sala à direita!\n");
        } else if (opc == 's' || opc == 'S') {
            printf("Exploração finalizada pelo jogador.\n");
            return;
        } else {
            printf("Opção inválida, tente novamente.\n");
        }
    }
}

/* Função que percorre a BST e conta quantas pistas (nós) apontam para 'acusado'. */
int contarPistasParaSuspeito(PistaNode *raiz, HashNode *tabela[], const char *acusado) {
    if (!raiz) return 0;
    int cnt = 0;
    const char *sus = encontrarSuspeito(tabela, raiz->pista);
    if (sus && igualCaseInsensitive(sus, acusado)) cnt++;
    cnt += contarPistasParaSuspeito(raiz->esq, tabela, acusado);
    cnt += contarPistasParaSuspeito(raiz->dir, tabela, acusado);
    return cnt;
}

/* verificarSuspeitoFinal() – verifica se há ao menos 2 pistas apontando para acusado. */
int verificarSuspeitoFinal(PistaNode *raizPistas, HashNode *tabela[], const char *acusado) {
    if (!raizPistas) return 0;
    return contarPistasParaSuspeito(raizPistas, tabela, acusado);
}

// -------------------- Helpers --------------------

/* Le uma linha de entrada (trim newline). */
void lerLinha(char *buffer, int tamanho) {
    if (fgets(buffer, tamanho, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len-1] == '\n') buffer[len-1] = '\0';
}

/* comparação case-insensitive */
int igualCaseInsensitive(const char *a, const char *b) {
    if (!a || !b) return 0;
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return 0;
        a++; b++;
    }
    return *a == '\0' && *b == '\0';
}

/* liberarBST de pistas */
void liberarPistas(PistaNode *raiz) {
    if (!raiz) return;
    liberarPistas(raiz->esq);
    liberarPistas(raiz->dir);
    free(raiz);
}

/* liberar hash */
void liberarHash(HashNode *tabela[]) {
    for (int i = 0; i < HASH_SIZE; ++i) {
        HashNode *cur = tabela[i];
        while (cur) {
            HashNode *tmp = cur;
            cur = cur->next;
            free(tmp);
        }
        tabela[i] = NULL;
    }
}

/* liberar salas (pós-ordem) */
void liberarSalas(Sala *raiz) {
    if (!raiz) return;
    liberarSalas(raiz->esq);
    liberarSalas(raiz->dir);
    free(raiz);
}

// -------------------- main --------------------

int main() {
    // Criando mapa fixo da mansão (árvore binária de salas)
    // Estrutura exemplo:
    //                 Hall
    //               /      \
    //         Sala de Estar  Cozinha
    //         /       \        /   \
    //   Biblioteca  Jardim  Corredor  Porão

    Sala *hall = criarSala("Hall de Entrada", "Pegadas de botas sujas");
    Sala *salaEstar = criarSala("Sala de Estar", "Lenço com monograma");
    Sala *cozinha = criarSala("Cozinha", "Faca com mancha");
    Sala *biblioteca = criarSala("Biblioteca", "Livro aberto na página 77");
    Sala *jardim = criarSala("Jardim", "Pegadas rumo ao portão");
    Sala *corredor = criarSala("Corredor", "Marca de arranhão na parede");
    Sala *sotao = criarSala("Sótão", "Pedaços de tecido rasgado");
    Sala *porao = criarSala("Porão", "Cadeado quebrado");

    // montar relações
    hall->esq = salaEstar;
    hall->dir = cozinha;

    salaEstar->esq = biblioteca;
    salaEstar->dir = jardim;

    cozinha->esq = corredor;
    cozinha->dir = porao;

    corredor->esq = sotao; // adicionar um nível a mais para exemplo

    // Inicializar tabela hash
    HashNode *tabela[HASH_SIZE];
    for (int i = 0; i < HASH_SIZE; ++i) tabela[i] = NULL;

    // Inserir associações pista -> suspeito na hash (pré-definidas)
    inserirNaHash(tabela, "Pegadas de botas sujas", "Sr. Almeida");
    inserirNaHash(tabela, "Lenço com monograma", "Sra. Duarte");
    inserirNaHash(tabela, "Faca com mancha", "Sr. Almeida");
    inserirNaHash(tabela, "Livro aberto na página 77", "Prof. Castro");
    inserirNaHash(tabela, "Pegadas rumo ao portão", "Sra. Duarte");
    inserirNaHash(tabela, "Marca de arranhão na parede", "Sr. Almeida");
    inserirNaHash(tabela, "Pedaços de tecido rasgado", "Sra. Duarte");
    inserirNaHash(tabela, "Cadeado quebrado", "Prof. Castro");

    printf("=== Detective Quest — Nível MASTER ===\n");
    printf("Explore a mansão e colete pistas. Ao final, acuse o suspeito.\n");

    PistaNode *raizPistas = NULL;

    // Exploração interativa
    explorarSalas(hall, &raizPistas, tabela);

    // Exibir pistas coletadas
    printf("\n=== PISTAS COLETADAS ===\n");
    if (!raizPistas) {
        printf("Nenhuma pista coletada.\n");
    } else {
        exibirPistas(raizPistas);
    }

    // Fase de acusação
    char acusacao[MAX_NOME];
    printf("\nDigite o nome do suspeito que deseja acusar: ");
    lerLinha(acusacao, MAX_NOME);

    if (strlen(acusacao) == 0) {
        printf("Nenhum suspeito informado. Encerrando sem julgamento.\n");
    } else {
        int cont = verificarSuspeitoFinal(raizPistas, tabela, acusacao);
        printf("\nPistas que apontam para '%s': %d\n", acusacao, cont);
        if (cont >= 2) {
            printf("Resultado: Há evidências suficientes. '%s' é o culpado!\n", acusacao);
        } else {
            printf("Resultado: Evidências insuficientes para condenar '%s'.\n", acusacao);
        }
    }

    // Limpeza de memória
    liberarPistas(raizPistas);
    liberarHash(tabela);
    liberarSalas(hall);

    printf("\nEncerrando Detective Quest — obrigado por jogar!\n");
    return 0;
}
