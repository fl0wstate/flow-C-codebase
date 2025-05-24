#ifndef __CODE_BASE__
#define __CODE_BASE__

#include <assert.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>

typedef enum LOG_LEVEL
{
  PROMPT,
  INFO,
  DEBUG,
  ERROR
} logll;

/* ANSI_COLORS */
#define ANSI_RESET_ALL "\x1b[0m"
#define ANSI_COLOR_BLACK "\x1b[30m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_WHITE "\x1b[37m"

/* Unsigned int types */
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

/* Regular int types */
typedef char i8;
typedef short i16;
typedef int i32;
typedef long long i64;

/* floating point values */
typedef float f32;
typedef double f64;

/* nob configuration */
#define NOB_IMPLEMENTATION

/* Memory alignment size */
#define DEFAULT_ALIGNMENT 8

/* boolen flags */
#define true 1
#define false 0

/* Maximum unsinged int value */
#define max_u32 4294967295

#if defined(COMPILER_CLANG)
#define FILE_NAME __FILE_NAME__
#else
#define FILE_NAME __FILE__
#endif

#define fn(s)                                                                  \
  do                                                                           \
  {                                                                            \
    s                                                                          \
  } while (0);

#define clear fflush(stdout);
#define trace fn(LOG(DEBUG, "Tracing:%s @line:%d", FILE_NAME, __LINE__); clear;)
#define lost                                                                   \
  fn(LOG(ERROR, "How the heck did we get and error in %s on line %d",          \
         FILE_NAME, __LINE__);                                                 \
     clear;)

#define MAX_ARENA_SIZE (u64)(sizeof(u64) * 2048)
#define M_ARENA_COMMIT_SIZE (u64)(8 * 1024)
#define M_SCRATCH_SIZE (u64)(1024)

#define ZeroUpMem(d, s) memset((d), 0, (s))
#define ZeroUpStructMem(d, s) ZeroUpMem((d), sizeof(s))

/* for loggin */
void LOG(logll level, const char *format, ...);

typedef struct arena
{
  u8 *memory;
  u64 max_capacity;
  u64 alloc_position;
  u64 commit_position;
  u64 prev_alloc_position;
} Arena;

/*Poll Arenas*/

/* for aligment */
u64 mem_aligment(u64 ptr, u64 align);
u8 ispoweroftwo(u64);

void *arena_alloc(Arena *, u64);
void *arena_alloc_zero(Arena *, u64 size);
void arena_dealloc(Arena *, u64 size);
void arena_dealloc_to(Arena *, u64 position);
void *arena_raise(Arena *, void *, u64);
void *arena_alloc_array_sized(Arena *, u64 element_size, u64 count);
void *arena_realloc(Arena *arena, void *old_memory, u64 old_size, u64 new_size);
/* Only for already existing arenas */
void arena_init_sized(Arena *, u64);
void arena_init(Arena *);
void arena_clear(Arena *);
void arena_free(Arena *);

/* memory mapping other than using malloc and free */
void *map_memory(u64);
void commit_memory(void *, u64);
void uncommit_memory(void *, u64);
void unmap_memory(void *, u64);

/* threadContext */
typedef u64 thread_func(void *context);
typedef void *linux_thread_routine(void *context);

/* OS_thread structure */
typedef struct OS_thread
{
  u64 v[1];
} OS_thread;

/* Linux thread Structure */
typedef struct Linux_thread
{
  pthread_t thread;
} Linux_thread;

/* Scratch list Structure */
typedef struct scratch_list
{
  u32 index;
  struct scratch_list *next;
} scratch_ll;

/*Scratch Arenas*/
typedef struct s_arena
{
  Arena arena;
  u32 index;
  u64 position;
} A_Scratch;

/* ThreadContext Structure */
typedef struct ThreadContext
{
  Arena arena;
  u32 max_created;
  scratch_ll *list;
} ThreadContext;

/* initialization and cleaning of a thread */
void thread_ctx_init(ThreadContext *ctx);
void thread_ctx_free(ThreadContext *ctx);

/* creating an arena inside the thread */
A_Scratch thread_ctx_get(ThreadContext *ctx);
void thread_ctx_reset(ThreadContext *ctx, A_Scratch *);
void thread_ctx_return(ThreadContext *ctx, A_Scratch *);

/* creating and joining a the thread */
OS_thread OS_ThreadCreate(thread_func *start, void *context);
void OS_threadWaitForJoin(OS_thread *other);

#endif
