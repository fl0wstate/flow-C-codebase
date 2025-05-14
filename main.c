#include "codebase.h"

int main(void)
{
  char test[512] = {0};
  Arena r_arena;
  LOG(INFO, "Hello world");
#if 0
  clear;
  trace;
  lost;
#endif

  arena_init_sized(&r_arena, 512);

  LOG(DEBUG, "%u", r_arena.max_capacity);

  return (0);
}
