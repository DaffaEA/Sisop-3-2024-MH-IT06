#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

// Function to convert words to numbers
int wordsToNumbers(char *word) {
    if (strcmp(word, "nol") == 0) return 0;
    else if (strcmp(word, "satu") == 0) return 1;
    else if (strcmp(word, "dua") == 0) return 2;
    else if (strcmp(word, "tiga") == 0) return 3;
    else if (strcmp(word, "empat") == 0) return 4;
    else if (strcmp(word, "lima") == 0) return 5;
    else if (strcmp(word, "enam") == 0) return 6;
    else if (strcmp(word, "tujuh") == 0) return 7;
    else if (strcmp(word, "delapan") == 0) return 8;
    else if (strcmp(word, "sembilan") == 0) return 9;
    else return -1; // Return -1 for unknown words
}

// Function to convert numbers to words
void numbersToWords(int number, char *result) {
    const char *ones[] = {"", "satu", "dua", "tiga", "empat", "lima", "enam", "tujuh", "delapan", "sembilan"};
    const char *tens[] = {"", "sepuluh", "dua puluh", "tiga puluh", "empat puluh", "lima puluh", "enam puluh", "tujuh puluh", "delapan puluh", "sembilan puluh"};

    if (number < 10)
        sprintf(result, "%s", ones[number]);
    else if (number < 20)
        sprintf(result, "%s belas", ones[number % 10]);
    else {
        sprintf(result, "%s", tens[number / 10]);
        if (number % 10 != 0) {
            strcat(result, " ");
            strcat(result, ones[number % 10]);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <operation>\n", argv[0]);
        printf("Operations:\n");
        printf("-kali: Perkalian\n-tambah: Penjumlahan\n-kurang: Pengurangan\n-bagi: Pembagian\n");
        return 1;
    }

    // Create pipes
    int pipe_parent_child[2];
    int pipe_child_parent[2];
    if (pipe(pipe_parent_child) == -1 || pipe(pipe_child_parent) == -1) {
        perror("Failed to create pipe");
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        return 1;
    } else if (pid == 0) { // Child process
        // Close unused ends of pipes
        close(pipe_parent_child[1]);
        close(pipe_child_parent[0]);

        // Read data from parent
        int result;
        read(pipe_parent_child[0], &result, sizeof(result));

        // Convert number to words
        char words_result[100];
        numbersToWords(result, words_result);

        // Write result to parent
        write(pipe_child_parent[1], words_result, strlen(words_result) + 1);

        // Close remaining ends of pipes
        close(pipe_parent_child[0]);
        close(pipe_child_parent[1]);
    } else { // Parent process
        // Close unused ends of pipes
        close(pipe_parent_child[0]);
        close(pipe_child_parent[1]);

  
        char input[100];
        printf("Masukkan dua kata: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;

        char *token = strtok(input, " ");
        int numbers[2];
        int i = 0;
        while (token != NULL && i < 2) {
            numbers[i] = wordsToNumbers(token);
            if (numbers[i] == -1) {
                printf("Kata tidak valid: %s\n", token);
                return 1;
            }
            token = strtok(NULL, " ");
            i++;
        }

     
        int result;
        if (strcmp(argv[1], "-kali") == 0) {
            result = numbers[0] * numbers[1];
        } else if (strcmp(argv[1], "-tambah") == 0) {
            result = numbers[0] + numbers[1];
        } else if (strcmp(argv[1], "-kurang") == 0) {
            result = numbers[0] - numbers[1];
            if (result < 0) {
                printf("ERROR\n");
                return 1;
            }
        } else if (strcmp(argv[1], "-bagi") == 0) {
            if (numbers[1] == 0) {
                printf("Pembagian oleh nol tidak diizinkan\n");
                return 1;
            }
            result = numbers[0] / numbers[1];
        } else {
            printf("Operasi tidak valid\n");
            return 1;
        }

        // Write result to child
        write(pipe_parent_child[1], &result, sizeof(result));

        // Read result from child
        char words_result[100];
        read(pipe_child_parent[0], words_result, sizeof(words_result));

        // Print result
        printf("Hasil %s %s dan %s adalah %s.\n", argv[1] + 1, input, words_result);

        FILE *log_file = fopen("histori.log", "a");
        if (log_file == NULL) {
            perror("Failed to open log file");
            return 1;
        }

        char log_entry[200];
        time_t current_time;
        time(&current_time);
        struct tm *time_info = localtime(&current_time);
        strftime(log_entry, sizeof(log_entry), "[%d/%m/%y %H:%M:%S] ", time_info);
        strcat(log_entry, argv[1] + 1);
        strcat(log_entry, " ");
        strcat(log_entry, words_result);
        fprintf(log_file, "%s.\n", log_entry);

        fclose(log_file);

   
        close(pipe_parent_child[1]);
        close(pipe_child_parent[0]);
    }

    return 0;
}
