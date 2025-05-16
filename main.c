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


  /* LOG(DEBUG, "%s", r_arena.memory); */

  for (i = 0; i < 10; i++)
  {
    /* char *str; */

    // Reset all arena offsets for each loop
    /* arena_clear(&global_arena); */

    /* str = arena_alloc(&global_arena, 10); */

    /* printf("%p: %s\n", str, str); */

    /* printf("%p: %s\n", str, str); */


  f = (float *)arena_alloc(&global_arena, sizeof(float));

  *f = 987;

  printf("%p: %d\n", (void *)x, *x);
  printf("%p: %f\n", (void *)f, *f);


  LOG(DEBUG, "%u", global_arena.max_capacity);
  LOG(DEBUG, "%u", global_arena.alloc_position);
  }

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

  LOG(DEBUG, "I called arena clear to set everything to zero");
  arena_clear(&global_arena);

  LOG(DEBUG, "%u", global_arena.alloc_position);
  str = arena_realloc(&global_arena, str, 10, 16);
  memmove(str + 6, "world!", 7);

  LOG(DEBUG, "char at 6: %c", str[6]);
  LOG(DEBUG, "char at 13: %c", str[11]);
  LOG(DEBUG, "Address: %p: %s", (void *)str, str);
  LOG(DEBUG, "Length of str: %zu", strlen(str));

  LOG(DEBUG, "%u", global_arena.alloc_position);
  arena_clear(&global_arena);
  arena_free(&global_arena);

  LOG(DEBUG, "%u", global_arena.alloc_position);
  return (0);
}
