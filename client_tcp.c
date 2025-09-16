#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _WIN32_WINNT 0x0600 
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
typedef SOCKET socket_t;
#define CLOSESOCK closesocket
#define SOCKINIT()  do { WSADATA wsa; WSAStartup(MAKEWORD(2,2), &wsa); } while (0)
#define SOCKCLEAN() WSACleanup()

#include "musica.h"

#define BUF_SIZE 512

// Comandos do protocolo
#define CMD_READ "READ"
#define CMD_DELETE "DELETE"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <IP> <porta>\n", argv[0]);
        return 1;
    }

    const char *ip = argv[1];
    int porta = atoi(argv[2]);

    SOCKINIT();

    socket_t sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { 
      perror("socket"); 
      return 1; 
    }

    struct sockaddr_in srv = {0};
    srv.sin_family = AF_INET;
    srv.sin_port = htons(porta);
    srv.sin_addr.s_addr = inet_addr(ip);  // Converte string para binário (IPv4)

    if (connect(sock, (struct sockaddr*)&srv, sizeof(srv)) < 0) {
        perror("connect"); 
        return 1;
    }

    int opcao;
    char titulo_delete[128];

    while (1) {
        printf("\nMenu Cliente:\n1) Listar Músicas\n2) Remover Música\n0) Sair\nEscolha: ");
        if (scanf("%d", &opcao) != 1) break;
        getchar(); // consome newline

        if (opcao == 0) {
            break; // encerra conexão
        } else if (opcao == 1) {
            // Envia comando READ
            send(sock, CMD_READ, strlen(CMD_READ), 0);

            // Recebe número de registros
            unsigned int total_musicas = 0;
            int n = recv(sock, (char*)&total_musicas, sizeof(total_musicas), 0);
            if (n <= 0) { printf("Erro ao receber total de músicas.\n"); break; }
            total_musicas = ntohl(total_musicas);

            printf("\n--- Registros recebidos (%u) ---\n", total_musicas);
            for (unsigned int i = 0; i < total_musicas; i++) {
                Musica musica_recebida;
                n = recv(sock, (char*)&musica_recebida, sizeof(Musica), 0);
                if (n <= 0) { printf("Erro ao receber música.\n"); break; }
                printf("%u) Título: %s, Artista: %s, Álbum: %s, Ano: %d\n", i+1, 
                       musica_recebida.titulo, musica_recebida.artista, 
                       musica_recebida.album, musica_recebida.ano);
            }

        } else if (opcao == 2) {
            printf("Digite o título da música a ser removida: ");
            fgets(titulo_delete, sizeof(titulo_delete), stdin);
            titulo_delete[strcspn(titulo_delete, "\n")] = 0; // Remover newline

            char comando_completo[256];
            snprintf(comando_completo, sizeof(comando_completo), "%s %s", CMD_DELETE, titulo_delete);
            send(sock, comando_completo, strlen(comando_completo), 0);

            char resposta[BUF_SIZE];
            int n = recv(sock, resposta, sizeof(resposta) - 1, 0);
            if (n > 0) {
                resposta[n] = '\0';
                printf("Resposta do servidor: %s\n", resposta);
            } else {
                printf("Erro ao receber resposta do servidor.\n");
            }
        }
    }

    CLOSESOCK(sock);
    SOCKCLEAN();
    printf("Desconectado.\n");
    return 0;
}


