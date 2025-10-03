#include "linux_util.c"

#include "free_list_allocator.c"
#include "red_black_allocator.c"

#define ALLOCATOR_MEMORY_SIZE (4096 * 4)

int main(void)
{
	if(!free_list_allocator_initialize(ALLOCATOR_MEMORY_SIZE))
	{
		LOG_ERROR("Failed to initialize free list allocator."
				" Terminating.");
		return(-1);
	}

	LOG_INFO("Initialized free list allocator with memory of size:"
			" %u bytes @%p.", ALLOCATOR_MEMORY_SIZE, free_list.address);

	free_list_allocate(4096);
	print_free_list();

	return(0);
}
