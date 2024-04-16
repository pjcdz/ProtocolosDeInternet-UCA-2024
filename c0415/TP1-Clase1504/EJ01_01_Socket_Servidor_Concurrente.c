#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#include <pthread.h> // pthread_create(), pthread_join()
#include <math.h> // factorial calculation
#include <sys/types.h> // pid_t
#include <sys/wait.h> // waitpid()
#include <time.h> // time()

#define MAX 1024
#define PORT 6667
#define SA struct sockaddr

void* func(void* sockfd) {
    int connfd = *((int*)sockfd);
    char buff[MAX];
    int n;
    for (;;) {
        bzero(buff, sizeof(buff));
        int numBytes = read(connfd, buff, sizeof(buff));
        if (numBytes <= 0) {
            printf("El cliente ha cerrado la conexión.\n");
            break;
        }
        // Verificar si el mensaje es un número válido
        char *endptr;
        long num = strtol(buff, &endptr, 10);
        if (endptr == buff) {
            printf("Mensaje no válido recibido: %s\n", buff);
            continue;
        }
        printf("Número recibido: %ld\n", num);

        // Calcular el factorial
        time_t start_time = time(NULL);
        int factorial = 1;
        for(int i = 1; i <= num; ++i) {
            factorial *= i;
        }
        time_t end_time = time(NULL);

        // Obtener el tiempo de ejecución
        double execution_time = difftime(end_time, start_time);

        // Obtener los PID del proceso hijo y padre
        pid_t child_pid = getpid();
        pid_t parent_pid = getppid();

        // Enviar la respuesta al cliente
        sprintf(buff, "\nFactorial de %ld: %d\nTiempo de ejecución: %.2f segundos\nPID del proceso hijo: %d\nPID del proceso padre: %d\n", num, factorial, execution_time, child_pid, parent_pid);
        write(connfd, buff, sizeof(buff));

        if (strncmp("SALIR", buff, 4) == 0) {
            printf("Salgo del servidor...\n");
            break;
        }
    }
    close(connfd);
    pthread_exit(NULL);
}

int main() {
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;
    pthread_t thread_id;

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

        if (pthread_create(&thread_id, NULL, func, &connfd) != 0)
            printf("Falla al crear el hilo...\n");
        else
            printf("Hilo asignado al cliente...\n");
    }

    close(sockfd);
}