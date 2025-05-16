#include "codebase.h"

int main(void)
{
  i32 *x;
  f32 *f;
  i8 *str;

  Arena global_arena;
#if 0
  clear;
  trace;
  lost;

  /* arena_clear(&r_arena); */
  /* arena_raise(&r_arena, (void *)array, strlen(array)); */
#endif

  arena_init_sized(&global_arena, 400);

  arena_clear(&global_arena);
  LOG(DEBUG, "%u", global_arena.max_capacity);
  LOG(DEBUG, "%u", global_arena.alloc_position);

  x = (i32 *)arena_alloc(&global_arena, sizeof(i32));
  f = (f32 *)arena_alloc(&global_arena, sizeof(f32));
  str = arena_alloc(&global_arena, 10);

  *x = 123;
  *f = 442.43;
  memmove(str, "Hello ", 7);

  LOG(DEBUG, "Address: %p: %d", (void *)x, *x);
  LOG(DEBUG, "Address: %p: %f", (void *)f, *f);
  LOG(DEBUG, "Address: %p: %s", (void *)str, str);

  str = arena_realloc(&global_arena, str, 10, 16);

  memmove(str + 6, "world!", 7);

  LOG(DEBUG, "Address: %p: %s", (void *)str, str);
  LOG(DEBUG, "Length of str: %zu", strlen(str));

  LOG(DEBUG, "I called arena clear to set everything to zero");
  arena_clear(&global_arena);

  char *a = arena_alloc(&global_arena, 8);
  memcpy(a, "first ", 6);

  LOG(DEBUG, "Afer allocation of a");
  LOG(DEBUG, "%u", global_arena.prev_alloc_position);
  LOG(DEBUG, "%u", global_arena.alloc_position);

  char *b = arena_alloc(&global_arena, 8);
  memcpy(b, "second", 7);

  LOG(DEBUG, "Afer allocation of b");
  LOG(DEBUG, "%u", global_arena.prev_alloc_position);
  LOG(DEBUG, "%u", global_arena.alloc_position);

  b = arena_realloc(&global_arena, b, 8, 16);
  memcpy(b + 6, " extended", 10);
  printf("B: %s\n", b);

  arena_clear(&global_arena);
  printf("B: %s\n", b);

  LOG(DEBUG, "%u", global_arena.alloc_position);
  arena_clear(&global_arena);

  /* final free */
  arena_free(&global_arena);
  return (0);
}
