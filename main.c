#include "codebase.h"

int main(void)
{
  u64 *x;
  f32 *f;
  i32 i;
  Arena global_arena;

  LOG(INFO, "Hello world");
#if 0
  clear;
  trace;
  lost;


  /* LOG(DEBUG, "%s", r_arena.memory); */

  for (i = 0; i < 10; i++)
  {
    /* char *str; */

    // Reset all arena offsets for each loop
    /* arena_clear(&global_arena); */

    /* str = arena_alloc(&global_arena, 10); */

    /* printf("%p: %s\n", str, str); */

    /* str = arena_realloc(&global_arena, str, 10, 16); */
    /* memmove(str + 7, " world!", 7); */
    /* printf("%p: %s\n", str, str); */


  f = (float *)arena_alloc(&global_arena, sizeof(float));

  *f = 987;
  /* memmove(str, "Hello ", 7); */

  printf("%p: %d\n", (void *)x, *x);
  printf("%p: %f\n", (void *)f, *f);


  LOG(DEBUG, "%u", global_arena.max_capacity);
  LOG(DEBUG, "%u", global_arena.alloc_position);
  }

  /* arena_clear(&r_arena); */
  /* arena_raise(&r_arena, (void *)array, strlen(array)); */
#endif

  arena_init_sized(&global_arena, 400);
  arena_dealloc(&global_arena, 200);

  LOG(DEBUG, "%u", global_arena.max_capacity);
  LOG(DEBUG, "%u", global_arena.alloc_position);

  x = (u64 *)arena_alloc(&global_arena, sizeof(i32));
  f = (f32 *)arena_alloc(&global_arena, sizeof(f32));

  *x = 123;
  *f = 53.442;

  LOG(DEBUG, "Address: %p: %llu", (void *)x, x);
  LOG(DEBUG, "Address: %p: %llu", (void *)f, f);

  LOG(DEBUG, "%u", global_arena.max_capacity);
  LOG(DEBUG, "%u", global_arena.alloc_position);
  arena_clear(&global_arena);
  return (0);
}
