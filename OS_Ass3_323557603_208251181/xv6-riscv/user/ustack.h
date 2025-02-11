#include "kernel/types.h"
#include "user/user.h"

#ifndef USTACK_H
#define USTACK_H

#define MAX_ALLOC 512

void* ustack_malloc(uint len);
int ustack_free(void);
#endif
