#include "http-server.h"
#include <string.h>

int num = 0;

char const HTTP_404_NOT_FOUND[] = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\n";
char const HTTP_200_OK[] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";

void handle_404(int client_sock, char *path)  {
    printf("SERVER LOG: Got request for unrecognized path \"%s\"\n", path);

    char response_buff[BUFFER_SIZE];
    snprintf(response_buff, BUFFER_SIZE, "Error 404:\r\nUnrecognized path \"%s\"\n", path);
    // snprintf includes a null-terminator

    // write header, then respond with body
    write(client_sock, HTTP_404_NOT_FOUND, strlen(HTTP_404_NOT_FOUND));
    write(client_sock, response_buff, strlen(response_buff));
}


void handle_shownum(int client_sock) {
    printf("SERVER LOG: Got shownum\n");

    char response_buff[BUFFER_SIZE];
    snprintf(response_buff, BUFFER_SIZE, "Your number is: %d\n", num);

    // write header, then respond with body
    write(client_sock, HTTP_200_OK, strlen(HTTP_200_OK));
    write(client_sock, response_buff, strlen(response_buff));
}

void handle_increment(int client_sock) {
    printf("SERVER LOG: Got increment: adding 1 to %d\n", num);

    num += 1;

    char response_buff[BUFFER_SIZE];
    snprintf(response_buff, BUFFER_SIZE, "Adding 1. Your number is now: %d\n", num);

    // write header, then respond with body
    write(client_sock, HTTP_200_OK, strlen(HTTP_200_OK));
    write(client_sock, response_buff, strlen(response_buff));
}


// path is like: "/add?value=N", where N is an integer
void handle_add(int client_sock, char *path) {
    printf("SERVER LOG: got add: path = \"%s\"\n", path);

    int value;
    if (sscanf(path, "/add?value=%d", &value) != 1) {
        printf("SERVER LOG: ERROR, scanf failed to parse value from path\n");
        // TODO: Respond to client with error code
        return;
    }

    num += value;
    char response_buff[BUFFER_SIZE];
    snprintf(response_buff, BUFFER_SIZE, "Added %d. Number is now %d\n", value, num);

    write(client_sock, HTTP_200_OK, strlen(HTTP_200_OK));
    write(client_sock, response_buff, strlen(response_buff));
}

void handle_response(char *request, int client_sock) {
    char path[256];

    printf("\nSERVER LOG: Got request: \"%s\"\n", request);

    // Parse the path out of the request line (limit buffer size; sscanf null-terminates)
    if (sscanf(request, "GET %255s", path) != 1) {
        printf("Invalid request line\n");
        return;
    }

    // Examine path to decide what to do
    if ( strcmp(path, "/shownum") == 0) {
        handle_shownum(client_sock);
    } else if ( strcmp(path, "/increment") == 0 ) {
        handle_increment(client_sock);
    } else if ( strncmp(path, "/add", 4) == 0 ) {
        handle_add(client_sock, path);
    } else {
        handle_404(client_sock, path);
    }
}

int main(int argc, char *argv[]) {
    int port = 0;
    if(argc >= 2) { // if called with a port number, use that
        port = atoi(argv[1]);
    }

    start_server(&handle_response, port);
}
