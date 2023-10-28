#include <stdio.h>
#include <cjson/cJSON.h>

int main() {
    FILE *fp = fopen("test.json", "r");
    if (fp == NULL) {
        printf("ERROR");
        return 1;
    }

    char buffer[1024];
    int len = fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);

    cJSON *json = cJSON_Parse(buffer);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            printf("Error: %s\n", error_ptr);
        }
        cJSON_Delete(json);
        return 1;
    }

    cJSON *name = cJSON_GetObjectItemCaseSensitive(json, "name");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        printf("Name: %s\n", name->valuestring);
    }

    cJSON *age = cJSON_GetObjectItemCaseSensitive(json, "age");
    if (cJSON_IsNumber(age)) {
        printf("Age: %d\n", age->valueint);
    }

    cJSON *email = cJSON_GetObjectItemCaseSensitive(json, "email");
    if (cJSON_IsString(email)) {
        printf("Email Address: %s\n", email->valuestring);
    }

    cJSON_Delete(json);
    return 0;
}