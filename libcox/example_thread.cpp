#include "event_loop.h"
#include "event_coroutine.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

void* RoutineFunc(void *args)
{
    int* routineid = (int *)args;

    while (true)
    {
        char sBuff[128];
        sprintf(sBuff, "from Thread ID[%d] Coroutine ID[%d] with stack addr %p\n", GetTid(), *routineid, sBuff);
        printf("%s", sBuff);
        poll(NULL, 0, 1000);
    }
    return NULL;
}

int loop(void *)
{
    //printf("I am loopping\n");
	return 0;
}

static void *routine_func( void * )
{
    EventLoop lp(loop, 0);
    Coroutine* co[2];
    int routineid[2];

    for (int i = 0; i < 2; i++) {
        routineid[i] = i;
        co[i] = new Coroutine(RoutineFunc, routineid + i);
        co[i]->Resume();
    }

    lp.Run();
	return 0;
}

int main(int argc,char *argv[])
{
    int cnt;
    if (argc == 2) {
	    cnt = atoi( argv[1] );
    } else { // by default 2 threads
        cnt = 2;
    }

	pthread_t tid[ cnt ];
	for(int i=0;i<cnt;i++)
	{
		// 创建线程
		pthread_create( tid + i,NULL,routine_func,0);
	}
    // 
	for(;;)
	{
		sleep(1);
	}
	
	return 0;
}

