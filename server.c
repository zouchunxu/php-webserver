#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVER_STRING "Server：jdbhttpd/0.1.0\r\n"
#define STRING "helllo world"


void headers(int client, int n) {
    char buf[1024];

    strcpy(buf, "HTTP/1.0 200 OK\r\n");
    send(client, buf, strlen(buf), 0);
    strcpy(buf, SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Length: %d\r\n", n);
    send(client, buf, strlen(buf), 0);
    strcpy(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
}

void get_line(int socket, char buf[]) {
    char c;
    int bool = 1;
    int i = 0;
    while (recv(socket, &c, 1, 0) > 0 && bool) {
        if (c == '\r') {
            recv(socket, &c, 1, MSG_PEEK);
            if (c == '\n') {
                bool = 0;
            }
        }
        buf[i++] = c;
    }
}


int main() {

    int httpd, http_len;
    httpd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(5151);
    http_len = sizeof(server);
    bind(httpd, (struct sockaddr *) &server, sizeof(server));


    printf("port:%d\n", ntohs(server.sin_port));
    listen(httpd, 5);

    char buf[1024];
    struct sockaddr_in client;
    int client_sock = -1, client_name_len;
    client_name_len = sizeof(client);

    int pid, status;
    while (1) {
        client_sock = accept(httpd, (struct sockaddr *) &client, (socklen_t *)&client_name_len);

        int output[2], input[2];
        pipe(output);

        char header[1024];
        get_line(client_sock, header);
//        printf("%s\n", header);
        char *method = strtok(header, " ");
        char *token = strtok(NULL, " ");
        if (strlen(token) <= 1 || strcmp(method, "GET") != 0) {
            token = "/test.php";
        }

//        printf("file:%s\n", token);

        char path[255] = "/Users/zouchunxu/c-webserver/htdocs";
        pid = fork();
        if (pid == 0) {
            dup2(output[1], 1);
            execl("/usr/local/bin/php", "php", strcat(path, token), NULL);
            exit(0);
        } else {
            int len = 1024;
            char c[len];
            read(output[0], &c, len);

            headers(client_sock, strlen(c));
            send(client_sock, c, strlen(c), 0);

            close(output[0]);
            printf("c:%s\n", c);
            waitpid(pid, &status, 0);
        }


    }


    return 0;
}
