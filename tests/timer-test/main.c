#include <stdio.h>
#include <stdint.h>
#include <NDL.h>

int main() {
  printf("Start\n");
  NDL_Init(0);
  uint32_t timer = NDL_GetTicks();
  printf("Init timer: %d ms!\n", timer);
  while (1) {
    uint32_t current_timer = NDL_GetTicks();
    if (current_timer - timer > 500) {
      timer = current_timer;
      printf("Hello World from Navy-apps!\n");
    }
  }
  return 0;
}
