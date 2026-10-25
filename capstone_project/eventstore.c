#include <stdio.h>
#include <stdlib.h>
#include "eventstore.h"

void initEventStore(EventStore *store) {
    store->head = NULL;
}

int addEvent(EventStore *store, Event event) {
    if (findEvent(store, event.id) != NULL) {
        return 0;
    }

    EventNode *newNode = malloc(sizeof(EventNode));

    if (newNode == NULL) {
        return 0;
    }

    newNode->event = event;
    newNode->next = store->head;
    store->head = newNode;

    return 1;
}

Event *findEvent(EventStore *store, int id) {
    EventNode *current = store->head;

    while (current != NULL) {
        if (current->event.id == id) {
            return &current->event;
        }

        current = current->next;
    }

    return NULL;
}

int deleteEvent(EventStore *store, int id) {
    EventNode *current = store->head;
    EventNode *previous = NULL;

    while (current != NULL) {
        if (current->event.id == id) {
            if (previous == NULL) {
                store->head = current->next;
            } else {
                previous->next = current->next;
            }

            free(current);
            return 1;
        }

        previous = current;
        current = current->next;
    }

    return 0;
}
int updateEvent(EventStore *store, Event updatedEvent) {
    Event *existingEvent = findEvent(store, updatedEvent.id);

    if (existingEvent == NULL) {
        return 0;
    }

    *existingEvent = updatedEvent;

    return 1;
}
void freeEventStore(EventStore *store) {
    EventNode *current = store->head;

    while (current != NULL) {
        EventNode *next = current->next;
        free(current);
        current = next;
    }

    store->head = NULL;
}
