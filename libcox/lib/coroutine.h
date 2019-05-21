#ifndef __COROUTINE_H__
#define __COROUTINE_H__

#include "co_routine_inner.h"
#include <stdlib.h>

class CoRoutine
{
    public:                     
        CoRoutine()    
        {              
            coroutine = (struct stCoRoutine_t *)calloc(1, sizeof(struct stCoRoutine_t));
	    fd = -1;
        }              
        ~CoRoutine()   
        {
	    fd = -1;              
            free(coroutine);
        }              
        struct stCoRoutine_t *coroutine;
	int fd;
};

#endif
