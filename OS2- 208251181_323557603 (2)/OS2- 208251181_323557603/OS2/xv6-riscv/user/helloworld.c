#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "uthread.h"

void
maintroll(){
  char str[19] = "Hello World troll\n";
  write(1, str, 19); 
  //uthread_get_priority();
  uthread_exit();
}
int
main()
{
  char str[13] = "Hello World\n";
  write(1, str, 13);
  int i = uthread_create(maintroll,0);
  int k = uthread_create(maintroll,2);
  printf("Value of i: %d\n", i);
  printf("Value of i: %d\n", k);
  //uthread_yield();
  int j = uthread_start_all();
  exit(j);
}
