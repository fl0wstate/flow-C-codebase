#include "codebase.h"

static pthread_key_t linux_pthread_context_key;

void OS_Init(void) { pthread_key_create(&linux_pthread_context_key, NULL); }

void OS_threadContextSet(void *context)
{
  pthread_setspecific(linux_pthread_context_key, context);
}

void *OS_threadContextGet(void)
{
  return pthread_getspecific(linux_pthread_context_key);
}

void thread_ctx_init(ThreadContext *ctx)
{
  arena_init(&ctx->arena);
  OS_threadContextSet(ctx);
}

void thread_ctx_free(ThreadContext *ctx)
{
  arena_free(&ctx->arena);
  OS_threadContextSet(ctx);
}

A_Scratch thread_ctx_get(ThreadContext *ctx)
{
  if (!ctx->list)
  {
    A_Scratch scratch = {0};
    void *ptr;

    ptr = arena_alloc(&ctx->arena, M_SCRATCH_SIZE);
    scratch.arena.memory = ptr;
    scratch.arena.alloc_position = 0;
    scratch.arena.commit_position = M_SCRATCH_SIZE;
    scratch.arena.prev_alloc_position = 0;
    scratch.arena.max_capacity = M_SCRATCH_SIZE;

    ctx->max_created++;
    return scratch;
  }
  else
  {
    A_Scratch scratch = {0};

    scratch.arena.memory = (u8 *)ctx->list;
    scratch.arena.alloc_position = 0;
    scratch.arena.commit_position = M_SCRATCH_SIZE;
    scratch.arena.prev_alloc_position = 0;
    scratch.arena.max_capacity = M_SCRATCH_SIZE;

    ctx->list = ctx->list->next;
    return scratch;
  }
}

void thread_ctx_reset(ThreadContext *ctx, A_Scratch *scratch)
{
  scratch->arena.alloc_position = 0;
}

void thread_ctx_return(ThreadContext *ctx, A_Scratch *scratch)
{
  scratch_ll *prev_head = ctx->list;
  ctx->list = (scratch_ll *)scratch->arena.memory;
  ctx->list->next = prev_head;
}

OS_thread OS_ThreadCreate(thread_func *start, void *context)
{

  OS_thread thread_ = {0};

  Linux_thread *linux_thread = (Linux_thread *)&thread_;

  pthread_create(&linux_thread->thread, NULL, (linux_thread_routine *)start,
                 context);

  return thread_;
}

void OS_threadWaitForJoin(OS_thread *other)
{
  Linux_thread *linux_thread = (Linux_thread *)other;

  pthread_join(linux_thread->thread, NULL);
}
