all: number-server
  
number-server: number-server.c
    gcc -std=c11 -Wall -Wno-unused-variable -fsanitize=address -g number-server.c -o number-server

clean:
    rm -f number-server
