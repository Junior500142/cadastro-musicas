#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "musica.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
typedef SOCKET socket_t;
#define CLOSESOCK closesocket
#define SOCKINIT()  do { WSADATA wsa; WSAStartup(MAKEWORD(2,2), &wsa); } while (0)
#define SOCKCLEAN() WSACleanup()

#define PORTA 9000
#define NOME_ARQUIVO_DADOS "musicas.bin"

// Comandos do protocolo
#define CMD_READ "READ"
#define CMD_DELETE "DELETE"

int main(void) {
    SOCKINIT();

    socket_t servidor = socket(AF_INET, SOCK_STREAM, 0);
    if (servidor < 0) { perror("socket"); return 1; }

    setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, (const char*)&(int){1}, sizeof(int));

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORTA);

    if (bind(servidor, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind"); CLOSESOCK(servidor); SOCKCLEAN(); return 1;
    }

    if (listen(servidor, 5) < 0) {
        perror("listen"); CLOSESOCK(servidor); SOCKCLEAN(); return 1;
    }

    printf("Servidor escutando na porta %d...\n", PORTA);

    No *lista_musicas = carregar_musicas_de_arquivo(NOME_ARQUIVO_DADOS);

    while (1) {
        struct sockaddr_in cliente_addr;
        socklen_t len = sizeof(cliente_addr);
        socket_t cliente = accept(servidor, (struct sockaddr*)&cliente_addr, &len);
        if (cliente < 0) { perror("accept"); continue; }

        printf("Cliente conectado!\n");

        char buffer[256]; 
        int n;

        while ((n = recv(cliente, buffer, sizeof(buffer) - 1, 0)) > 0) {
            buffer[n] = '\0'; 
            printf("Comando recebido: %s\n", buffer);

            if (strncmp(buffer, CMD_READ, strlen(CMD_READ)) == 0) {
                No *atual = lista_musicas;
                int count = 0;
                while(atual != NULL) {
                    count++;
                    atual = atual->proximo;
                }

                unsigned int total_musicas = htonl(count);
                send(cliente, (char*)&total_musicas, sizeof(total_musicas), 0);

                atual = lista_musicas;
                while(atual != NULL) {
                    send(cliente, (char*)&(atual->musica), sizeof(Musica), 0);
                    atual = atual->proximo;
                }
                printf("Enviadas %d músicas para o cliente.\n", count);

            } else if (strncmp(buffer, CMD_DELETE, strlen(CMD_DELETE)) == 0) {
                char *titulo_delete = buffer + strlen(CMD_DELETE) + 1; 
                
                No *old_lista = lista_musicas;
                lista_musicas = remover_musica(lista_musicas, titulo_delete);
                if (old_lista != lista_musicas) {
                    send(cliente, "OK\n", 3, 0);
                } else {
                    send(cliente, "ERRO\n", 5, 0);
                }
            } else {
                send(cliente, "COMANDO_INVALIDO\n", 17, 0);
            }
        }

        printf("Cliente desconectado.\n");
        CLOSESOCK(cliente);
    }

    salvar_musicas_para_arquivo(lista_musicas, NOME_ARQUIVO_DADOS);
    liberar_lista(lista_musicas);
    CLOSESOCK(servidor);
    SOCKCLEAN();
    
    printf("Servidor encerrado.\n");
    return 0;
}


