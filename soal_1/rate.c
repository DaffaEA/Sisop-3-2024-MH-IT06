#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <libgen.h>

const char *get_filename(const char *path) {
    const char *filename = strrchr(path, '/');
    if (filename == NULL)
        return path;
    else
        return filename + 1;
}

void readFromSharedMemory(key_t key) {
    int shmid = shmget(key, 0, 0666);
    char *shm_ptr = (char *)shmat(shmid, NULL, 0);
    
    const char *filename = NULL;
    if (key == 1233) {
        filename = get_filename(shm_ptr);
        printf("Type: Parking Lot\nFilename: %s\n", filename);
    } else if (key == 1235) {
        filename = get_filename(shm_ptr);
        printf("Type: Trash Can\nFilename: %s\n", filename);
    }
    
    printf("------------------------------------\n");

    shmdt(shm_ptr);
}

void find_max_rating(key_t key) {
    float max_rating = -1;
    char max_name[100] = "";
    
    int shmid = shmget(key, 0, 0666);
    char *shm_ptr = (char *)shmat(shmid, NULL, 0);
    char *data = strdup(shm_ptr);

    char *line = strtok(data, "\n");

    while (line != NULL) {
        char name[100];
        float rating;
        sscanf(line, "%[^,], %f", name, &rating);

        if (rating > max_rating) {
            max_rating = rating;
            strcpy(max_name, name);
        }

        line = strtok(NULL, "\n");
    }

    free(data);
    shmdt(shm_ptr);

    printf("Name: %s\nRating: %.1f\n", max_name, max_rating);
}


int main() {
    readFromSharedMemory(1233);
    find_max_rating(1234);
    printf("\n\n");
    readFromSharedMemory(1235);
    find_max_rating(1236);
    
    return 0;
}
