#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 256

int main(void) {
    int sock;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1",
                  &server_address.sin_addr) <= 0) {
        perror("Invalid address");
        close(sock);
        return 1;
    }

    if (connect(sock, (struct sockaddr *)&server_address,
                sizeof(server_address)) < 0) {
        perror("Connection failed");
        close(sock);
        return 1;
    }

    printf("Connected to Calendar Server.\n");
    printf("Shared Calendar Events:\n");

    while ((bytes_received = recv(sock, buffer,
                                  sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        printf("%s", buffer);
    }

    if (bytes_received < 0) {
        perror("Receive failed");
    }

    close(sock);

    return bytes_received < 0 ? 1 : 0;
}
