#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define PORT 6667
#define BUFFER_SIZE 1024

long factorial(int n) {
    if (n == 0)
        return 1;
    else
        return n * factorial(n-1);
}

void *proceso_hijo(void *socket_desc) {
    printf("PID del proceso hijo: %d\n", getpid());
    printf("ID del hilo: %lu\n", pthread_self());
    int sock = *(int*)socket_desc;
    char buffer[BUFFER_SIZE];
    int num;
    long fact;
    struct timespec start, end;
    double tiempo_t;

    while (1) {
        bzero(buffer, BUFFER_SIZE);
        read(sock, buffer, BUFFER_SIZE);
        if (!strlen(buffer)) {
            printf("No hay más datos\n");
            break;
        }
        num = atoi(buffer);
        printf("Número recibido: %d\n", num);
        fact = factorial(num);
        sprintf(buffer, "%ld", fact);
        clock_gettime(CLOCK_MONOTONIC, &start);
        write(sock, buffer, strlen(buffer));
        clock_gettime(CLOCK_MONOTONIC, &end);
        tiempo_t = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        printf("Tiempo de respuesta: %f segundos\n", tiempo_t);
    }
    close(sock);
    free(socket_desc);
    pthread_exit(NULL);
}

int main() {
    int server_sock, client_sock, *new_sock;
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
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;
        if (pthread_create(&sniffer_thread, NULL, proceso_hijo, (void*)new_sock) < 0) {
            perror("No se pudo crear el hilo");
            exit(EXIT_FAILURE);
        }
    }
    if (client_sock < 0) {
        perror("Falla en accept");
        exit(EXIT_FAILURE);
    }
    return 0;
}