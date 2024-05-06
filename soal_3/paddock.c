
#include "action.c"
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#define PORT 8080


void log_message(char* source, char* command, char* additional_info) {
    FILE *fp;
    fp = fopen("race.log", "a");
    if (fp == NULL) {
        printf("Error opening file race.log.\n");
        return;
    }
    fprintf(fp, "[%s] [%s]: [%s]\n", source, command, additional_info);
    fclose(fp);
}

char* rpc_call(char* command, char* info) {
    char* response = "";
    if (strcmp(command, "Gap") == 0) {
        float distance = atof(info);
        response = gap(distance);
    } 
    else if (strcmp(command, "Fuel") == 0) {
        int fuel_percent = atoi(info);
        response = fuel(fuel_percent);
    } 
    else if (strcmp(command, "Tire") == 0) {
        int tire_usage = atoi(info);
        response = tire(tire_usage);
    } 
    else if (strcmp(command, "Tire Change") == 0) {

        response = tire_change(info);
    } 
    else {
        response = "Invalid command";
    }
    log_message("Paddock", command, response);
    return response;
}

int main(int argc, char const *argv[]) {
    int server_fd, new_socket, valread, n;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char bapak[1024] = {0};
    char *hello = "Hello from server";
      
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
      
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
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

while(1){
            char *balik;
            n = read(new_socket, bapak, 1024);
            char ayah[100], ibu[100], anak[100], kakek[100];
            sscanf(bapak, "%s %s %s", ayah, ibu, anak);
            if(strcmp(ibu, "Change") == 0){
                sprintf(kakek, "%s %s", ayah, ibu);
                balik = rpc_call(kakek, anak);
            }
            else balik = rpc_call(ayah, ibu);
            send(new_socket, balik, strlen(balik), 0);
        
        }
}

