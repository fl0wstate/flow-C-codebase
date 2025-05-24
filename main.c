#include "codebase.h"

/* the routine function that will be runned inside the thread */
u64 test_thread(void *contex)
{
  u8 *temp;
  u8 *temp2;
  ThreadContext *thread_ctx = (ThreadContext *)contex;
  thread_ctx_init(thread_ctx);

  A_Scratch scratch = thread_ctx_get(thread_ctx);

  temp = (u8 *)arena_alloc(&scratch.arena, 128);

  sprintf((void *)temp, "Hello world from a thread");

  LOG(DEBUG, "Current buffer size: %u", scratch.arena.alloc_position);
  LOG(INFO, "%s", (u8 *)temp);

  thread_ctx_return(thread_ctx, &scratch);

  temp2 = (u8 *)arena_alloc(&scratch.arena, 200);
  LOG(DEBUG, "Current buffer size: %u", scratch.arena.alloc_position);
  sprintf((void *)temp2, "Hello world from a thread memory storage two");
  LOG(INFO, "%s", (u8 *)temp2);

  thread_ctx_free(thread_ctx);
  return 0;
}

int main(void)
{
  i32 *x;
  f32 *f;
  i8 *str;

  ThreadContext worker_ctx = {0};
  OS_thread worker = OS_ThreadCreate(test_thread, &worker_ctx);
  OS_threadWaitForJoin(&worker);
  LOG(INFO, "You are now entering the main thread");

#if 0
  clear;
  trace;
  lost;

  /* arena_clear(&r_arena); */
  /* arena_raise(&r_arena, (void *)array, strlen(array)); */

  Arena global_arena;
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

  memmove(str + 6, "w orld!", 7);

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
#endif
  return (0);
}
