#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 6667
#define BUFFER_SIZE 1024
#define NUM_CONNECTIONS 5

double tiempo_transcurrido(struct timespec *inicio, struct timespec *fin) {
    return (fin->tv_sec - inicio->tv_sec) + (fin->tv_nsec - inicio->tv_nsec) / 1e9;
}

int main() {
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    struct timespec start, end;
    double tiempo_t;

    for (int i = 0; i < NUM_CONNECTIONS; ++i) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) { // Proceso hijo
            printf("PID del proceso hijo: %d\n", getpid());
            int sock = socket(AF_INET, SOCK_STREAM, 0);
            if (sock < 0) {
                perror("socket");
                exit(EXIT_FAILURE);
            }

            memset(&serv_addr, '0', sizeof(serv_addr));
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(PORT);

            if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
                printf("\nDirección Inválida\n");
                return -1;
            }

            if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
                printf("\nFalla en la conexión \n");
                return -1;
            }

            for (int num = 1; num <= 5; ++num) {
                char mensaje[10];
                sprintf(mensaje, "%d", num);
                send(sock, mensaje, strlen(mensaje), 0);

                int bytes_recibidos = recv(sock, buffer, BUFFER_SIZE, 0);
                if (bytes_recibidos > 0) {
                    buffer[bytes_recibidos] = '\0';
                    printf("Respuesta del servidor: %s\n", buffer);
                } else if (bytes_recibidos == 0) {
                    printf("Conexión cerrada por el servidor\n");
                } else {
                    perror("recv");
                }
            }

            close(sock);
            exit(EXIT_SUCCESS);
        } else {
            wait(NULL);
            sleep(2);
        }
    }

    return 0;
}