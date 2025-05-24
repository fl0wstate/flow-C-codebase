#include "codebase.h"

static pthread_key_t linux_pthread_context_key;

/* OS_init - used to create the pthread_key that's going to be avalilable in all
 * the threads max_created
 *
 * Return: void (ensures that the pthread_key value is created something like a
 * global vairable)
 * */
void OS_Init(void) { pthread_key_create(&linux_pthread_context_key, NULL); }

/* OS_threadContextSet - used to initialize the pthread_key with a value, in our
 * case it's the context(ThreadContext Structure)
 *
 * @context: A void pointer to the ThreadContext Struct (an instance of it).
 *
 * Return: NULL
 * */
void OS_threadContextSet(void *context)
{
  pthread_setspecific(linux_pthread_context_key, context);
}

/* OS_threadContextGet - gets you the value set on the pthread_key, basically
 * returns you the context(ThreadContext Struct)
 *
 * Reeturn: void pointer that which is to be casted into a ThreadContext Struct
 * */
void *OS_threadContextGet(void)
{
  return pthread_getspecific(linux_pthread_context_key);
}

/* thread_ctx_init - initializes both the arean, and sets the context to the
 * pthread_key
 *
 * @ctx: ThreadContext Struct pointer to be set as the value for the
 * linux_pthread_context_key (pthread_key)
 *
 * Return: void
 * */
void thread_ctx_init(ThreadContext *ctx)
{
  arena_init(&ctx->arena);
  OS_threadContextSet(ctx);
}

/*
 * thread_ctx_get -  */
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
    LOG(DEBUG, "%u", ctx->max_created);
    return scratch;
  }
}

/* thread_ctx_free - frees the arena, and sets linux_pthread_context_key with
 * the new ctx again
 *
 * @ctx: ThreadContext Struct pointer to be set as the value for the
 * linux_pthread_context_key (pthread_key)
 *
 * Return: void
 * */
void thread_ctx_free(ThreadContext *ctx)
{
  arena_free(&ctx->arena);
  OS_threadContextSet(ctx);
}

/* */
void thread_ctx_reset(ThreadContext *ctx, A_Scratch *scratch)
{
  scratch->arena.alloc_position = 0;
}

/* */
void thread_ctx_return(ThreadContext *ctx, A_Scratch *scratch)
{
  scratch_ll *prev_head = ctx->list;
  ctx->list = (scratch_ll *)scratch->arena.memory;
  ctx->list->next = prev_head;
}

/* */
OS_thread OS_ThreadCreate(thread_func *start, void *context)
{

  OS_thread thread_ = {0};

  Linux_thread *linux_thread = (Linux_thread *)&thread_;

  pthread_create(&linux_thread->thread, NULL, (linux_thread_routine *)start,
                 context);

  return thread_;
}

/* */
void OS_threadWaitForJoin(OS_thread *other)
{
  Linux_thread *linux_thread = (Linux_thread *)other;

  pthread_join(linux_thread->thread, NULL);
}
