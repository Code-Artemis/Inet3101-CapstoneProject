#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "eventstore.h"
#include "storage.h"

#define PORT 8080
#define DATA_FILE "events.dat"
static volatile sig_atomic_t running = 1;

static void handleSignal(int signalNumber) {
    (void)signalNumber;
    running = 0;
}

int main(void) {
    signal(SIGINT, handleSignal);
    int server_fd, client_fd;
    struct sockaddr_in address;
    int opt = 1;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
                   &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(server_fd);
        return 1;
    }

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        close(server_fd);
        return 1;
    }

    printf("Calendar server listening on port %d...\n", PORT);

    while (running) {
    	client_fd = accept(server_fd, NULL, NULL);

        if (client_fd < 0) {
   		if (!running) {
        		break;
    		}

    		perror("Accept failed");
    		continue;
	}

        EventStore store;
        initEventStore(&store);
        loadEvents(&store, DATA_FILE);

        EventNode *current = store.head;
        char buffer[256];

        while (current != NULL) {
            int length = snprintf(
                buffer, sizeof(buffer),
                "ID: %d | %s | %s | %s-%s\n",
                current->event.id,
                current->event.title,
                current->event.date,
                current->event.start_time,
                current->event.end_time
            );

            if (length < 0 || length >= (int)sizeof(buffer)) {
                fprintf(stderr, "Event message too long.\n");
                break;
            }

            size_t sent = 0;

            while (sent < (size_t)length) {
                ssize_t result = send(
                    client_fd,
                    buffer + sent,
                    (size_t)length - sent,
                    0
                );

                if (result <= 0) {
                    perror("Send failed");
                    break;
                }

                sent += (size_t)result;
            }

            if (sent < (size_t)length) {
                break;
            }

            current = current->next;
        }

        freeEventStore(&store);
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
