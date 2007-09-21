/*
 * event.h
 *
 * periodic event scheduler
 */

/* $Id: event.h,v 1.2 2001/02/27 04:13:26 nyef Exp $ */

#ifndef EVENT_H
#define EVENT_H

struct event {
    struct event *next;
    int timeleft;
    int duration;
    void (*callback)(void);
};

typedef struct event_scheduler *event_scheduler;

typedef void (*event_delay_callback_t)(void *, int);

void event_register(event_scheduler scheduler, struct event *new_event);
void event_timeslice(event_scheduler scheduler);
event_scheduler new_event_scheduler(event_delay_callback_t callback,
				    void *callback_data);

#endif 

/*
 * $Log: event.h,v $
 * Revision 1.2  2001/02/27 04:13:26  nyef
 * changed to use a callback function instead of a cal_cpu interface
 *
 * Revision 1.1  2000/03/10 01:48:40  nyef
 * Initial revision
 *
 */
