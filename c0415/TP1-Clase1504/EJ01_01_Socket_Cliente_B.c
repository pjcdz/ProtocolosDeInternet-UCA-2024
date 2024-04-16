#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // read(), write(), close()
#include <string.h> // bzero()
#include <sys/socket.h>
#include <netinet/in.h> // struct sockaddr_in
#include <arpa/inet.h> // inet_addr()

// sudo ./EJ01_01_Socket_Cliente_B

#define MAX 1024
#define PORT 6667
#define SA struct sockaddr

void func(int sockfd) {
    char buffer[MAX] = {0};
    int leovalor;

    for (int i = 1; i <= 5; i++) {  // Cambiado de 10 a 5
        // Enviar número al servidor
        write(sockfd, &i, sizeof(int));

        // Leer respuesta del servidor
        leovalor = read(sockfd, buffer, MAX);
        printf("%s\n", buffer);

        printf("Enviado: %d - ", i);
    }

    // Leer respuesta final del servidor
    leovalor = read(sockfd, buffer, MAX);
    printf("%s\n", buffer);
}

int main() {
    int sockfd;
    struct sockaddr_in servaddr;

    // socket: creo socket y lo verifico
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Falla la creación del socket...\n");
        exit(0);
    }
    else
        printf("Socket creado ..\n");
    bzero(&servaddr, sizeof(servaddr));

    // asigno IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    // Conecto los sockets entre cliente y servidor
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("Falla de conexión con servidor...\n");
        exit(0);
    }
    else
        printf("Conectado al servidor..\n");

    // Función para el chat
    func(sockfd);

    //Cierro el socket
    close(sockfd);
}