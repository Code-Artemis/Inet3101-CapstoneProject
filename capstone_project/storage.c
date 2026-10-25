#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <unistd.h>
#include <fcntl.h>
#include "storage.h"

int saveEvents(EventStore *store, const char *filename) {
    int lock_fd = open("events.lock", O_CREAT | O_RDWR, 0600);

    if (lock_fd < 0) {
        perror("Error opening lock file");
        return 0;
    }

    if (flock(lock_fd, LOCK_EX) < 0) {
        perror("Error locking file");
        close(lock_fd);
        return 0;
    }

    FILE *file = fopen(filename, "wb");

    if (file == NULL) {
        perror("Error opening file");
        flock(lock_fd, LOCK_UN);
        close(lock_fd);
        return 0;
    }

    EventNode *current = store->head;
    int success = 1;

    while (current != NULL) {
        if (fwrite(&current->event, sizeof(Event), 1, file) != 1) {
            perror("Error writing event");
            success = 0;
            break;
        }

        current = current->next;
    }

    if (fclose(file) != 0) {
        perror("Error closing file");
        success = 0;
    }

    flock(lock_fd, LOCK_UN);
    close(lock_fd);

    return success;
}

int loadEvents(EventStore *store, const char *filename) {
    int lock_fd = open("events.lock", O_CREAT | O_RDWR, 0600);

    if (lock_fd < 0) {
        perror("Error opening lock file");
        return 0;
    }

    if (flock(lock_fd, LOCK_SH) < 0) {
        perror("Error locking file");
        close(lock_fd);
        return 0;
    }

    FILE *file = fopen(filename, "rb");

    if (file == NULL) {
        flock(lock_fd, LOCK_UN);
        close(lock_fd);
        return 0;
    }

    Event event;
    int success = 1;

    while (fread(&event, sizeof(Event), 1, file) == 1) {
        if (!addEvent(store, event)) {
            success = 0;
            break;
        }
    }

    if (ferror(file)) {
    perror("Error reading file");
    success = 0;
} else {
    long position = ftell(file);

    if (position < 0 || position % sizeof(Event) != 0) {
        fprintf(stderr, "Error: Incomplete event record.\n");
        success = 0;
    }
}

    if (fclose(file) != 0) {
        perror("Error closing file");
        success = 0;
    }

    flock(lock_fd, LOCK_UN);
    close(lock_fd);

    return success;
}
