#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <time.h>

const char *get_filename(const char **path) {
    const char *filename = strrchr(*path, '/');
    if (filename == NULL)
        return *path;
    else
        return filename + 1;
}

char *read_shared_memory(key_t key) {
    int shmid = shmget(key, 0, 0666);
    char *shm_ptr = (char *)shmat(shmid, NULL, 0);
    char *string = strdup(shm_ptr);
    shmdt(shm_ptr);
    return string;
}

void updatePath(char **path) {
    char *token;
    const char search[] = "new-data";
    const char replacement[] = "microservices/database";

    token = strstr(*path, search);

    if (token != NULL) {
        size_t length_before = token - *path;
        size_t length_after = strlen(token + strlen(search));
        char *new_path = (char *)malloc(length_before + strlen(replacement) + length_after + 1);
        strncpy(new_path, *path, length_before);
        new_path[length_before] = '\0';
        strcat(new_path, replacement);
        strcat(new_path, token + strlen(search));
        free(*path);
        *path = new_path;
    }
}

int main() {
    char *string_1 = read_shared_memory(1233);
    char *oldstring_1 = read_shared_memory(1233);
    updatePath(&string_1);
    if (rename(oldstring_1, string_1) == 0) {
        const char *basename = get_filename((const char **)&oldstring_1);
        FILE *log_file = fopen("database/db.log", "a");
        if (log_file != NULL) {
            time_t raw_time;
            struct tm *time_info;
            time(&raw_time);
            time_info = localtime(&raw_time);
            fprintf(log_file, "[%02d/%02d/%04d %02d:%02d:%02d] [Parking Lot] [%s]\n",
                    time_info->tm_mday, time_info->tm_mon + 1, time_info->tm_year + 1900,
                    time_info->tm_hour, time_info->tm_min, time_info->tm_sec, basename);
            fclose(log_file);
        }
    }
    free(oldstring_1);

    char *string_2 = read_shared_memory(1235);
    char *oldstring_2 = read_shared_memory(1235);
    updatePath(&string_2);
    if (rename(oldstring_2, string_2) == 0) {
        const char *basename = get_filename((const char **)&oldstring_2);
        FILE *log_file = fopen("database/db.log", "a");
        if (log_file != NULL) {
            time_t raw_time;
            struct tm *time_info;
            time(&raw_time);
            time_info = localtime(&raw_time);
            fprintf(log_file, "[%02d/%02d/%04d %02d:%02d:%02d] [Trash Can] [%s]\n",
                    time_info->tm_mday, time_info->tm_mon + 1, time_info->tm_year + 1900,
                    time_info->tm_hour, time_info->tm_min, time_info->tm_sec, basename);
            fclose(log_file);
        }
    }
    free(oldstring_2);

    return 0;
}
