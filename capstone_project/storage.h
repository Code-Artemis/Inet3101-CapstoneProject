#ifndef STORAGE_H
#define STORAGE_H

#include "eventstore.h"

int saveEvents(EventStore *store, const char *filename);
int loadEvents(EventStore *store, const char *filename);

#endif
