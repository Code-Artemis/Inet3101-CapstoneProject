#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eventstore.h"
#include "storage.h"

#define DATA_FILE "events.dat"
void clearInputBuffer(void) {
    int ch;

    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

int main(void) {
    EventStore store;
    initEventStore(&store);

    loadEvents(&store, DATA_FILE);

    int choice;
    int id;
    Event event;

    while (1) {
        printf("\nCalendar Management System\n");
        printf("1. Add Event\n");
        printf("2. View Events\n");
        printf("3. Update Event\n");
        printf("4. Delete Event\n");
        printf("5. Exit\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input.\n");
            break;
        }

        switch (choice) {
            case 1:
                memset(&event, 0, sizeof(Event));

                printf("Event ID: ");
                if (scanf("%d", &event.id) != 1) break;

                printf("Title: ");
                if (scanf(" %99[^\n]", event.title) != 1) break;

                printf("Date (YYYY-MM-DD): ");
                if (scanf("%10s", event.date) != 1) break;
		clearInputBuffer();

                printf("Start time (HH:MM): ");
                if (scanf("%5s", event.start_time) != 1) break;
		clearInputBuffer();
                
		printf("End time (HH:MM): ");
                if (scanf("%5s", event.end_time) != 1) break;
		clearInputBuffer();
                
		printf("Recurrence (0=None, 1=Daily, 2=Weekly): ");
                if (scanf("%d", &event.recurrence.frequency) != 1) break;

                printf("Number of occurrences: ");
                if (scanf("%d", &event.recurrence.count) != 1) break;

		if (!validateEvent(&event)) {
   		 printf("Invalid event information.\n");
    		break;
		}
                int conflict = 0;
                EventNode *current = store.head;

                while (current != NULL) {
                    if (checkConflict(&event, &current->event)) {
                        conflict = 1;
                        break;
                    }
                    current = current->next;
                }

                if (conflict) {
                    printf("Event conflicts with an existing event.\n");
                } else if (addEvent(&store, event)) {
                    printf("Event added successfully.\n");
                    if (!saveEvents(&store, DATA_FILE)) {
                        printf("Error saving events.\n");
                    }
                } else {
                    printf("Could not add event.\n");
                }
                break;

            case 2:
                if (store.head == NULL) {
                    printf("No events found.\n");
                }

                current = store.head;

                while (current != NULL) {
                    Event *e = &current->event;

                    printf("\nID: %d\n", e->id);
                    printf("Title: %s\n", e->title);
                    printf("Date: %s\n", e->date);
                    printf("Time: %s - %s\n",
                           e->start_time, e->end_time);

                    current = current->next;
                }
                break;

            case 3:
                printf("Enter event ID to update: ");
                if (scanf("%d", &id) != 1) break;

                Event *existing = findEvent(&store, id);

                if (existing == NULL) {
                    printf("Event not found.\n");
                    break;
                }

                event = *existing;

                printf("New title: ");
                if (scanf(" %99[^\n]", event.title) != 1) break;

                printf("New date (YYYY-MM-DD): ");
                if (scanf("%10s", event.date) != 1) break;

                printf("New start time (HH:MM): ");
                if (scanf("%5s", event.start_time) != 1) break;

                printf("New end time (HH:MM): ");
                if (scanf("%5s", event.end_time) != 1) break;
		if (!validateEvent(&event)) {
    			printf("Invalid event information.\n");
    			break;
		}
		int updateConflict = 0;
		EventNode *check = store.head;

		while (check != NULL) {
    			if (check->event.id != event.id &&
        			checkConflict(&event, &check->event)) {
        			updateConflict = 1;
        			break;
    			}

    			check = check->next;
		}

		if (updateConflict) {
    			printf("Updated event conflicts with an existing event.\n");
    			break;
		}

                if (updateEvent(&store, event)) {
                    printf("Event updated successfully.\n");
                    if (!saveEvents(&store, DATA_FILE)) {
                        printf("Error saving events.\n");
                    }
                }
                break;

            case 4:
                printf("Enter event ID to delete: ");
                if (scanf("%d", &id) != 1) break;

                if (deleteEvent(&store, id)) {
                    printf("Event deleted successfully.\n");
                    if (!saveEvents(&store, DATA_FILE)) {
                        printf("Error saving events.\n");
                    }
                } else {
                    printf("Event not found.\n");
                }
                break;

            case 5:
                freeEventStore(&store);
                return 0;

            default:
                printf("Invalid choice.\n");
        }
    }

    freeEventStore(&store);
    return 0;
}
