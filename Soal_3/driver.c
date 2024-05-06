#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8080
  
int main(int argc, char const *argv[]) {
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char bapak[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
while (1) {
    printf("Enter message: ");
    memset(bapak, 0, sizeof(bapak));
    fgets(bapak, sizeof(bapak), stdin);
    
    // Remove newline character if present
    bapak[strcspn(bapak, "\n")] = '\0';

    // Check if user wants to exit
    if (strcmp(bapak, "exit") == 0) {
        printf("Exiting...\n");
        break;
    }

    // Send message to server
    send(sock, bapak, strlen(bapak), 0);

    // Receive message from server
    memset(bapak, 0, sizeof(bapak));
    valread = recv(sock, bapak, sizeof(bapak), 0);

    // Print received message
    printf("[Papa]: [%s]\n", bapak);
}
    close(sock);
   return 0;
}
