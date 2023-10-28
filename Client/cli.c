#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int client_sockfd;
    struct sockaddr_in server_addr;

    client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sockfd < 0) {
        perror("Error in socket creation");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(36010);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(client_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error in connection");
        exit(EXIT_FAILURE);
    }

    char buffer[1024];
    int recv_bytes = recv(client_sockfd, buffer, sizeof(buffer), 0);
    if (recv_bytes > 0) {
        buffer[recv_bytes] = '\0';
        printf("Server: %s\n", buffer);
    }

    while (1) {
        printf("Enter a command (or 'quit' to exit): ");
        fgets(buffer, sizeof(buffer), stdin);

        send(client_sockfd, buffer, strlen(buffer), 0);

        recv_bytes = recv(client_sockfd, buffer, sizeof(buffer), 0);
        if (recv_bytes <= 0) {
            printf("Server disconnected.\n");
            break;
        }
        buffer[recv_bytes] = '\0';
        printf("Server: %s\n", buffer);

        if (strncmp(buffer, "quit", 4) == 0) {
            printf("Exiting...\n");
            break;
        }
    }

    close(client_sockfd);

    return 0;
}
