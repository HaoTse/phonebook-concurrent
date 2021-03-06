#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "phonebook_opt.h"
#include "debug.h"

entry *findName(char lastname[], entry *pHead)
{
    size_t len = strlen(lastname);
    while (pHead != NULL) {
        if (strncasecmp(lastname, pHead->lastName, len) == 0
                && (pHead->lastName[len] == '\n' ||
                    pHead->lastName[len] == '\0')) {
            pHead->lastName = (char *) malloc(sizeof(char) *
                                              MAX_LAST_NAME_SIZE);
            memset(pHead->lastName, '\0', MAX_LAST_NAME_SIZE);
            strcpy(pHead->lastName, lastname);
            pHead->dtl = (pdetail) malloc(sizeof(detail));
            return pHead;
        }
        dprintf("find string = %s\n", pHead->lastName);
        pHead = pHead->pNext;
    }
    return NULL;
}

append_arg *new_append_arg(char *ptr, char *eptr, int tid, int ntd, entry *start)
{
    append_arg *app = (append_arg *) malloc(sizeof(append_arg));

    app->ptr = ptr;
    app->eptr = eptr;
    app->tid = tid;
    app->nthread = ntd;
    app->entryStart = start;

    app->pHead = (app->pLast = app->entryStart);
    return app;
}

void append(void *arg)
{
    struct timespec start, end;

    clock_gettime(CLOCK_REALTIME, &start);

    append_arg *app = (append_arg *) arg;

    int count = 0;
    entry *j = app->entryStart;
    char *i = app->ptr;
    while(i < app->eptr) {
        app->pLast->pNext = j;
        app->pLast = app->pLast->pNext;

        app->pLast->lastName = i;
        dprintf("thread %d append string = %s\n",
                app->tid, app->pLast->lastName);
        app->pLast->pNext = NULL;

        i += MAX_LAST_NAME_SIZE * app->nthread;
        j += app->nthread;
        count++;
    }
    clock_gettime(CLOCK_REALTIME, &end);

    dprintf("thread take %lf sec, count %d\n", diff_in_second(start, end), count);

    pthread_exit(NULL);
}

void show_entry(entry *pHead)
{
    while (pHead != NULL) {
        printf("lastName = %s\n", pHead->lastName);
        pHead = pHead->pNext;
    }
}

#ifdef PROFILE
static double diff_in_second(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec-t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec + diff.tv_nsec / 1000000000.0);
}
#endif
