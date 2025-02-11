#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

extern struct proc proc[NPROC];

void kthreadinit(struct proc *p)
{
  initlock(&p->kthread_lock, "thread ID");

  for (struct kthread *kt = p->kthread; kt < &p->kthread[NKT]; kt++)
  {
    kt->state = UNUSED;
    kt->parent = p;
    // WARNING: Don't change this line!
    // get the pointer to the kernel stack of the kthread
    kt->kstack = KSTACK((int)((p - proc) * NKT + (kt - p->kthread)));
  }
}

struct kthread *mykthread()
{
  struct cpu* c = mycpu();
  struct kthread* kt = c->thread;
  return kt;
}

struct trapframe *get_kthread_trapframe(struct proc *p, struct kthread *kt)
{
  return p->base_trapframes + ((int)(kt - p->kthread));
}

int
alloctid(struct proc *p)
{
  int tid;
  
  acquire(&p->kthread_lock);//this?????????
  tid = p->nextt+1;
  release(&p->kthread_lock);
  //printf("here");
  //printf("%d\n", tid);


  return tid;
}

struct kthread *
allocthread(struct proc *p)
{
  struct kthread *kt;

  for (kt = p->kthread; kt < &p->kthread[NKT]; kt++) {
    //acquire(&kt->lock);
    //printf("lick");
    if (kt->state == UNUSED) {
      goto found;
    } else {
      //release(&kt->lock);
          //printf("realllerrere");

    }
  }

  return 0;

found:
  kt->tid = alloctid(p);
  kt->state = USED;
  kt->trapframe = get_kthread_trapframe(p,kt);
          //printf("realllerrere");

  // Set up new context to start executing at forkret,
  // which returns to user space.
  memset(&kt->context, 0, sizeof(kt->context));
  kt->context.ra = (uint64)forkret;
  kt->context.sp = kt->kstack;
  //release(&kt->lock);
  acquire(&kt->lock);
  return kt;
}
void
free_kthread(struct kthread *kt)
{
  kt->tid = 0;
  kt->parent = 0;
  kt->state = UNUSED;
  kt->chan = 0;
  kt->killed = 0;
  kt->xstate = 0;
  kt->trapframe = 0;
  memset(&kt->context, 0, sizeof(kt->context));
}






/*// TODO: delte this after you are done with task 2.2
void allocproc_help_function(struct proc *p) {
  p->kthread->trapframe = get_kthread_trapframe(p, p->kthread);

  p->context.sp = p->kthread->kstack + PGSIZE;}
*/
