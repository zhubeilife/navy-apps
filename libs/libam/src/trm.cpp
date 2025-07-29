// copy from https://gitlab.eduxiji.net/educg-group-22022-2210132/T202410487992568-3560/-/blob/pa3/navy-apps/libs/libam/src/trm.cpp
#include <am.h>
#include <stdlib.h>

Area heap;

void putch(char ch) {
  printf("%c", ch);
}

void halt(int code) {
  exit(0);
  //asm volatile("li.w $a7, 0; syscall 0");
}

