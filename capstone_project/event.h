#ifndef EVENT_H
#define EVENT_H

typedef struct {
    int frequency;
    int count;
} Recurrence;

typedef struct {
    int id;
    char title[100];
    char date[11];
    char start_time[6];
    char end_time[6];
    Recurrence recurrence;
} Event;
int checkConflict(const Event *event1, const Event *event2);
int validateEvent(const Event *event);

#endif
