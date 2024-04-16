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

void proceso_hijo(int sock) {
    char buffer[BUFFER_SIZE];
    struct timespec start, end;

    while (1) {
        int bytes_recibidos = recv(sock, buffer, BUFFER_SIZE, 0);
        if (bytes_recibidos > 0) {
            buffer[bytes_recibidos] = '\0';
            int num = atoi(buffer);
            long fact = factorial(num);
            // Formatear el mensaje para incluir el tiempo de respuesta
            clock_gettime(CLOCK_MONOTONIC, &start);
            sprintf(buffer, "%ld", fact);
            write(sock, buffer, strlen(buffer));
            clock_gettime(CLOCK_MONOTONIC, &end);
            double tiempo_t = tiempo_transcurrido(&start, &end);
            sprintf(buffer, "%f", tiempo_t);
            write(sock, buffer, strlen(buffer));
        } else if (bytes_recibidos == 0) {
            printf("No hay más datos\n");
            break;
        } else {
            perror("recv");
            break;
        }
    }

    close(sock);
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