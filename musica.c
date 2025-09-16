#include "musica.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Função para criar um novo nó da lista encadeada
No* criar_no(Musica musica) {
    No *novo_no = (No*)malloc(sizeof(No));
    if (novo_no == NULL) {
        perror("Erro ao alocar memória para o nó");
        exit(EXIT_FAILURE);
    }
    novo_no->musica = musica;
    novo_no->proximo = NULL;
    return novo_no;
}

// Função para inserir uma música na lista encadeada (no final)
No* inserir_musica(No *lista, Musica musica) {
    No *novo_no = criar_no(musica);
    if (lista == NULL) {
        return novo_no;
    } else {
        No *atual = lista;
        while (atual->proximo != NULL) {
            atual = atual->proximo;
        }
        atual->proximo = novo_no;
        return lista;
    }
}

// Função para remover uma música da lista encadeada por título
No* remover_musica(No *lista, const char *titulo) {
    if (lista == NULL) {
        printf("Lista vazia. Nenhuma música para remover.\n");
        return NULL;
    }

    No *atual = lista;
    No *anterior = NULL;

    // Se a música a ser removida é o primeiro nó
    if (strcmp(atual->musica.titulo, titulo) == 0) {
        lista = atual->proximo;
        free(atual);
        printf("Música \'%s\' removida com sucesso.\n", titulo);
        return lista;
    }

    // Procura a música na lista
    while (atual != NULL && strcmp(atual->musica.titulo, titulo) != 0) {
        anterior = atual;
        atual = atual->proximo;
    }

    // Se a música foi encontrada
    if (atual != NULL) {
        anterior->proximo = atual->proximo;
        free(atual);
        printf("Música \'%s\' removida com sucesso.\n", titulo);
    } else {
        printf("Música \'%s\' não encontrada na lista.\n", titulo);
    }
    return lista;
}

// Função para buscar uma música na lista encadeada por título
Musica* buscar_musica(No *lista, const char *titulo) {
    No *atual = lista;
    while (atual != NULL) {
        if (strcmp(atual->musica.titulo, titulo) == 0) {
            return &(atual->musica);
        }
        atual = atual->proximo;
    }
    return NULL; // Música não encontrada
}

// Função para listar todas as músicas na lista encadeada
void listar_musicas(No *lista) {
    if (lista == NULL) {
        printf("Nenhuma música cadastrada.\n");
        return;
    }
    No *atual = lista;
    printf("\n--- Lista de Músicas ---\n");
    while (atual != NULL) {
        printf("Título: %s\n", atual->musica.titulo);
        printf("Artista: %s\n", atual->musica.artista);
        printf("Álbum: %s\n", atual->musica.album);
        printf("Ano: %d\n", atual->musica.ano);
        printf("------------------------\n");
        atual = atual->proximo;
    }
}

// Função para liberar a memória alocada pela lista encadeada
void liberar_lista(No *lista) {
    No *atual = lista;
    No *proximo;
    while (atual != NULL) {
        proximo = atual->proximo;
        free(atual);
        atual = proximo;
    }
}

// Função para salvar as músicas em um arquivo binário
void salvar_musicas_para_arquivo(No *lista, const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "wb");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo para escrita");
        return;
    }

    No *atual = lista;
    while (atual != NULL) {
        fwrite(&(atual->musica), sizeof(Musica), 1, arquivo);
        atual = atual->proximo;
    }

    fclose(arquivo);
    printf("Músicas salvas em %s com sucesso.\n", nome_arquivo);
}

// Função para carregar as músicas de um arquivo binário
No* carregar_musicas_de_arquivo(const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "rb");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo para leitura");
        return NULL;
    }

    No *lista = NULL;
    Musica musica_lida;
    while (fread(&musica_lida, sizeof(Musica), 1, arquivo) == 1) {
        lista = inserir_musica(lista, musica_lida);
    }

    fclose(arquivo);
    printf("Músicas carregadas de %s com sucesso.\n", nome_arquivo);
    return lista;
}


