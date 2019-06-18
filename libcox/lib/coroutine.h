#ifndef __COROUTINE_H__
#define __COROUTINE_H__

#include "co_routine_inner.h"
#include "inner_pre.h"
#include <stdlib.h>

class CoRoutine
{
    public:
        CoRoutine()
        {
            coroutine = (struct stCoRoutine_t *)calloc(1, sizeof(struct stCoRoutine_t));
            fd = -1;
            functor = 0;
            seconds = 0;
        }        
        CoRoutine(const Functor& f, int sec)
            : functor(f)
            , seconds(sec)
        {              
            coroutine = (struct stCoRoutine_t *)calloc(1, sizeof(struct stCoRoutine_t));
            fd = -1;
        }              
        ~CoRoutine()   
        {
            free(coroutine);
        }              
    public:
        struct stCoRoutine_t *coroutine;
	    int fd;
        Functor functor;
        int seconds; 
};

#endif
