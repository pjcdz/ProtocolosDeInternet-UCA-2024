#include <arpa/inet.h> // inet_addr()
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#include <time.h> // clock_gettime()
#include <sys/types.h> // pid_t

// sudo ./EJ01_01_Socket_Servidor_Concurrente

#define MAX 1024
#define PORT 6667
#define SA struct sockaddr

double tiempo_transcurrido(struct timespec *inicio, struct timespec *fin) {
    return (fin->tv_sec - inicio->tv_sec) * 1e9 + (fin->tv_nsec - inicio->tv_nsec);
}

void func(int* sockfd) {
    struct timespec start, end;
    int connfd = *sockfd;
    int buff;
    for (;;) {
        int numBytes = read(connfd, &buff, sizeof(int));
        if (numBytes <= 0) {
            printf("El cliente ha cerrado la conexión.\n");
            break;
        }
        printf("Número recibido: %d\n", buff);

        // Calcular el factorial
        clock_gettime(CLOCK_MONOTONIC, &start);
        int factorial = 1;
        for(int i = 1; i <= buff; ++i) {
            factorial *= i;
        }
        clock_gettime(CLOCK_MONOTONIC, &end);

        // Obtener el tiempo de ejecución
        double execution_time = tiempo_transcurrido(&start, &end);

        // Enviar la respuesta al cliente
        char response[MAX];
        sprintf(response, "Factorial de %d: %d\nTiempo de ejecución: %.0f nanosegundos\nPID del proceso hijo: %d\n", buff, factorial, execution_time, getpid());
        write(connfd, response, sizeof(response));

        if (strncmp("SALIR", response, 4) == 0) {
            printf("Salgo del servidor...\n");
            break;
        }
    }
    close(connfd);
    exit(0);
}

int main() {
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Falla la creación del socket...\n");
        exit(0);
    }
    else
        printf("Socket creado...\n");
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("Falla socket bind ...\n");
        exit(0);
    }
    else
        printf("Se hace el socket bind ..\n");

    if ((listen(sockfd, 5)) != 0) {
        printf("Falla el listen ...\n");
        exit(0);
    }
    else
        printf("Servidor en modo escucha ...\n");

    while (1) {
        unsigned int len = sizeof(cli);
        connfd = accept(sockfd, (SA*)&cli, &len);
        if (connfd < 0) {
            printf("Falla al aceptar datos en el servidor...\n");
            exit(0);
        }
        else
            printf("El servidor acepta al cliente ...\n");

        int pid = fork();
        if (pid < 0) {
            printf("Falla al crear el proceso...\n");
        }
        else if (pid == 0) { // Proceso hijo
            close(sockfd); // Cerrar el descriptor de archivo del servidor en el proceso hijo
            func(&connfd);
        }
        else { // Proceso padre
            char response[MAX];
            sprintf(response, "PID del proceso padre: %d\n", getpid());
            write(connfd, response, sizeof(response));
            close(connfd); // Cerrar el socket en el proceso padre
        }
    }

    close(sockfd);
}