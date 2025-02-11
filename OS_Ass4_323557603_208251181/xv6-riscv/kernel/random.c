
#include <stdarg.h>

#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "proc.h"


static uint8 LFSR = 0x2A;



struct {
    struct spinlock lock;
    #define INPUT_BUF_SIZE 128
    char buf[INPUT_BUF_SIZE];
} rand;




uint8 lfsr_char(uint8 lfsr)
{
    uint8 bit;
    bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 4)) & 0x01;
    lfsr = (lfsr >> 1) | (bit << 7);
    return lfsr;
}


int randomread(int fd, uint64 dst, int n){
    int i;
    int counter = 0;
    char cbuf;
    for(i =0 ; i < n ; i++)
    {

        cbuf = (char)lfsr_char(LFSR);
        LFSR = (uint16)cbuf;
        if(either_copyout(fd, dst, &cbuf, 1) == -1)
        break;
        counter ++;
    }
    
    return counter;
}

int randomwrite(int fd, const uint64 src, int n){
    if(n ==1)
    {
        uint16 lfsr =lfsr_char(LFSR);
        LFSR = lfsr;
        char cbuf = (char)lfsr;
        if(either_copyout(fd, src, &cbuf, 1) == -1)
        {
            return -1;
        }
        return 1;

    }
    return -1;
}

void
randominit(void)
{
  initlock(&rand.lock, "rand");

  //uartinit();

  // connect read and write system calls
  // to consoleread and consolewrite.
  devsw[RANDOM].read = randomread;
  devsw[RANDOM].write = randomwrite;
}


