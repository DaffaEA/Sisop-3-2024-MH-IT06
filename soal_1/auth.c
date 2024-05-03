#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

void movetoshared(const char* filename, key_t key){
    FILE *file = fopen(filename, "r");
    
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    int shmid = shmget(key, file_size, IPC_CREAT | 0666);

    char *shm_ptr = (char *)shmat(shmid, NULL, 0);

    size_t bytes_read = fread(shm_ptr, 1, file_size, file);

    shmdt(shm_ptr);
    fclose(file);
}

void movenametoshared(const char* filename, key_t key){
    int shmid = shmget(key, strlen(filename) + 1, IPC_CREAT | 0666);
    char *shm_ptr = (char *)shmat(shmid, NULL, 0);

    strcpy(shm_ptr, filename);

    shmdt(shm_ptr);
}

int main() {
    char dir_path[100];
    getcwd(dir_path, sizeof(dir_path));
    strcat(dir_path, "/new-data");

    DIR *dir = opendir(dir_path);

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name + strlen(entry->d_name) - 14, "parkinglot.csv") == 0) {
            char filepath[256];
            snprintf(filepath, sizeof(filepath), "%s/%s", dir_path, entry->d_name);
             movenametoshared(filepath, 1233);
            movetoshared(filepath, 1234);
        } else if (strcmp(entry->d_name + strlen(entry->d_name) - 12, "trashcan.csv") == 0) {
            char filepath[256];
            snprintf(filepath, sizeof(filepath), "%s/%s", dir_path, entry->d_name);
            movenametoshared(filepath, 1235);
            movetoshared(filepath, 1236);
        } else {
            char filepath[256];
            snprintf(filepath, sizeof(filepath), "%s/%s", dir_path, entry->d_name);
            remove(filepath);
        }
    }
    closedir(dir);

    return 0;
}
