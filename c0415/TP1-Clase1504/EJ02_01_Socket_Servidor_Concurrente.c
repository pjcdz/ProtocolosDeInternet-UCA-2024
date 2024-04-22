#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

// sudo ./EJ02_01_Socket_Servidor_Concurrente

#define MAX 1024
#define PORT 6667
#define BUFFER_SIZE 1024

double tiempo_transcurrido(struct timespec *inicio, struct timespec *fin) {
    return (fin->tv_sec - inicio->tv_sec) * 1e9 + (fin->tv_nsec - inicio->tv_nsec);
}

long factorial(int n) {
    if (n == 0)
        return 1;
    else
        return n * factorial(n-1);
}

// EJ02_01_Socket_Servidor_Concurrente.c
void proceso_hijo(int sock) {
    char buffer[BUFFER_SIZE];
    struct timespec start, end;
    int buff;

    for (int num = 1; num <= 5; ++num) {
        // Leer el número enviado por el cliente
        read(sock, buffer, BUFFER_SIZE);
        buff = atoi(buffer);

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
        write(sock, response, sizeof(response));
    }
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server, client;
    socklen_t client_len = sizeof(client);
    int opt = 1;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("No se pudo crear el socket");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("Falla en setsockopt");
        exit(EXIT_FAILURE);
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Falla en bind");
        exit(EXIT_FAILURE);
    }
    listen(server_sock, 3);

    printf("Socket en modo escucha\n");
    while ((client_sock = accept(server_sock, (struct sockaddr *)&client, &client_len))) {
        printf("Conexión establecida con %s\n", inet_ntoa(client.sin_addr));
        int pid = fork();
        if (pid < 0) {
            perror("No se pudo crear el proceso");
            exit(EXIT_FAILURE);
        }
        if (pid == 0) { // Proceso hijo
            close(server_sock);
            proceso_hijo(client_sock);
            exit(0);
        } else { // Proceso padre
            close(client_sock);
        }
    }
    if (client_sock < 0) {
        perror("Falla en accept");
        exit(EXIT_FAILURE);
    }
    return 0;
}