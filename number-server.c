#include "http-server.h"
#include <string.h>

int num = 0; // the state of the server (the hello world version of chats list)

char const HTTP_404_NOT_FOUND[] = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\n";
char const HTTP_200_OK[] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";

void handle_404(int client_sock, char *path)  {
    printf("SERVER LOG: Got request for unrecognized path \"%s\"\n", path);

    char response_buff[BUFFER_SIZE];
    snprintf(response_buff, BUFFER_SIZE, "Error 404:\r\nUnrecognized path \"%s\"\r\n", path);

    write(client_sock, HTTP_404_NOT_FOUND, strlen(HTTP_404_NOT_FOUND));
    write(client_sock, response_buff, strlen(response_buff));
}

void handle_root(int client_sock) {
  char message[BUFFER_SIZE];
  snprintf(message, BUFFER_SIZE, "Current number: %d\n", num);
  write(client_sock, HTTP_200_OK, strlen(HTTP_200_OK));  
  write(client_sock, message, strlen(message));
}

void handle_increment(int client_sock) {
  char message[BUFFER_SIZE];
  num += 1;
  snprintf(message, BUFFER_SIZE, "Number incremented to: %d\n", num);
  write(client_sock, HTTP_200_OK, strlen(HTTP_200_OK));  
  write(client_sock, message, strlen(message));
}

char addprefix[] = "/add?value=";

void handle_add(int client_sock, char* path) {
  // Assume: path looks like "/add?value=________"
  // Q: What do atoi() strtol() do?
  // A: Convert strings like '123' into integers like 123
  // Q: What do atoi() strtol() do on non-integer data

  // sscanf is a whole other approach

  // char* start_of_num = path + strlen(addprefix);

  char* start_of_num = strstr(path, "=") + 1;
  int toadd = atoi(start_of_num);
  num += toadd; 

  char message[BUFFER_SIZE];
  snprintf(message, BUFFER_SIZE, "Added %d, got: %d\n", toadd, num);
  write(client_sock, HTTP_200_OK, strlen(HTTP_200_OK));  
  write(client_sock, message, strlen(message));
}

void handle_response(char *request, int client_sock) {
    char path[256];

    printf("\nSERVER LOG: Got request: \"%s\"\n", request);

    // Parse the path out of the request line (limit buffer size; sscanf
    // null-terminates)
    if (sscanf(request, "GET %255s", path) != 1) {
        printf("Invalid request line\n");
        return;
    }

    /* "/" : shows "Current number: ____"
       "/increment" : adds 1 to number and shows "Incremented to: _____"
       "/add?value=N" : adds N to number and shows "Added N, got: _____"
    */
    // How to write the if statements to detect which path we have?
    if(strcmp(path, "/") == 0) {
      handle_root(client_sock);
      return;
    }
    else if(strcmp(path, "/increment") == 0) {
      handle_increment(client_sock);
      return;
    }
    else if(strncmp(path, "/add?value=", strlen("/add?value=")) == 0) {
      handle_add(client_sock, path);
      return;
    }
    else {
      handle_404(client_sock, path);
    }

    // strstr if there might be shared prefixes, like looking for "/post" in
    // the PA save strstr for later

}

int main(int argc, char *argv[]) {
    int port = 0;
    if(argc >= 2) { // if called with a port number, use that
        port = atoi(argv[1]);
    }

    start_server(&handle_response, port);
}
