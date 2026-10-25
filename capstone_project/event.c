#include <stdio.h>
#include <string.h>
#include <time.h>
#include "event.h"

static int eventDate(const Event *event, struct tm *date) {
    int year, month, day;
    char extra;

    if (sscanf(event->date, "%4d-%2d-%2d%c",
               &year, &month, &day, &extra) != 3) {
        return 0;
    }

    if (year < 1900 || month < 1 || month > 12 ||
        day < 1 || day > 31) {
        return 0;
    }

    memset(date, 0, sizeof(*date));
    date->tm_year = year - 1900;
    date->tm_mon = month - 1;
    date->tm_mday = day;
    date->tm_hour = 12;
    date->tm_isdst = -1;

    if (mktime(date) == (time_t)-1) {
        return 0;
    }

    return date->tm_year == year - 1900 &&
           date->tm_mon == month - 1 &&
           date->tm_mday == day;
}

static int timeMinutes(const char *value) {
    int hour, minute;
    char extra;

    if (sscanf(value, "%2d:%2d%c",
               &hour, &minute, &extra) != 2) {
        return -1;
    }

    if (hour < 0 || hour > 23 || minute < 0 || minute > 59) {
        return -1;
    }

    return hour * 60 + minute;
}

int checkConflict(const Event *event1, const Event *event2) {
    struct tm date1, date2;

    if (!eventDate(event1, &date1) ||
        !eventDate(event2, &date2)) {
        return 0;
    }

    int start1 = timeMinutes(event1->start_time);
    int end1 = timeMinutes(event1->end_time);
    int start2 = timeMinutes(event2->start_time);
    int end2 = timeMinutes(event2->end_time);

    if (start1 < 0 || start2 < 0 ||
        end1 <= start1 || end2 <= start2) {
        return 0;
    }

    if (!(start1 < end2 && end1 > start2)) {
        return 0;
    }

    int count1 = event1->recurrence.frequency == 0
                 ? 1 : event1->recurrence.count;
    int count2 = event2->recurrence.frequency == 0
                 ? 1 : event2->recurrence.count;

    if (count1 < 1 || count2 < 1) {
        return 0;
    }

    int step1 = event1->recurrence.frequency == 1 ? 1 :
                event1->recurrence.frequency == 2 ? 7 : 0;
    int step2 = event2->recurrence.frequency == 1 ? 1 :
                event2->recurrence.frequency == 2 ? 7 : 0;

    for (int i = 0; i < count1; i++) {
        struct tm occurrence1 = date1;
        occurrence1.tm_mday += i * step1;

        time_t day1 = mktime(&occurrence1);

        for (int j = 0; j < count2; j++) {
            struct tm occurrence2 = date2;
            occurrence2.tm_mday += j * step2;

            time_t day2 = mktime(&occurrence2);

            if (day1 != (time_t)-1 &&
                day2 != (time_t)-1 &&
                occurrence1.tm_year == occurrence2.tm_year &&
                occurrence1.tm_yday == occurrence2.tm_yday) {
                return 1;
            }
        }
    }

    return 0;
}


int validateEvent(const Event *event) {
    struct tm date;

    if (event == NULL) {
        return 0;
    }

    if (event->id <= 0) {
        return 0;
    }

    if (event->title[0] == '\0') {
        return 0;
    }

    if (!eventDate(event, &date)) {
        return 0;
    }

    int start = timeMinutes(event->start_time);
    int end = timeMinutes(event->end_time);

    if (start < 0 || end < 0 || start >= end) {
        return 0;
    }

    if (event->recurrence.frequency < 0 ||
        event->recurrence.frequency > 2) {
        return 0;
    }

    if (event->recurrence.count < 1 ||
        event->recurrence.count > 365) {
        return 0;
    }

    return 1;
}
