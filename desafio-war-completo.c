#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// --- ESTRUTURAS ---
typedef struct {
    char nome[30];
    char cor[10]; // "Azul", "Vermelho", etc.
    int tropas;
} Territorio;

// Lista de missões possíveis (Vetor de Strings constantes)
const char* LISTA_MISSOES[] = {
    "Conquistar a maioria dos territorios (3 ou mais)",
    "Eliminar totalmente o exercito Vermelho",
    "Acumular um total de 20 tropas no mapa"
};

// --- PROTOTIPOS ---
void atribuirMissao(char** missaoDestino, int totalMissoes);
int verificarMissao(char* missao, Territorio* mapa, int qtdTerritorios, char* corJogador);
void atacar(Territorio* atacante, Territorio* defensor);
void exibirMapa(Territorio* mapa, int qtd);
void liberarMemoria(Territorio* mapa, char* missao);

// --- MAIN ---
int main() {
    Territorio* mapa;
    char* missaoJogador = NULL; // Ponteiro para a string da missão
    int qtdTerritorios = 5;     // Fixo para este exemplo
    int rodando = 1;            // Controle do Game Loop
    int idAtacante, idDefensor;
    
    srand(time(NULL));

    printf("=== WAR: NIVEL MESTRE (FINAL) ===\n");

    // 1. Alocação Dinâmica do Mapa
    mapa = (Territorio*) calloc(qtdTerritorios, sizeof(Territorio));
    
    // Setup inicial (Hardcoded para agilizar o teste)
    strcpy(mapa[0].nome, "Brasil");   strcpy(mapa[0].cor, "Azul");     mapa[0].tropas = 5;
    strcpy(mapa[1].nome, "Franca");   strcpy(mapa[1].cor, "Vermelho"); mapa[1].tropas = 3;
    strcpy(mapa[2].nome, "China");    strcpy(mapa[2].cor, "Vermelho"); mapa[2].tropas = 8;
    strcpy(mapa[3].nome, "EUA");      strcpy(mapa[3].cor, "Azul");     mapa[3].tropas = 6;
    strcpy(mapa[4].nome, "Egito");    strcpy(mapa[4].cor, "Neutro");   mapa[4].tropas = 2;

    // 2. Atribuição de Missão (Passagem por Referência de Ponteiro)
    // Passamos o endereço do ponteiro (&missaoJogador) para alocar dentro da função
    atribuirMissao(&missaoJogador, 3);
    
    printf("\n>>> SUA MISSAO SECRETA: %s <<<\n", missaoJogador);
    printf("Pressione ENTER para começar...");
    getchar(); 

    // 3. GAME LOOP (O Jogo Real)
    while (rodando) {
        printf("\n========================================\n");
        exibirMapa(mapa, qtdTerritorios);
        printf("========================================\n");

        // Verifica Condição de Vitória ANTES da jogada
        if (verificarMissao(missaoJogador, mapa, qtdTerritorios, "Azul")) {
            printf("\nPARABENS! Voce cumpriu sua missao: %s\n", missaoJogador);
            rodando = 0;
            break;
        }

        // Entrada de dados
        printf("\n[Sua cor e AZUL]\n");
        printf("Digite ID Atacante (0 para sair): ");
        scanf("%d", &idAtacante);
        if (idAtacante == 0) break;

        printf("Digite ID Defensor: ");
        scanf("%d", &idDefensor);
        idAtacante--; idDefensor--; // Ajuste de índice

        // Validação Simples
        if (idAtacante >= 0 && idAtacante < qtdTerritorios && 
            idDefensor >= 0 && idDefensor < qtdTerritorios) {
            
            // Só pode atacar com seus territórios
            if (strcmp(mapa[idAtacante].cor, "Azul") == 0) {
                 atacar(&mapa[idAtacante], &mapa[idDefensor]);
            } else {
                printf("Erro: Voce so pode atacar com exercitos Azuis!\n");
            }
        } else {
            printf("IDs invalidos!\n");
        }
    }

    // 4. Encerramento e Limpeza
    liberarMemoria(mapa, missaoJogador);
    printf("Jogo Encerrado.\n");

    return 0;
}

