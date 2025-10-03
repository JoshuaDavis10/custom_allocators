typedef struct {
	void *address;
	u64 size;
} free_list_data;

#define FREE_LIST_BLOCK_HEADER_SIZE \
(sizeof(void*) + sizeof(u64) + sizeof(b32))
/* NOTE: yes, above and below are the same value */
#define FREE_LIST_BLOCK_NEXT_OFFSET \
(sizeof(void*) + sizeof(u64) + sizeof(b32))

#define FREE_LIST_BLOCK_SIZE_OFFSET \
(sizeof(u64) + sizeof(b32))

#define FREE_LIST_BLOCK_IS_FREE_OFFSET \
(sizeof(b32))

static free_list_data free_list = {0};

/* utility functions. NOT "user-facing" */
static void free_list_block_set_next(void *address, void *next);
static void free_list_block_set_size(void *address, u64 size);
static void free_list_block_set_is_free(void *address, b32 is_free);
static void *free_list_block_get_next(void *address);
static u64 free_list_block_get_size(void *address);
static b32 free_list_block_get_is_free(void *address);

/* user-facing. sets up allocator */
b32 free_list_allocator_initialize(u64 memory_size)
{
	/* don't initialize the allocator if it's already initialized */
	void *memory_address;
	if(free_list.address)
	{
		LOG_WARN("(free_list_allocator_initialize) - free list memory"
				" region has already been initialized. returning 0");
		return false;
	}
	
	/* determine number of pages needed and get them from OS */
	u32 num_pages = (memory_size / linux_get_page_size()) + 1;
	memory_address = linux_get_memory_pages(num_pages);

	free_list.address = (char*)memory_address + 
						FREE_LIST_BLOCK_HEADER_SIZE;
	free_list.size = memory_size;

	/* initialize the first block */
	free_list_block_set_next(free_list.address, 0);
	free_list_block_set_size(
			free_list.address, 
			memory_size - FREE_LIST_BLOCK_HEADER_SIZE);
	free_list_block_set_is_free(free_list.address, true);
	return true;
}

/* user-facing. allocates memory */
void *free_list_allocate(u64 size)
{
	if(size > free_list.size)
	{
		LOG_ERROR("(free_list_allocate) - Cannot allocate %u bytes of"
			" memory. Allocator only has %u bytes of memory to operate"
			" with. Returning 0 (NULL pointer).", size, free_list.size);
		return(0);
	}

	/* loop through blocks until a large enough block is found */
	void *current_block = free_list.address;
	while(current_block != 0)
	{
		/* current block is available */
		if(free_list_block_get_is_free(current_block))
		{
			u64 block_size = free_list_block_get_size(current_block);

			/* block is big enough to split */
			if(block_size > size + FREE_LIST_BLOCK_HEADER_SIZE)
			{
				/* split the block */
				void *new_block = 
					(void*) ( (char*)current_block + size + 
					FREE_LIST_BLOCK_HEADER_SIZE );
				void *tmp_current_block_next = 
					free_list_block_get_next(current_block);

				/* block that we are returning */
				free_list_block_set_size(current_block, size);
				free_list_block_set_is_free(current_block, false);
				free_list_block_set_next(current_block, new_block);

				/* new block */
				free_list_block_set_size(new_block, 
						block_size - size - FREE_LIST_BLOCK_HEADER_SIZE);
				free_list_block_set_is_free(new_block, true);
				free_list_block_set_next(new_block, 
						tmp_current_block_next);

				return current_block;
			}
			/* block is too small to split but still big enough */
			if(block_size >= size)
			{
				free_list_block_set_is_free(current_block, false);
				return current_block;
			}
		}

		/* go to next block in list since current one won't work */
		current_block = free_list_block_get_next(current_block);
	}

	/* no blocks found that will work at this point */
	LOG_ERROR("(free_list_allocate) - Cannot allocate %u bytes of"
		" memory. No blocks large enough could be found."
		" Returning 0 (NULL pointer).", size);
	return(0);
}

/* user-facing. frees memory */
b32 free_list_free(void *address)
{
	/* TODO: code here */
	return true;
}

static void free_list_block_set_next(void *address, void *next)
{
	void **next_ptr = 
		(void**)((char*)address - FREE_LIST_BLOCK_NEXT_OFFSET);
	*next_ptr = next;
}

static void free_list_block_set_size(void *address, u64 size)
{
	u64 *size_ptr = 
		(u64*)((char*)address - FREE_LIST_BLOCK_SIZE_OFFSET);
	*size_ptr = size;
}

static void free_list_block_set_is_free(void *address, b32 is_free)
{
	b32 *is_free_ptr =
		(b32*)((char*)address - FREE_LIST_BLOCK_IS_FREE_OFFSET);
	*is_free_ptr = is_free;
}

static void *free_list_block_get_next(void *address)
{
	void **next_ptr = 
		(void**)((char*)address - FREE_LIST_BLOCK_NEXT_OFFSET);
	return *next_ptr;
}

static u64 free_list_block_get_size(void *address)
{
	u64 *size_ptr = 
		(u64*)((char*)address - FREE_LIST_BLOCK_SIZE_OFFSET);
	return *size_ptr;
}
static b32 free_list_block_get_is_free(void *address)
{
	b32 *is_free_ptr =
		(b32*)((char*)address - FREE_LIST_BLOCK_IS_FREE_OFFSET);
	return *is_free_ptr;
}

/* XXX: temp debugging function, delete later */
void print_free_list()
{
	void *current_block = free_list.address;
	LOG_TRACE("PRINTING FREE LIST BELOW...");
	while(current_block != 0)
	{
		LOG_TRACE("\nBLOCK @%p:\n\tnext: %p\n\tsize: %u\n\tis_free: %d"
				  "\n\n", current_block,
				  free_list_block_get_next(current_block),
				  free_list_block_get_size(current_block),
				  free_list_block_get_is_free(current_block));
		current_block = free_list_block_get_next(current_block);
	}
}
