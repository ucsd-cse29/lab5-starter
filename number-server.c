#include "http-server.h"
#include <string.h>

char const HTTP_200_OK[] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";
char const HTTP_404_NOT_FOUND[] = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\n";

int global_num = 0; // Persistent server state

void handle_404(int client_sock, char *path)  {
    printf("SERVER LOG: Got request for unrecognized path \"%s\"\n", path);

    char response_buff[BUFFER_SIZE];
    snprintf(response_buff, BUFFER_SIZE, "Error 404:\r\nUnrecognized path \"%s\"\r\n", path);
    // snprintf includes a null-terminator

    write(client_sock, HTTP_404_NOT_FOUND, strlen(HTTP_404_NOT_FOUND));
    write(client_sock, response_buff, strlen(response_buff));
}

void handle_shownum(int client_sock) {
    char response_buff[BUFFER_SIZE];
    snprintf(response_buff, BUFFER_SIZE, "Your number is %d\r\n", global_num);
    // snprintf always null-terminates

    write(client_sock, HTTP_200_OK, strlen(HTTP_200_OK));
    write(client_sock, response_buff, strlen(response_buff));
}

void handle_response(char *request, int client_sock) {
    char path[256];

    printf("\nSERVER LOG: Got request \"%s\"\n", request);

    // Parse the path out of the request line (limit buffer size; sscanf null-terminates)
    if (sscanf(request, "GET %255s", path) != 1) {
        printf("Invalid request line\n");
        return;
    }
    printf("SERVER LOG: path is \"%s\"\n", path);

    // show number path
    if (strncmp(path, "/shownum", 8)) { // if path starts with shownum
        handle_shownum(client_sock);
    } else {
        handle_404(client_sock, path);
    }
}

int main(int argc, char *argv[]) {
    int port = 0;
    if(argc >= 2) {
        port = atoi(argv[1]);
    }

    start_server(&handle_response, port);
}
