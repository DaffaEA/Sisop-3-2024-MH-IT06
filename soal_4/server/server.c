#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>

#define PORT 8080
#define MAX_LINE_LENGTH 1024

char* status(const char *searchString) {
    FILE *file = fopen("../myanimelist.csv", "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return NULL;
    }

    size_t resultSize = 1024;
    char *result = (char *)malloc(resultSize * sizeof(char));
    if (result == NULL) {
        printf("Memory allocation error!\n");
        fclose(file);
        return NULL;
    }
    result[0] = '\0';

    int count = 1;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        char *token = strtok(line, ",");
        int column = 1;
        while (token != NULL) {
            if (column == 3) {
                if (strstr(token, searchString) != NULL) {
                    token = strtok(NULL, ",");
                    snprintf(result + strlen(result), resultSize - strlen(result), "%s\n", token);
                    break;
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

    size_t resultSize = 1024;
    char *result = (char *)malloc(resultSize * sizeof(char));
    if (result == NULL) {
        printf("Memory allocation error!\n");
        fclose(file);
        return NULL;
    }
    result[0] = '\0';

    int count = 1;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        char *token = strtok(line, ",");
        int column = 1;
        while (token != NULL) {
            if (column == 3) {
                snprintf(result + strlen(result), resultSize - strlen(result), "%d. %s\n", count, token);
                count++;
                break;
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

    size_t resultSize = 1024;
    char *result = (char *)malloc(resultSize * sizeof(char));
    if (result == NULL) {
        printf("Memory allocation error!\n");
        fclose(file);
        return NULL;
    }
    result[0] = '\0';

    int count = 1;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        char *token = strtok(line, ",");
        int column = 1;
        while (token != NULL) {
            if (column == 2) {
                if (strstr(token, searchString) != NULL) {
                    token = strtok(NULL, ",");
                    snprintf(result + strlen(result), resultSize - strlen(result), "%d. %s\n", count, token);
                    count++;
                    break;
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

    size_t resultSize = 1024;
    char *result = (char *)malloc(resultSize * sizeof(char));
    if (result == NULL) {
        printf("Memory allocation error!\n");
        fclose(file);
        return NULL;
    }
    result[0] = '\0';

    fseek(file, 0, SEEK_SET);

    int count = 1;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        char *token = strtok(line, ",");
        int column = 1;
        while (token != NULL) {
            if (column == 1) {
                if (strstr(token, searchString) != NULL) {
                    for(int i = 0; i < 2; i++){
                        token = strtok(NULL, ",");
                    }
                    snprintf(result + strlen(result), resultSize - strlen(result), "%d. %s\n", count, token);
                    count++;
                    break;
                }
            }
            token = strtok(NULL, ",");
            column++;
        }
    }

    fclose(file);
    return result;
}

int findrow(const char *searchString) {
    FILE *file = fopen("../myanimelist.csv", "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return -1;
    }

    int row = 0;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        row++;
        line[strcspn(line, "\n")] = '\0';
        char *token = strtok(line, ",");
        int column = 1;
        while (token != NULL) {
            if (column == 3) {
                if (strstr(token, searchString) != NULL) {
                    fclose(file);
                    return row;
                }
            }
            token = strtok(NULL, ",");
            column++;
        }
    }

    fclose(file);
    return 0;
}

void deleteRow(const char *filename, int rowToDelete, const char* roww) {
    FILE *fp, *tempFile;
    char buffer[1024];
    int row = 0;

    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return;
    }

    tempFile = fopen("temp.csv", "w");
    if (tempFile == NULL) {
        printf("Error creating temporary file!\n");
        fclose(fp);
        return;
    }

    while (fgets(buffer, sizeof(buffer), fp)) {
        row++;

        if (row == rowToDelete)
            continue;

        fputs(buffer, tempFile);
    }

    fclose(fp);
    fclose(tempFile);

    remove(filename);

    rename("temp.csv", filename);

    FILE *logFile = fopen("../change.log", "a");
    if (logFile == NULL) {
        perror("Error opening log file");
        return;
    }

    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    fprintf(logFile, "[%02d/%02d/%02d] [DEL] %s berhasil dihapus.\n", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, roww);
    fclose(logFile);
}


void addRow(const char *filename, const char *rowData) {
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    fprintf(file, "\n%s", rowData);
    fclose(file);

    FILE *logFile = fopen("../change.log", "a");
    if (logFile == NULL) {
        perror("Error opening log file");
        return;
    }

    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    fprintf(logFile, "[%02d/%02d/%02d] [ADD] %s ditambahkan.\n", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, rowData);
    fclose(logFile);
}

#define MAX_COLS 100
#define MAX_ROW_LEN 1000

void editRow(const char *filename, int rowNumber, const char *rowData) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    char rows[MAX_COLS][MAX_ROW_LEN];
    int currentRow = 0;

    while (fgets(rows[currentRow], MAX_ROW_LEN, file) != NULL && currentRow < MAX_COLS) {
        currentRow++;
    }
    fclose(file);

    if (rowNumber < 1 || rowNumber > currentRow) {
        printf("Invalid row number\n");
        return;
    }

    file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    for (int i = 0; i < currentRow; i++) {
        if (i == rowNumber - 1) {
            fprintf(file, "%s\n", rowData);
        } else {
            fprintf(file, "%s", rows[i]);
        }
    }
    fclose(file);

    FILE *logFile = fopen("../change.log", "a");
    if (logFile == NULL) {
        perror("Error opening log file");
        return;
    }

    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char *originalRow = strdup(rows[rowNumber - 1]);
    originalRow[strcspn(originalRow, "\n")] = '\0';
    fprintf(logFile, "[%02d/%02d/%02d] [EDIT] %s diubah menjadi %s.\n", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, originalRow, rowData);
    free(originalRow);
    fclose(logFile);
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
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        valread = read(new_socket, buffer, 1024);

        printf("Received: %s\n", buffer);

        if (buffer[strlen(buffer) - 1] == '\n')
            buffer[strlen(buffer) - 1] = '\0';

        char *firstWord = strtok(buffer, " ");
        char *restOfString = NULL;

        if (firstWord != NULL) {
            restOfString = strtok(NULL, "");
        }

        if (firstWord != NULL) {
            if (strcmp("status", firstWord) == 0) {
                char* statres = status(restOfString);
                if (statres != NULL) {
                    send(new_socket, statres, strlen(statres), 0);
                    free(statres);
                } else {
                    char errmsg[] = "Error retrieving status.";
                    send(new_socket, errmsg, strlen(errmsg), 0);
                }
            } else if (strcmp("tampilkan", firstWord) == 0) {
                char* tampres = tampil();
                if (tampres != NULL) {
                    send(new_socket, tampres, strlen(tampres), 0);
                    free(tampres);
                } else {
                    char errmsg[] = "Error displaying data.";
                    send(new_socket, errmsg, strlen(errmsg), 0);
                }
            } else if (strcmp("genre", firstWord) == 0) {
                char* genreres = genre(restOfString);
                if (genreres != NULL) {
                    send(new_socket, genreres, strlen(genreres), 0);
                    free(genreres);
                } else {
                    char errmsg[] = "Error retrieving genre.";
                    send(new_socket, errmsg, strlen(errmsg), 0);
                }
            } else if (strcmp("hari", firstWord) == 0) {
                char* harires = hari(restOfString);
                if (harires != NULL) {
                    send(new_socket, harires, strlen(harires), 0);
                    free(harires);
                } else {
                    char errmsg[] = "Error retrieving day.";
                    send(new_socket, errmsg, strlen(errmsg), 0);
                }
            } else if (strcmp("delete", firstWord) == 0) {
                int delrow = findrow(restOfString);
                if (delrow != -1) {
                    deleteRow("../myanimelist.csv", delrow, restOfString);
                    char delmessage[] = "Anime berhasil dihapus";
                    send(new_socket, delmessage, strlen(delmessage), 0);
                } else {
                    char errmsg[] = "Anime tidak ditemukan";
                    send(new_socket, errmsg, strlen(errmsg), 0);
                }
            } else if (strcmp("add", firstWord) == 0) {
                addRow("../myanimelist.csv", restOfString);
                char addmessage[] = "Anime berhasil ditambahkan";
                send(new_socket, addmessage, strlen(addmessage), 0);
            } else if (strcmp("edit", firstWord) == 0) {
                char *restOfStringCopy = strdup(restOfString); 
                char *secondWord = strtok(restOfStringCopy, ","); 
                char *anotherword = strtok(NULL, "");
                int editrow = findrow(secondWord);
                if (editrow != 0) {
                    editRow("../myanimelist.csv", editrow, anotherword);
                    char editmessage[] = "Anime berhasil diubah";
                    send(new_socket, editmessage, strlen(editmessage), 0);
                } else {
                    char notfoundmsg[] = "Anime tidak ditemukan";
                    send(new_socket, notfoundmsg, strlen(notfoundmsg), 0);
                }
                free(restOfStringCopy);
            } else {
                char invalidmsg[] = "Invalid Command";
                send(new_socket, invalidmsg, strlen(invalidmsg), 0);
            }
        }
        memset(buffer, 0, sizeof(buffer));
        close(new_socket);
    }

    return 0;
}
