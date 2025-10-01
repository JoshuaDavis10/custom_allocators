static void *free_list_memory_region_address = 0;
static u64 free_list_memory_region_size = 0;

static b32 free_list_allocator_initialize(u64 memory_region_size)
{
	if(free_list_memory_region_address)
	{
		LOG_WARN("free_list_allocator_initialize: free list memory region"
				" has already been initialized. returning 0");
		return false;
	}
	
	u32 num_pages = (memory_region_size / linux_get_page_size()) + 1;
	free_list_memory_region_address = 
		linux_get_memory_pages(num_pages);
	free_list_memory_region_size = memory_region_size;
	/* TODO: setup first block which is just the whole region at first */
	return true;
}

static void *free_list_allocate(u64 size)
{
	/* TODO: code here */
	return(0);
}

static b32 free_list_free(void *address)
{
	/* TODO: code here */
	return true;
}
