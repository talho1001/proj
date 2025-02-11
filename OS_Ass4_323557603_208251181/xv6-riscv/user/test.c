#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"
char buf[512];
void
cat(int fd)
{
  int n;

  while((n = read(fd, buf, sizeof(buf))) > 0) {
    if (write(1, buf, n) != n) {
      fprintf(2, "cat: write error\n");
      exit(1);
    }
  }
  if(n < 0){
    fprintf(2, "cat: read error\n");
    exit(1);
  }
}

int 
TEST_Random_Read()
{
  int fs2 = open("random", 2);
  if (fs2 < 0)
  {
    printf("file error\n");
  }
  char buf[5];
  int ret = read(fs2, buf, 5);
  if(ret == 5)
  {
    printf("Passed Read Champ!\n");
  }
  char buf1[1];
  int ret1;
  int j0= -1;
  int jlast= -2;
  for (int j =0; j < 256 ; j ++)
  {

    ret1 = write(fs2, buf1, 1);
    if(j == 0)
    {
      j0 = ret1;
    }
    if(j == 255)
    {
      jlast = ret1;
    }
  }
  if(j0 == jlast)
  {
    printf("Passed Write, bairly.... =%d\n", j0);
  }
  return 0;
}


int CreateTestFileFromRandom()
{
  int fs2 = open("random", 2);
  int fs5 =open("newFile", O_CREATE | O_RDWR);
  if (fs2 < 0)
  {
    printf("file error\n");
  }
  char buf[500];
  char temp[1];
  for(int i =0 ; i<500 ; i++)
  {
    read(fs2, temp, 1);
    buf[i] = temp[0];
  }
  read(fs2, buf, 500);
  write(fs5, buf, 500);
  printf("-----------Random of size 500\n");
  seek(fs5, 0, SEEK_SET);
  cat(fs5);
  seek(fs5, 0, SEEK_SET);
  printf("-----------Random of size 500\n");
  close(fs2);
  close(fs5);
  return 0;
}

int CreateFileForSeek()
{
  int fs =open("newFileSEEK", O_CREATE | O_RDWR);
  char buf[11];
  for(int i =0 ; i<10 ; i++)
  {
    buf[i] = i + 48;
  }
  buf[10] = '\n';
  for(int i =0 ; i<=10 ; i++)
  {
    write(fs, buf, 11);
  }
  close(fs);
  return fs;
}

int Test_SEEK()
{
  CreateFileForSeek();
  int fs = open("newFileSEEK", O_RDWR);
  printf("----------10X10--------------\n");
  seek(fs, 0, SEEK_SET);
  seek(fs, -1, SEEK_CUR);
  cat(fs);
  printf("----------0X0--------------\n");
  seek(fs, 1, SEEK_CUR);
  cat(fs);
  printf("---------6.5 Rows----------\n");
  seek(fs, 0, SEEK_SET);
  seek(fs, 50, SEEK_CUR);
  cat(fs);
  printf("--------2 Rows-------------\n");
  seek(fs, 0, SEEK_SET);
  seek(fs, 50, SEEK_CUR);
  seek(fs, 50, SEEK_CUR);
  cat(fs);
  printf("--------0 Rows-------------\n");
  seek(fs, 0, SEEK_SET);
  seek(fs, 50, SEEK_CUR);
  seek(fs, 50, SEEK_CUR);
  seek(fs, 50, SEEK_CUR);
  cat(fs);
  return 0;
}


int
main(int argc, char *argv[])
{
  CreateTestFileFromRandom();
  Test_SEEK();
  TEST_Random_Read();
  exit(0);
}