#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "musica.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
typedef SOCKET socket_t;
#define CLOSESOCK closesocket
#define SOCKINIT() do { WSADATA wsa; WSAStartup(MAKEWORD(2, 2), &wsa); } while (0)
#define SOCKCLEAN() WSACleanup()

#define PORT_HTTP 8080
#define BUF_SIZE 2048
#define NOME_ARQUIVO_DADOS "musicas.bin"

// Função para gerar o HTML com a lista de músicas
char* gerar_html_lista_musicas(No *lista) {
    char *html = (char*)malloc(BUF_SIZE * sizeof(char));
    if (html == NULL) {
        perror("Erro ao alocar memória para HTML");
        return NULL;
    }
    strcpy(html, "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n");
    strcat(html, "<html><head><title>Gerenciador de Músicas</title></head><body>");
    strcat(html, "<h1>Músicas Cadastradas</h1>");
    strcat(html, "<table border=\'1\'><tr><th>Título</th><th>Artista</th><th>Álbum</th><th>Ano</th><th>Ação</th></tr>");

    No *atual = lista;
    while (atual != NULL) {
        char row[256];
        snprintf(row, sizeof(row), "<tr><td>%s</td><td>%s</td><td>%s</td><td>%d</td><td><a href=\\'/delete/%s\\'>Remover</a></td></tr>",
                 atual->musica.titulo, atual->musica.artista, atual->musica.album, atual->musica.ano, atual->musica.titulo);
        strcat(html, row);
        atual = atual->proximo;
    }
    strcat(html, "</table>");
    strcat(html, "</body></html>");
    return html;
}

int main() {
    SOCKINIT();

    socket_t server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&(int){1}, sizeof(int));

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT_HTTP);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        CLOSESOCK(server_fd);
        SOCKCLEAN();
        return 1;
    }

    if (listen(server_fd, 5) < 0) {
        perror("listen");
        CLOSESOCK(server_fd);
        SOCKCLEAN();
        return 1;
    }

    printf("Servidor HTTP escutando em http://<seu-ip>:%d\n", PORT_HTTP);

    No *lista_musicas = carregar_musicas_de_arquivo(NOME_ARQUIVO_DADOS);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        socket_t client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        char buffer[BUF_SIZE];
        int bytes_received = recv(client_fd, buffer, BUF_SIZE - 1, 0);
        if (bytes_received <= 0) {
            CLOSESOCK(client_fd);
            continue;
        }
        buffer[bytes_received] = '\0';

        printf("Requisição HTTP recebida:\n%s\n", buffer);

        char method[16], path[256];
        sscanf(buffer, "%s %s", method, path);

        char *response_html = NULL;
        char response_header[256];
        int status_code = 200;

        if (strcmp(path, "/") == 0) {
            response_html = gerar_html_lista_musicas(lista_musicas);
        } else if (strncmp(path, "/delete/", strlen("/delete/")) == 0) {
            char *titulo_delete = path + strlen("/delete/");
            
            No *old_lista = lista_musicas;
            lista_musicas = remover_musica(lista_musicas, titulo_delete);
            salvar_musicas_para_arquivo(lista_musicas, NOME_ARQUIVO_DADOS); 

            strcpy(response_header, "HTTP/1.1 302 Found\r\nLocation: /\r\n\r\n");
            send(client_fd, response_header, strlen(response_header), 0);
            CLOSESOCK(client_fd);
            continue;
        } else {
            status_code = 404;
            response_html = (char*)malloc(BUF_SIZE * sizeof(char));
            if (response_html) {
                strcpy(response_html, "HTTP/1.1 404 Not Found\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n");
                strcat(response_html, "<html><body><h1>404 - Página Não Encontrada</h1></body></html>");
            }
        }

        if (response_html) {
            if (status_code == 200) {
                send(client_fd, response_html, strlen(response_html), 0);
            } else if (status_code == 404) {
                send(client_fd, response_html, strlen(response_html), 0);
            }
            free(response_html);
        }

        CLOSESOCK(client_fd);
    }

    salvar_musicas_para_arquivo(lista_musicas, NOME_ARQUIVO_DADOS);
    liberar_lista(lista_musicas);
    CLOSESOCK(server_fd);
    SOCKCLEAN();

    return 0;
}


