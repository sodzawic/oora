#ifndef _OORA_PAPI_
#define _OORA_PAPI_

#include <stdio.h>
#include <stdlib.h>
#include "papi.h"

struct papi_context {
    int eventSet;
    int eventId;
    const char *eventName;
    double startTime;
};

struct papi_context papi_start(const char *ev_name) {
    /* init lib */
    int papi_err;
    struct papi_context context;
    context.eventSet = PAPI_NULL;

    if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT) {
        fprintf(stderr, "PAPI is unsupported.\n");
    }

    if (PAPI_num_counters() < 1) {
        fprintf(stderr, "PAPI is unsupported.\n");
        return context;
    }

    if ((papi_err = PAPI_event_name_to_code((char *)ev_name, &context.eventId)) != PAPI_OK) {
        fprintf(stderr, "Could convert event name %s to code: %s\n", ev_name, PAPI_strerror(papi_err));
        return context;
    }
    context.eventName = ev_name;

    if ((papi_err = PAPI_create_eventset(&context.eventSet)) != PAPI_OK) {
        fprintf(stderr, "Could not create event set: %s\n", PAPI_strerror(papi_err));
        return context;
    }

    if ((papi_err = PAPI_add_event(context.eventSet, context.eventId)) != PAPI_OK ) {
        fprintf(stderr, "Could not add event: %s\n", PAPI_strerror(papi_err));
        return context;
    }

    if ((papi_err = PAPI_start(context.eventSet)) != PAPI_OK) {
        fprintf(stderr, "Could not start counters: %s\n", PAPI_strerror(papi_err));
        return context;
    }

    context.startTime = PAPI_get_virt_cyc();
    return context;
}

void papi_stop(struct papi_context context) {
    if (context.eventSet != PAPI_NULL) {
        int papi_err;
        long long values[1] = {0};
        if ((papi_err = PAPI_stop(context.eventSet, values)) != PAPI_OK) {
            fprintf(stderr, "Could not get values: %s\n", PAPI_strerror(papi_err));
            return;
        }
        double endTime = PAPI_get_virt_cyc();
        printf("Time: %g\n", endTime - context.startTime);
        printf("Performance counters: \n");
        printf("%s %lld\n", context.eventName, values[0]);
    }
}

#endif
