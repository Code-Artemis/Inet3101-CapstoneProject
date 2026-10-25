#ifndef EVENTSTORE_H
#define EVENTSTORE_H

#include "event.h"

typedef struct EventNode {
    Event event;
    struct EventNode *next;
} EventNode;

typedef struct {
    EventNode *head;
} EventStore;

void initEventStore(EventStore *store);
int addEvent(EventStore *store, Event event);
Event *findEvent(EventStore *store, int id);
int deleteEvent(EventStore *store, int id);
int updateEvent(EventStore *store, Event updatedEvent);
void freeEventStore(EventStore *store);

#endif
