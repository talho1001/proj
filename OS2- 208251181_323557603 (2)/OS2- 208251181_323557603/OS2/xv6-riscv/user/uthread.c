#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "uthread.h"

static struct uthread uthreads[MAX_UTHREADS];
static int tired_threads[MAX_UTHREADS];
static int num_uthreads = 0;
static int current_thread = 0;
static int main_returned = 0;
static int started = 0;

void drinkWateIfNeeded(){
    int i;
    int flag=1;
    for (i=0;i<num_uthreads;i++){
        if (uthreads[i].state==RUNNABLE && !uthreads[i].tired){
            flag=0;
        } 
    }
    if (flag==0){
        for (i=0;i<num_uthreads;i++){
            uthreads[i].tired=0;
    }
    }
}


int uthread_create(void (*start_func)(), enum sched_priority priority) {
    if (num_uthreads >= MAX_UTHREADS) {
        return -1;
    }
    printf("Value of i: %d\n",num_uthreads);
    struct uthread *ut = &uthreads[num_uthreads];
    ut->priority = priority;
    ut->context.ra = (uint64)(start_func);//maybe uint //maybe not

    //ut->ustack = (char*)(malloc(STACK_SIZE * sizeof(char)));
    uint64 ustack = (uint64) ut->ustack;
    ut->context.sp = ustack; //is it the top of the stack?
    /*
    uint64_t *stack = (uint64_t *)new_thread->ustack;
    stack[STACK_SIZE-1] = (uint64_t)start_func;
    new_thread->context.sp = (uint64_t)&stack[STACK_SIZE-32];//update ra??look up
    */
    num_uthreads++;
    ut->state = RUNNABLE;
    return 0;
}

int isTired(int index)
{
    if(!tired_threads[index])
    {
        return 0;
    }
    int i;
    int flag = 0;
    for(i =0; i < num_uthreads ; i ++)
    {
        if(!tired_threads[i])
        {
            flag = 1;
        }
    }
    if(!flag)
    {
        for(i =0; i < num_uthreads ; i ++)
        {
            tired_threads[i] = 0;
        }
        return 0;
    }
    return 1;
}

int find_highest_priority_uthread()
{
    drinkWateIfNeeded();
    struct uthread *max_thread = NULL;
    int i;
    int j = -1;
    for (i =0; i< num_uthreads; i++)
    {
        struct uthread* cur_thread = &uthreads[i];

        if(cur_thread->state == RUNNABLE &&
           // !isTired(i) &&
            ((max_thread == NULL) || (cur_thread->priority > max_thread->priority))//deadlock prone ?
          )
        {
            max_thread = cur_thread;
            j = i;
        }
        
    }
    tired_threads[j] = 1;
    return j;
}

void uthread_yield() 
{
    int index_of_max = find_highest_priority_uthread();
    struct uthread* max_priority_thread = &uthreads[index_of_max];
    if(max_priority_thread == NULL)
    {
        return; //incidates on no runnable threads
    }

    struct uthread* cur_thread = &uthreads[current_thread];
    printf("Value of State is: %d\n",cur_thread->state);
    if(cur_thread -> state == FREE)
    {
        max_priority_thread->state = RUNNING;
        current_thread = index_of_max;
        struct context junk_context = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        uswtch(&junk_context, &(max_priority_thread->context));
        return;
    }
    cur_thread->state = RUNNABLE;
    max_priority_thread->state = RUNNING;
    current_thread = index_of_max;
    max_priority_thread->tired=1;
    uswtch(&(cur_thread->context), &(max_priority_thread->context));
    return;
    
}

/*
void uthread_yield() {
    int i;

    // find the next ready thread with highest priority
    struct uthread *next_thread = NULL;
    for (i = 0; i < num_uthreads; i++) {
        current_thread = (current_thread + 1) % num_uthreads;
        if (uthreads[current_thread].state == RUNNABLE &&
                (next_thread == NULL || uthreads[current_thread].priority > next_thread->priority)) {
            next_thread = &uthreads[current_thread];
        }
    }

    if (next_thread == NULL) {
        // no other runnable threads
        return;
    }

    struct uthread *prev_thread = &uthreads[current_thread];
    next_thread->state = RUNNING;
    prev_thread->state = RUNNABLE;
    uswtch(&prev_thread->context, &next_thread->context);
}
*/

void uthread_exit() 
{
    printf("Called Exit: %s\n","s");

    struct uthread *ut = &uthreads[current_thread];
    ut->state = FREE;
    int next = find_highest_priority_uthread();
    if(next < 0)
    {
        //no other runnable threads
        if(main_returned)
        {
            exit(0);
        }
        else
        {
            uthread_yield(); //chat-gpt suggest but I think it will just return void in any case
            //---duck snitches chase bunny---// 
            //--HeroBrain Was Here--//
            return;
        }
    }
    else
    {
        struct uthread* nt = &uthreads[next];
        nt-> state = RUNNING;
        uswtch(&(ut->context), &(nt->context));
        current_thread = next;
    }

}



/*

void uthread_exit() {
    struct uthread *current_thread = &uthreads[current_thread];
    current_thread->state = FREE;

    // find the next ready thread with highest priority
    struct uthread *next_thread = NULL;
    int i;
    for (i = 0; i < num_uthreads; i++) {
        current_thread = &uthreads[current_thread];
        current_thread = (current_thread + 1) % num_uthreads;
        if (current_thread->state == RUNNABLE &&
                (next_thread == NULL || current_thread->priority > next_thread->priority)) {
            next_thread = current_thread;
        }
    }

    if (next_thread == NULL) {
        // no other runnable threads
        if (main_returned) {
            exit(0);
        } else {
            printf("All threads terminated\n");
            uthread_yield();
        }
    } else {
        next_thread->state = RUNNING;
        uswtch(&current_thread->context, &next_thread->context);
    }
}
*/


enum sched_priority uthread_set_priority(enum sched_priority priority) 
{
    struct uthread *ut = &uthreads[current_thread];
    enum sched_priority old_priority = ut->priority;
    ut-> priority = priority;
    return old_priority;
}


enum sched_priority uthread_get_priority()
{
    struct uthread *ut = &uthreads[current_thread];
    return ut->priority;

}

int uthread_start_all()
{
    if (started)
    {
        return -1;
    }
    started = 1;
    if(num_uthreads > 0)
    {
        int index = find_highest_priority_uthread();
        struct uthread* ut = &uthreads[index];
        printf("Value of i: %s\n","PUTA!!!!!!!!!!!!!");
        struct context junk_context = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        main_returned = 1;
        uswtch(&junk_context, &(ut->context)); //yet to validate
    }
            printf("Value of i is still: %s\n","PUTA!!!!!!!!!!!!!");

    main_returned = 1;
    return 0;
}

struct uthread* uthread_self()
{
    return &uthreads[current_thread];
}
/*
int uthread_start_all() {
    if (started) {
        return -1;
    }

    started = 1;

    if (num_uthreads > 0) {
        uthreads[0].state = RUNNING;
        uswtch(NULL, &uthreads[0].context);
    }

    main_returned = 1;
    return 0;
}

enum sched_priority uthread_set_priority(enum sched_priority priority) {
    struct uthread *current_thread = &uthreads[current_thread];
    enum sched_priority old_priority = current_thread->priority;
    current_thread->
*/