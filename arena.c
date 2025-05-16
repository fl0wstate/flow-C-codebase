#include "codebase.h"

/* This will be where all the memory allocation will be created and happening.
 * if a number is divisible by two it will switch all the bits into zero
 *
 * example: 8 & (8 - 1)
 * 1000
 * 0001
 * ----
 * 0000
 *
 * returns false multiple of 2
 */

u8 ispoweroftwo(u64 x) { return (x & (x - 1)) == false; }

/*
 * A function that returns a pointer to a paged memory on the virtual space
 * @size: 64bit size which will be used to allocate the memory size
 *  - PROT_NONE -> means the memory pages cannont be accessed
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

/* memory aligment - aligns the inputed memory size to be a multiple of two for
 * faster memory reading and writing. better for opitimization
 *
 * @ptr: pointer to the user inputed memory
 * @align: pointer to the default memory size aligment to be used
 *
 * Return: an aligned size that is a multiple of the DEFAULT_ALIGNMENT
 * provided which is 8bytes(64bits)
 */
u64 mem_aligment(u64 ptr, u64 align)
{
  u64 apointer;
  u64 size;
  u64 modulo;

  assert(ispoweroftwo(align));

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

/* arena_alloc - returns a pointer to an allocated memory inputed by the user
 *
 * @arena: Pointer to the arena struct
 * @size: size of the memory to allocate
 *
 * Returns: a void pointer with the allocated memory size
 */

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
    {
      commit_memory(arena->memory + arena->commit_position, commit_size);
      arena->commit_position += commit_size;
    }
  }
  /* make sure you are starting at the first memory address  */
  /* "hello" -> arena->memory -> 'h' and arena->alloc_position = 0; */
  backing_buffer = arena->memory + arena->alloc_position;
  arena->alloc_position += size;
  return backing_buffer;
}

/* arena_alloc_zero - arena memory initialization
 *
 * @arena: a pointer to the arena struct
 * @size: 8byte memory size to be allocated
 *
 * Returns: a void pointer to the allocated memory which is initialized to zero
 */
void *arena_alloc_zero(Arena *arena, u64 size)
{
  void *allocated_memory = arena_alloc(arena, size);
  memset(allocated_memory, 0, size);
  return allocated_memory;
}

/* arena_dealloc -  clearing the arena
 *
 * @arena: a pointer to the arena struct
 * @size: 8byte memory size to be allocated
 *
 * Returns: NULL
 */
void arena_dealloc(Arena *arena, u64 size)
{
  if (size > arena->alloc_position)
    size = arena->alloc_position;
  arena->alloc_position -= size;
}

/* arena_dealloc - clearing the arena
 *
 * @arena: a pointer to the arena struct
 * @position: 8byte sized position the memory size should be reduced to
 *
 * Returns: NULL
 */
void arena_dealloc_to(Arena *arena, u64 position)
{
  if (position > arena->max_capacity)
    position = arena->max_capacity;
  if (position < 0)
    position = 0;

  arena->alloc_position = position;
}

/* arena_raise - allocate the data from the void ptr to the arena
 *
 * @arena: a pointer to the arena struct
 * @ptr: pointer to some random data to be copied on the arena
 * @size: size of the void pointer memory
 *
 * Returns: a pointer to the allocated memory in the arena
 */
void *arena_raise(Arena *arena, void *ptr, u64 size)
{
  void *allocated_memory = arena_alloc(arena, size);
  memcpy(allocated_memory, ptr, size);
  return allocated_memory;
}

/* arena_alloc_array_sized - allocate memory on the arena which is sized to hold
 * the actual elements inside of the sized array
 *
 * @arena: a pointer to the arena struct
 * @elem_size: size of each element in the array
 * @count: size of the inputed array
 *
 * Returns: a pointer to the allocated memory in the arena
 */
void *arena_alloc_array_sized(Arena *arena, u64 elem_size, u64 count)
{
  return arena_alloc(arena, elem_size * count);
}

/**
 * arena_realloc - it's going to increase the memory size of the pointer given
 *
 * @arena: pointer to the arena struct
 * @old_memory: pointer to the old pointer
 * @old_size: size of the old allocated memory
 * @new_size: size of the new allocated memory on the arena
 * @align: size of the memory alignment
 *
 * Returns: void pointer to the newly created memory inside the arena
 */
void *arena_realloc(Arena *arena, void *old_memory, u64 old_size, u64 new_size)
{
  u8 *old_mem = old_memory;

  if (!old_mem || old_size == 0)
    return arena_alloc(arena, new_size);

  if (arena->memory <= old_mem &&
      old_mem <= (arena->memory + arena->max_capacity))
  {
    if (arena->memory + arena->alloc_position == old_mem + old_size)
    {
      arena->alloc_position = (old_mem - arena->memory) + new_size;

      if (new_size > old_size)
        memset(old_mem + old_size, 0, new_size - old_size);

      return old_mem;
    }
    else
    {
      void *new_mem = arena_alloc(arena, new_size);

      u64 copy_size = old_size < new_size ? old_size : new_size;

      memmove(new_mem, old_memory, copy_size);
      return new_mem;
    }
  }
  else
  {
    assert(0 && "Memory is out of bounds of the buffer in this arena");
    return NULL;
  }
}

/* arena_init - initialization of the arean with the default Memory size 8KB
 *
 * @arena: a pointer to the arena struct
 *
 * Returns; NULL
 */
void arena_init(Arena *arena)
{
  ZeroUpStructMem(arena, Arena);
  arena->max_capacity = sizeof(void *) * 1024;
  arena->memory = map_memory(arena->max_capacity);
  arena->alloc_position = 0;
  arena->commit_position = 0;
}

/* arena_init - initialization of the arena with a suggested size
 *
 * @arena: a pointer to the arena struct
 * @capacity: size of the arena memory to be allocated
 *
 * Returns; NULL
 */
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
