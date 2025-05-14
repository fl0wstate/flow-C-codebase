#include "codebase.h"

/* This will be where all the memory allocation will be created and happening.
 */

u8 ispoweroftwo(u64 x) { return (x & (x - 1)); }

/*
 * A function that returns a pointer to a paged memory on the virtual space
 * @size: 64bit size which will be used to allocate the memory size
 *  - PROT_NONE -> means the pages cannont be accessed
 *  - MAP_PRIVATE -> mapping are not visible to other processes
 *  - MAP_ANONYMOUS -> basically means all the contents are intialized to zero
 * (zero paged memory)
 */
void *map_memory(u64 size)
{
  return mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

/* makes the memory readable and writable */
void commit_memory(void *mapped_memory, u64 size)
{
  mprotect(mapped_memory, size, PROT_READ | PROT_WRITE);
}

/* makes the memory unreadable and unwritable */
void uncommit_memory(void *mapped_memory, u64 size)
{
  mprotect(mapped_memory, size, PROT_NONE);
}

/* basically the same as freeing the mapped memory */
void unmap_memory(void *mapped_memory, u64 size)
{
  munmap(mapped_memory, size);
}

/* memory aligment */
u64 mem_aligment(u64 ptr, u64 align)
{
  u64 apointer, size, modulo;
  assert(ispoweroftwo(size));

  apointer = ptr;
  size = align;

  modulo = apointer & (size - 1);

  if (modulo != 0)
    apointer += size - modulo;

  /* an althernative since we are just rounding up the unaligned memory to the
   * nearest multiple of align */
  /* apointer = (apointer + (align - 1)) & ~(align - 1); */

  return (apointer);
}

/* arena Allocation */
void *arena_alloc(Arena *arena, u64 size)
{
  void *backing_buffer = NULL;

  /* align the memory of the backing buffer to the nearest multipl of align */
  size = mem_aligment(size, DEFAULT_ALIGNMENT);

  /* actual memory allocation */
  if (arena->alloc_position + size > arena->commit_position)
  {
    u64 commit_size = size;

    /* rounding up the commit_size to the nearest multiple of
     * M_ARENA_COMMIT_SIZE */
    commit_size += (M_ARENA_COMMIT_SIZE - 1);
    commit_size -= commit_size % M_ARENA_COMMIT_SIZE;

    if (arena->commit_position >= arena->max_capacity)
      assert(0 && "Arena is out of memory");
    else
      arena->commit_position += commit_size;
  }
  /* make sure you are starting at the first memory address  */
  /* "hello" -> arena->memory -> 'h' and arena->alloc_position = 0; */
  backing_buffer = arena->memory + arena->alloc_position;
  arena->alloc_position += size;
  return backing_buffer;
}

/* arena memory initialization */
void *arena_alloc_zero(Arena *arena, u64 size)
{
  void *allocated_memory = arena_alloc(arena, size);
  memset(allocated_memory, 0, size);
  return allocated_memory;
}

/* clearing the arena */
void arena_dealloc(Arena *arena, u64 size)
{
  if (size > arena->alloc_position)
    size = arena->alloc_position;
  arena->alloc_position -= size;
}

void arena_dealloc_to(Arena *arena, u64 position)
{
  if (position > arena->max_capacity)
    position = arena->max_capacity;
  if (position < 0)
    position = 0;

  arena->alloc_position = position;
}

void *arena_raise(Arena *arena, void *ptr, u64 size)
{
  void *allocated_memory = arena_alloc(arena, size);
  memcpy(allocated_memory, ptr, size);
  return allocated_memory;
}

/* use it to hold strings in memory important strings */
void *arena_alloc_array_sized(Arena *arena, u64 elem_size, u64 count)
{
  return arena_alloc(arena, elem_size * count);
}

/* initialization of the arena */
void arena_init(Arena *arena)
{
  ZeroUpStructMem(arena, Arena);
  arena->max_capacity = sizeof(void *) * 1024;
  arena->memory = map_memory(arena->max_capacity);
  arena->alloc_position = 0;
  arena->commit_position = 0;
}
void arena_init_sized(Arena *arena, u64 capacity)
{
  ZeroUpStructMem(arena, Arena);
  arena->max_capacity = capacity;
  arena->memory = map_memory(arena->max_capacity);
  arena->alloc_position = 0;
  arena->commit_position = 0;
}

/* you have already created the memory and you want to create a new space */
void arena_clear(Arena *arena) { arena_dealloc(arena, arena->max_capacity); }

/* completely remove the memory page from the virtual memory address page */
void arena_free(Arena *arena)
{
  unmap_memory(arena->memory, arena->max_capacity);
}
