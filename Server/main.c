#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <cjson/cJSON.h>

// Net
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "Common/utils.h"
#include "Common/db.h"

enum Command {
    LIST,
    SEND_MSG,
    UNKNOWN
};

struct CommandMap {
    const char *commandStr;
    enum Command commandId;
};

struct CommandMap commandTable[] = {
    {"LIST", LIST},
    {"SEND_MSG", SEND_MSG},
    {"UNKNOWN", UNKNOWN}
};

enum Command getCommandId(const char *command) {
    printf("Command: %s\n", command);
    for (int i = 0; i < sizeof(commandTable) / sizeof(commandTable[0]); ++i) {
        if (strcmp(command, commandTable[i].commandStr) == 0) {
            return commandTable[i].commandId;
        }
    }
    return UNKNOWN;
}

void handle_commands(const char *data_recvd, char returned_str[]) {
    char buffer[4096] = { 0 }, command[64] = { 0 };
    printf("data_recvd: %s", data_recvd);

    cJSON *json_obj = cJSON_Parse(data_recvd);
    if (json_obj == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            printf("Error: %s\n", error_ptr);
        }
        cJSON_Delete(json_obj);
        strncpy(returned_str, "UNKNOWN RESPONSE", sizeof("UNKNOWN RESPONSE") - 1);
        return ;
    }

    cJSON *command_recvd = cJSON_GetObjectItemCaseSensitive(json_obj, "command");
    if (cJSON_IsString(command_recvd) && (command_recvd->valuestring != NULL)) {
        printf("command_recvd: %s\n", command_recvd->valuestring);
        strncpy(command, command_recvd->valuestring, strlen(command_recvd->valuestring));
        command[strlen(command_recvd->valuestring)] = '\0';
    } else {
        printf("Failed To Parse Command\n");
        strncpy(returned_str, "UNKNOWN RESPONSE", sizeof("UNKNOWN RESPONSE") - 1);
        return ;
    }

    cJSON_Delete(json_obj);

    // strncpy(command, command_recvd, sizeof(command_recvd) - 1);
    trimString(command);
    enum Command commandId = getCommandId(command);

    switch (commandId)
    {
    case LIST:
        strncpy(returned_str, "LIST RESPONSE", sizeof("LIST RESPONSE") - 1);
        break;
    case SEND_MSG:
        strncpy(returned_str, "SEND_MSG RESPONSE", sizeof("SEND_MSG RESPONSE") - 1);
        break;
    case UNKNOWN:
        strncpy(returned_str, "UNKNOWN RESPONSE", sizeof("UNKNOWN RESPONSE") - 1);
        break;
    default:
        break;
    }
}

void* handle_client(void *client_socket_ptr) {
    int client_sockfd = *((int*)client_socket_ptr);
    char buffer[1024] = { 0 }, reply_message[2048] = { 0 };

    const char* welcome_message = "Hello, Welcome to the DARKPINES Server!";
    send(client_sockfd, welcome_message, strlen(welcome_message), 0);

    while (1) {
        int recv_bytes = recv(client_sockfd, buffer, sizeof(buffer), 0);
        if (recv_bytes <= 0) {
            break; // Client disconnect, quit the while loop
        }
        // buffer[recv_bytes] = '\0';

        handle_commands(buffer, reply_message);

        // const char* reply_message = "Server received your message!";
        send(client_sockfd, reply_message, strlen(reply_message), 0);
    }

    close(client_sockfd);
    free(client_socket_ptr);

    pthread_exit(NULL);
}

void loadConfig(const char *file_path, char *listen_address, int *listen_port) {
    char default_address[16] = "127.0.0.1", buffer[1024] = { 0 };
    int default_port = 36010;

    FILE *fp = fopen(file_path, "r");
    if (fp == NULL) {
        printf("ERROR: Failed to open config file");
        exit(0);
    }

    int len = fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);

    cJSON *json_obj = cJSON_Parse(buffer);
    if (json_obj == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            printf("Error: %s\n", error_ptr);
        }
        cJSON_Delete(json_obj);
        exit(0);
    }

    cJSON *address, *port;
    address = cJSON_GetObjectItemCaseSensitive(json_obj, "address");
    port = cJSON_GetObjectItemCaseSensitive(json_obj, "port");

    if (!cJSON_IsString(address) || address->valuestring == NULL) {
        strncpy(listen_address, default_address, sizeof(default_address) - 1);
    } else {
        strncpy(listen_address, address->valuestring, sizeof(default_address) - 1);
    }

    if (!cJSON_IsNumber(port)) {
        *listen_port = default_port;
    } else {
        *listen_port = port->valueint;
    }

    cJSON_Delete(json_obj);
}

void listen_at(const char *address, int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR: Failed to create socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;  // IPv4
    server_addr.sin_port = htons(port); // Port
    // inet_pton(AF_INET, address, &(server_addr.sin_addr)); // Address

    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("ERROR: Failed to bind socket");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, SOMAXCONN) < 0) {
        perror("ERROR: Failed to listen on socket");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        // int client_sockfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
        int* client_sockfd_ptr = malloc(sizeof(int));
        *client_sockfd_ptr = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);

        if (*client_sockfd_ptr < 0) {
            perror("ERROR: Failed to accept client connection");
            free(client_sockfd_ptr);
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, (void*)client_sockfd_ptr);
    }

    printf("Client connected\n");
    close(sockfd);
}

int main() {
    char listen_address[16];
    int listen_port;
    const char *config_path = "../config.json";
    const char *db_path = "../data.db";
    FILE *db_file;
    db_file = fopen(db_path, "r");

    loadConfig(config_path, listen_address, &listen_port);

    if (db_file != NULL) {
        fclose(db_file);
    } else {
        printf("DataBase is not created, initializing...\n");
        initDatabase();
    }

    printf("Listening on Address: %s, Port: %d\n", listen_address, listen_port);
    listen_at(listen_address, listen_port);

    return 0;
}
