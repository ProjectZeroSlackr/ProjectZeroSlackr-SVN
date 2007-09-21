/*
 * event.c
 *
 * periodic event scheduler
 */

/* $Id: event.c,v 1.2 2001/02/27 04:13:22 nyef Exp $ */

#include "event.h"
#include <stdlib.h>
#include "tool.h"
#include "ui.h"
#include "cop.h"
struct event_scheduler {
    struct event *top_event;
    event_delay_callback_t delay_callback;
    void *delay_callback_data;
};
void event_reschedule(event_scheduler scheduler)
{
    struct event *prev_event;
    struct event *cur_event;
    struct event *next_event;
    int done;

    scheduler->top_event->timeleft = scheduler->top_event->duration;
    prev_event = NULL;
    cur_event = scheduler->top_event;
    next_event = cur_event->next;
    scheduler->top_event = next_event;
    done = 0;

    for (;;) {
	if (!next_event) {
	    done = 1;
	} else if (cur_event->timeleft <= next_event->timeleft) {
	    next_event->timeleft -= cur_event->timeleft;
	    done = 1;
	}
	
	if (done) {
	    if (prev_event) {
		prev_event->next = cur_event;
	    } else {
		scheduler->top_event = cur_event;
	    }
	    cur_event->next = next_event;
	    return;
	}
	
	cur_event->timeleft -= next_event->timeleft;
	
	prev_event = next_event;
	next_event = next_event->next;
    }
}

void event_register(event_scheduler scheduler, struct event *new_event)
{
    new_event->next = scheduler->top_event;
    scheduler->top_event = new_event;
    event_reschedule(scheduler);
}

void event_timeslice(event_scheduler scheduler)
{
    for (;;) {
	event_reschedule(scheduler);

	scheduler->delay_callback(scheduler->delay_callback_data,
				  scheduler->top_event->timeleft);
	
	if (system_flags & F_UNIMPL) {
	    deb_printf("Unimplimented Instruction.\n");
	    system_flags |= F_QUIT;
	    return;
	}
	
	if (!scheduler->top_event->callback) {
	    return;
	}

/* 	if (((inl(IBOY_COP_STATUS) & IBOY_COP_LINE_REQ) == 0) && (scheduler->top_event->duration==113))
	{
		cop_set(scheduler->top_event->callback);
		outl(inl(IBOY_COP_STATUS) | IBOY_COP_LINE_REQ2, IBOY_COP_STATUS); 
		outl(inl(IBOY_COP_STATUS) | IBOY_COP_LINE_REQ, IBOY_COP_STATUS); 
	}
	else*/
	{
		scheduler->top_event->callback();
	}
    }
}

event_scheduler new_event_scheduler(event_delay_callback_t callback,
				    void *callback_data)
{
    event_scheduler retval;

    retval = malloc(sizeof(struct event_scheduler));

    if (!retval) {
	return NULL;
    }

    retval->delay_callback = callback;
    retval->delay_callback_data = callback_data;
    retval->top_event = NULL;

    return retval;
}

/*
 * $Log: event.c,v $
 * Revision 1.2  2001/02/27 04:13:22  nyef
 * changed to use a callback function instead of a cal_cpu interface
 *
 * Revision 1.1  2000/03/10 01:48:36  nyef
 * Initial revision
 *
 */
