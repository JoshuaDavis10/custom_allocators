#ifndef FREE_LIST_ALLOCATOR_H
#define FREE_LIST_ALLOCATOR_H

#include "linux_util.c"

/* 
 * @brief: initializes free list allocator by getting 'memory_size' bytes
 * of memory from OS (linux) and setting up a free list in that memory
 * by creating one large block (header + remaining bytes)
 *
 * @param memory_size: 64 bit unsigned integer -> number of bytes of
 * memory for the free list allocator to use
 *
 * @return: returns 1 (true) on successful initialization, otherwise
 * returns 0 (false) and prints a warning message.
 */
b32 free_list_allocator_initialize(u64 memory_size);

/* 
 * @brief: finds an appropriate block within the free list to use for 
 * an allocation of size 'size' (or creates one if necessary) and returns
 * the address of that block. writing 'size' or more bytes past the 
 * returned address will result in undefined behaviour, as it will
 * overwrite the header of the next block.
 *
 * @param size: 64 bit unsigned integer -> number of bytes in the block
 * (not including header). Effectively the amount of memory that the 
 * user is "allocating" by calling free_list_allocate
 *
 * @return: returns the address of the block used for the allocation.
 * On failure to allocate, will return 0 (NULL pointer) and print an
 * error message.
 */
void *free_list_allocate(u64 size);

/* 
 * @brief: Frees the free list block at address 'address' if the block
 * is not already free. this is simply done by changing a header value
 * for the block (is_free). After the block is freed, free_list_free
 * will attempt to merge the freed block with adjacent free blocks if
 * possible (to avoid fragmentation)
 *
 * @param address: void pointer -> address of the block to free. If 
 * 'address' does not point to a valid block, undefined behavior will 
 * result, although in some cases an error message will be printed.
 *
 * @return: returns 1 (true) on a successful free. IMPORANT: will
 * return 1 (true) in some cases when an invalid 'address' parameter is
 * passed (i.e. address does not point to an actual free list block,
 * which is undefined behavior to begin with). Returns 0 (false) if the
 * block pointed to by 'address' is not free
 *
 * p.s. essentially, if the value at:
 *
 *		address - (offset to get to is_free value) 
 *
 * is not 0, free_list_free will return 0 (false). This could be either
 * because: 
 *		1) the block pointed to by 'address' is already free
 *		2) 'address' doesn't actually point to a block, but just happens
 *		to have a value of >= 1 at: 
 *
 *			address - (offset to get to is_free value)
 *
 * so, in cases where 'address' points to an invalid block that happens
 * to have a value of 0 at:
 *
 *			address - (offset to get to is_free value)
 *
 * free_list_free will "free" the block, resulting in undefined behavior
 * and no error message printed
 */
b32 free_list_free(void *address);

#endif
