// gcc create.c -o create -I/opt/homebrew/Cellar/cjson/1.7.15/include -L/opt/homebrew/Cellar/cjson/1.7.15/lib -lcjson
#include <stdio.h>
#include <cjson/cJSON.h>

int main() {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "name", "John Doe");
    cJSON_AddNumberToObject(json, "age", 30);
    cJSON_AddStringToObject(json, "email", "john@example.com");

    char *json_str = cJSON_Print(json);

    printf("%s\n", json_str);
}