// --- IMPLEMENTAÇÃO ---

/* * Atribui uma missão aleatória.
 * IMPORTANTE: Recebe char** (ponteiro para ponteiro) para poder
 * alterar para onde o ponteiro original aponta (malloc).
 */
void atribuirMissao(char** missaoDestino, int totalMissoes) {
    int sorteio = rand() % totalMissoes;
    const char* missaoSorteada = LISTA_MISSOES[sorteio];

    // Alocação dinâmica baseada no tamanho da string sorteada (+1 para o \0)
    *missaoDestino = (char*) malloc((strlen(missaoSorteada) + 1) * sizeof(char));
    
    if (*missaoDestino != NULL) {
        strcpy(*missaoDestino, missaoSorteada);
    }
}

/*
 * Lógica simples de verificação baseada na string da missão.
 * Retorna 1 se venceu, 0 se não.
 */
int verificarMissao(char* missao, Territorio* mapa, int qtd, char* corJogador) {
    int contagem = 0;
    int i;

    // Lógica 1: Maioria dos territórios
    if (strstr(missao, "maioria") != NULL) {
        for(i=0; i<qtd; i++) {
            if (strcmp(mapa[i].cor, corJogador) == 0) contagem++;
        }
        return (contagem >= 3); // Vitória se tiver 3 ou mais
    }
    
    // Lógica 2: Eliminar Vermelho (Verifica se NÃO existe vermelho)
    if (strstr(missao, "Vermelho") != NULL) {
        for(i=0; i<qtd; i++) {
            if (strcmp(mapa[i].cor, "Vermelho") == 0) return 0; // Ainda existe vermelho
        }
        return 1; // Não achou vermelho, ganhou
    }

    // Lógica 3: Acumular tropas
    if (strstr(missao, "20 tropas") != NULL) {
        for(i=0; i<qtd; i++) {
            if (strcmp(mapa[i].cor, corJogador) == 0) contagem += mapa[i].tropas;
        }
        return (contagem >= 20);
    }

    return 0;
}

void atacar(Territorio* atacante, Territorio* defensor) {
    printf("\n>>> COMBATE: %s (%d) vs %s (%d) <<<\n", 
           atacante->nome, atacante->tropas, defensor->nome, defensor->tropas);
    
    if (atacante->tropas <= 1) {
        printf("Ataque cancelado: Tropas insuficientes (minimo 2).\n");
        return;
    }

    int dAtacante = (rand() % 6) + 1;
    int dDefensor = (rand() % 6) + 1;
    printf("Dados: Atq [%d] x Def [%d] -> ", dAtacante, dDefensor);

    if (dAtacante > dDefensor) {
        printf("VITORIA DO ATACANTE!\n");
        // Lógica de conquista
        if (defensor->tropas <= 0 || dAtacante > dDefensor + 2) { 
             // Simplificação: Conquista imediata para dinâmica do jogo
             printf("O territorio %s foi CONQUISTADO!\n", defensor->nome);
             strcpy(defensor->cor, atacante->cor);
             defensor->tropas = atacante->tropas / 2;
             atacante->tropas -= defensor->tropas;
        } else {
             defensor->tropas--; // Apenas baixa
             printf("Defensor perdeu 1 tropa.\n");
        }
    } else {
        printf("DEFESA SEGUROU!\n");
        atacante->tropas--;
    }
}

void exibirMapa(Territorio* mapa, int qtd) {
    for (int i = 0; i < qtd; i++) {
        printf("%d. %-10s [%-8s] - Tropas: %d\n", 
            i + 1, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
}

void liberarMemoria(Territorio* mapa, char* missao) {
    if (mapa != NULL) free(mapa);
    if (missao != NULL) free(missao);
}
