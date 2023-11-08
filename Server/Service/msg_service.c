#include "msg_service.h"
#include "../Common/persistence.h"

void send_message(const char *buffer) {
    cJSON *json_obj = cJSON_Parse(buffer);
    cJSON *data_obj = cJSON_GetObjectItem(json_obj, "data");
    if (json_obj == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            printf("Error: %s\n", error_ptr);
        }
        cJSON_Delete(json_obj);
        return ;
    }

    cJSON *from_uid, *to_uid, *msg_content;
    from_uid = cJSON_GetObjectItemCaseSensitive(data_obj, "from_uid");
    to_uid = cJSON_GetObjectItemCaseSensitive(data_obj, "to_uid");
    msg_content = cJSON_GetObjectItemCaseSensitive(data_obj, "msg_content");

    if (!cJSON_IsNumber(from_uid)) {
        return ;
    } else if (!cJSON_IsNumber(to_uid)) {
        return ;
    } else if (!cJSON_IsString(msg_content) || (msg_content->valuestring == NULL)) {
        return ;
    }

    int index = hash_function(to_uid->valueint);
    struct Client *current_client = clients[index];
    while (current_client != NULL) {
        if (current_client->uid == to_uid->valueint) {
            send(current_client->sockfd, msg_content->valuestring, strlen(msg_content->valuestring), 0);
            break;
        } else {
            printf("Invalid uid");
            break;
        }
        current_client = current_client->next;
    }
}