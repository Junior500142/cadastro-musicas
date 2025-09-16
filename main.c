#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "musica.h"

#define NOME_ARQUIVO_DADOS "musicas.bin"

void exibir_menu() {
    printf("\n--- Gerenciador de Músicas ---\n");
    printf("1. Adicionar nova música\n");
    printf("2. Listar todas as músicas\n");
    printf("3. Remover música por título\n");
    printf("0. Sair\n");
    printf("Escolha uma opção: ");
}

int main() {
    No *lista_musicas = NULL;
    int opcao;
    Musica nova_musica;
    char titulo_remover[128];

    // Carregar músicas do arquivo ao iniciar o programa
    lista_musicas = carregar_musicas_de_arquivo(NOME_ARQUIVO_DADOS);

    do {
        exibir_menu();
        scanf("%d", &opcao);
        getchar(); // Consumir o newline

        switch (opcao) {
            case 1:
                printf("\n--- Adicionar Nova Música ---\n");
                printf("Título: ");
                fgets(nova_musica.titulo, sizeof(nova_musica.titulo), stdin);
                nova_musica.titulo[strcspn(nova_musica.titulo, "\n")] = 0; // Remover newline

                printf("Artista: ");
                fgets(nova_musica.artista, sizeof(nova_musica.artista), stdin);
                nova_musica.artista[strcspn(nova_musica.artista, "\n")] = 0; // Remover newline

                printf("Álbum: ");
                fgets(nova_musica.album, sizeof(nova_musica.album), stdin);
                nova_musica.album[strcspn(nova_musica.album, "\n")] = 0; // Remover newline

                printf("Ano: ");
                scanf("%d", &nova_musica.ano);
                getchar(); // Consumir o newline

                lista_musicas = inserir_musica(lista_musicas, nova_musica);
                printf("Música adicionada com sucesso!\n");
                break;
            case 2:
                listar_musicas(lista_musicas);
                break;
            case 3:
                printf("\n--- Remover Música ---\n");
                printf("Digite o título da música a ser removida: ");
                fgets(titulo_remover, sizeof(titulo_remover), stdin);
                titulo_remover[strcspn(titulo_remover, "\n")] = 0; // Remover newline

                lista_musicas = remover_musica(lista_musicas, titulo_remover);
                break;
            case 0:
                printf("Saindo do programa...\n");
                break;
            default:
                printf("Opção inválida. Tente novamente.\n");
                break;
        }
    } while (opcao != 0);

    // Salvar músicas no arquivo ao encerrar o programa
    salvar_musicas_para_arquivo(lista_musicas, NOME_ARQUIVO_DADOS);
    liberar_lista(lista_musicas);
    return 0;
}


