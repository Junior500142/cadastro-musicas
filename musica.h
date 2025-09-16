#ifndef MUSICA_H
#define MUSICA_H

// Estrutura para representar uma música
typedef struct {
    char titulo[128];
    char artista[128];
    char album[128];
    int ano;
} Musica;

// Estrutura para um nó da lista encadeada
typedef struct No {
    Musica musica;
    struct No *proximo;
} No;

// Protótipos das funções da lista encadeada
No* criar_no(Musica musica);
No* inserir_musica(No *lista, Musica musica);
No* remover_musica(No *lista, const char *titulo);
Musica* buscar_musica(No *lista, const char *titulo);
void listar_musicas(No *lista);
void liberar_lista(No *lista);

// Protótipos das funções de persistência
void salvar_musicas_para_arquivo(No *lista, const char *nome_arquivo);
No* carregar_musicas_de_arquivo(const char *nome_arquivo);

#endif // MUSICA_H


