#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080
#define MAX_LINE_LENGTH 1024

char* status(const char *searchString) {
    FILE *file = fopen("../myanimelist.csv", "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return NULL;
    }

    // Allocate memory for the result string
    size_t resultSize = 1024; // Initial size, can be adjusted
    char *result = (char *)malloc(resultSize * sizeof(char));
    if (result == NULL) {
        printf("Memory allocation error!\n");
        fclose(file);
        return NULL;
    }
    result[0] = '\0'; // Initialize result string

    int count = 1;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        // Remove newline character if present
        line[strcspn(line, "\n")] = '\0';

        char *token = strtok(line, ",");
        int column = 1;
        char thirdColumn[MAX_LINE_LENGTH]; // To store content of the second column
        while (token != NULL) {
            if (column == 3) { // If this is the second column
                strcpy(thirdColumn, token); // Save content of the second column
            }
            if (column == 4) { // If this is the fourth column
                // Check if the content matches the search string
                if (strstr(token, searchString) != NULL) {
                    // Concatenate the content of the second column to result
                    snprintf(result + strlen(result), resultSize - strlen(result), "%d. %s\n", count, thirdColumn);
                    count++;
                    break; // No need to continue processing this line
                }
            }
            token = strtok(NULL, ",");
            column++;
        }
    }

    fclose(file);
    return result;
}

char* tampil() {
    FILE *file = fopen("../myanimelist.csv", "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return NULL;
    }

    // Allocate memory for the result string
    size_t resultSize = 1024; // Initial size, can be adjusted
    char *result = (char *)malloc(resultSize * sizeof(char));
    if (result == NULL) {
        printf("Memory allocation error!\n");
        fclose(file);
        return NULL;
    }
    result[0] = '\0'; // Initialize result string

    int count = 1;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        // Remove newline character if present
        line[strcspn(line, "\n")] = '\0';

        char *token = strtok(line, ",");
        int column = 1;
        char thirdColumn[MAX_LINE_LENGTH]; // To store content of the second column
        while (token != NULL) {
            if (column == 3) { // If this is the second column
                strcpy(thirdColumn, token); // Save content of the second column
            }
            if (column == 4) { // If this is the fourth column
                    // Concatenate the content of the second column to result
                    snprintf(result + strlen(result), resultSize - strlen(result), "%d. %s\n", count, thirdColumn);
                    count++;
                    break; // No need to continue processing this line
                }
            token = strtok(NULL, ",");
            column++;
        }
    }

    fclose(file);
    return result;
}

char* genre(const char *searchString) {
    FILE *file = fopen("../myanimelist.csv", "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return NULL;
    }

    // Allocate memory for the result string
    size_t resultSize = 1024; // Initial size, can be adjusted
    char *result = (char *)malloc(resultSize * sizeof(char));
    if (result == NULL) {
        printf("Memory allocation error!\n");
        fclose(file);
        return NULL;
    }
    result[0] = '\0'; // Initialize result string

    int count = 1;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        // Remove newline character if present
        line[strcspn(line, "\n")] = '\0';

        char *token = strtok(line, ",");
        int column = 1;
        char thirdColumn[MAX_LINE_LENGTH]; // To store content of the second column
        while (token != NULL) {
            if (column == 3) { // If this is the second column
                strcpy(thirdColumn, token); // Save content of the second column
            }
            if (column == 2) { // If this is the fourth column
                // Check if the content matches the search string
                if (strstr(token, searchString) != NULL) {
                    token = strtok(NULL, ",");
                    // Concatenate the content of the second column to result
                    snprintf(result + strlen(result), resultSize - strlen(result), "%d. %s\n", count, token);
                    count++;
                    break; // No need to continue processing this line
                }
            }
            token = strtok(NULL, ",");
            column++;
        }
    }

    fclose(file);
    return result;
}

char* hari(const char *searchString) {
    FILE *file = fopen("../myanimelist.csv", "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return NULL;
    }

    // Allocate memory for the result string
    size_t resultSize = 1024; // Initial size, can be adjusted
    char *result = (char *)malloc(resultSize * sizeof(char));
    if (result == NULL) {
        printf("Memory allocation error!\n");
        fclose(file);
        return NULL;
    }
    result[0] = '\0'; // Initialize result string

    int count = 1;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        // Remove newline character if present
        line[strcspn(line, "\n")] = '\0';

        char *token = strtok(line, ",");
        int column = 1;
        char thirdColumn[MAX_LINE_LENGTH]; // To store content of the second column
        while (token != NULL) {
            if (column == 3) { // If this is the second column
                strcpy(thirdColumn, token); // Save content of the second column
            }
            if (column == 1) { // If this is the fourth column
                // Check if the content matches the search string
                if (strstr(token, searchString) != NULL) {
                    for(int i = 0; i<2; i++){
                        token = strtok(NULL, ",");
                    }
                    snprintf(result + strlen(result), resultSize - strlen(result), "%d. %s\n", count, token);
                    count++;
                    break; // No need to continue processing this line
                }
            }
            token = strtok(NULL, ",");
            column++;
        }
    }

    fclose(file);
    return result;
}

int main(int argc, char const *argv[]) {

    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT , &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }


    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }


    valread = read( new_socket , buffer, 1024);

    // Remove the newline character from the input string
    if (buffer[strlen(buffer) - 1] == '\n')
        buffer[strlen(buffer) - 1] = '\0';

    char *firstWord = strtok(buffer, " ");
    char *restOfString = strtok(NULL, "");

    if(strcmp("status", firstWord) == 0){
        char* statres = status(restOfString);
        send(new_socket, statres, strlen(statres), 0);
        free(statres); // Free the memory allocated by status
    }else if(strcmp("tampilkan", firstWord) == 0){
        char* tampres = tampil();
         send(new_socket, tampres, strlen(tampres), 0);
        free(tampres);
    }else if(strcmp("genre", firstWord) == 0){
        char* genreres = genre(restOfString);
         send(new_socket, genreres, strlen(genreres), 0);
        free(genreres);
    }else if(strcmp("hari", firstWord) == 0){
        char* harires = hari(restOfString);
         send(new_socket, harires, strlen(harires), 0);
        free(harires);
    }else{
        printf("what the fuck");
    }

}


