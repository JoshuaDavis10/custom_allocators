/* logging */
#include <stdio.h> /* for: printf, vsnprintf */
#include <stdarg.h> /* for: va_start, va_end, va_list */

#define MAX_LOGGER_MESSAGE_SIZE 16384

#define LOGGER_ERROR_ENABLED 1
#define LOGGER_WARN_ENABLED 1
#define LOGGER_INFO_ENABLED 1
#define LOGGER_DEBUG_ENABLED 1
#define LOGGER_TRACE_ENABLED 1

typedef enum {
	LOGGER_ERROR,
	LOGGER_WARN,
	LOGGER_INFO,
	LOGGER_DEBUG,
	LOGGER_TRACE,
	LOGGER_TYPES_COUNT
} logger_message_type;

#if LOGGER_ERROR_ENABLED
void LOG_ERROR(const char *message, ...) {
	char output[MAX_LOGGER_MESSAGE_SIZE];
	va_list arg_ptr;
	va_start(arg_ptr, message);
	/* NOTE: vsnprintf may not be in C89 standard */
    vsnprintf(output, MAX_LOGGER_MESSAGE_SIZE, message, arg_ptr);
	va_end(arg_ptr);
	printf("\e[1;31m[ERROR]:\e[0;31m %s\e[0;37m\n", output);
}
#else
void LOG_ERROR(const char *message, ...) {
}
#endif

#if LOGGER_WARN_ENABLED
void LOG_WARN(const char *message, ...) {
	char output[MAX_LOGGER_MESSAGE_SIZE];
	va_list arg_ptr;
	va_start(arg_ptr, message);
    vsnprintf(output, MAX_LOGGER_MESSAGE_SIZE, message, arg_ptr);
	va_end(arg_ptr);
	printf("\e[1;33m[WARN]:\e[0;33m %s\e[0;37m\n", output);
}
#else
void LOG_WARN(const char *message, ...) {
}
#endif

#if LOGGER_INFO_ENABLED
void LOG_INFO(const char *message, ...) {
	char output[MAX_LOGGER_MESSAGE_SIZE];
	va_list arg_ptr;
	va_start(arg_ptr, message);
    vsnprintf(output, MAX_LOGGER_MESSAGE_SIZE, message, arg_ptr);
	va_end(arg_ptr);
	printf("\e[1;32m[INFO]:\e[0;32m %s\e[0;37m\n", output);
}
#else
void LOG_INFO(const char *message, ...) {
}
#endif

#if LOGGER_DEBUG_ENABLED
void LOG_DEBUG(const char *message, ...) {
	char output[MAX_LOGGER_MESSAGE_SIZE];
	va_list arg_ptr;
	va_start(arg_ptr, message);
    vsnprintf(output, MAX_LOGGER_MESSAGE_SIZE, message, arg_ptr);
	va_end(arg_ptr);
	printf("\e[1;34m[DEBUG]:\e[0;34m %s\e[0;37m\n", output);
}
#else
void LOG_DEBUG(const char *message, ...) {
}
#endif

#if LOGGER_TRACE_ENABLED
void LOG_TRACE(const char *message, ...) {
	char output[MAX_LOGGER_MESSAGE_SIZE];
	va_list arg_ptr;
	va_start(arg_ptr, message);
    vsnprintf(output, MAX_LOGGER_MESSAGE_SIZE, message, arg_ptr);
	va_end(arg_ptr);
	printf("\e[1;37m[TRACE]:\e[0;37m %s\n", output);
}
#else
void LOG_TRACE(const char *message, ...) {
}
#endif

/* assertions */
#define STATIC_ASSERT(expression, message) \
typedef char static_assertion_##message[(expression)?1:-1]

#define ASSERT(expression) \
if(!(expression)) \
{ \
	printf("\e[1;31m[ASSERT] - [0;31mEXPRESSION: %s, FILE: %s," \
			" LINE: %d\e[0;37m\n", \
			#expression, __FILE__, __LINE__); \
	__builtin_trap(); \
} 

/* typedefs */
STATIC_ASSERT(sizeof(unsigned long long) == 8, 
		unexpected_size_for_unsigned_long_long);
STATIC_ASSERT(sizeof(signed long long) == 8,
		unexpected_size_for_signed_long_long);
STATIC_ASSERT(sizeof(unsigned int) == 4,
		unexpected_size_for_unsigned_int);
STATIC_ASSERT(sizeof(signed int) == 4,
		unexpected_size_for_signed_int);
STATIC_ASSERT(sizeof(unsigned short) == 2,
		unexpected_size_for_unsigned_short);
STATIC_ASSERT(sizeof(signed short) == 2,
		unexpected_size_for_signed_short);
STATIC_ASSERT(sizeof(unsigned char) == 1,
		unexpected_size_for_unsigned_char);
STATIC_ASSERT(sizeof(signed char) == 1,
		unexpected_size_for_signed_char);
STATIC_ASSERT(sizeof(char) == 1,
		unexpected_size_for_char);
STATIC_ASSERT(sizeof(float) == 4,
		unexpected_size_for_float);
STATIC_ASSERT(sizeof(double) == 8,
		unexpected_size_for_double);

typedef unsigned long long u64;
typedef signed long long i64;
typedef unsigned int u32;
typedef signed int i32;
typedef unsigned short u16;
typedef signed short i16;
typedef unsigned char u8;
typedef signed char i8;

typedef unsigned int b32;

typedef float f32;
typedef double f64;

#define true 1
#define false 0 

/* time stuff */
#include <sys/time.h> /* for: gettimeofday, timeval */
#include <unistd.h> /* for: sleep, usleep */

struct timeval linux_get_timeval()
{
	struct timeval tv;

	i32 gettimeofday_result = gettimeofday(&tv, 0);
	ASSERT(gettimeofday_result == 0);

	return tv;
}

f64 linux_get_time_ms()
{
	struct timeval tv = linux_get_timeval();
	f64 ms = (tv.tv_sec * 1000.0) + ((f64)tv.tv_usec / 1000.0);	
	return ms;
}

/* memory */
#include <sys/mman.h>
#include <unistd.h> /* for sysconf */
void *linux_get_memory_pages(u32 pages_count)
{
	u64 page_size = sysconf(_SC_PAGESIZE);
	void *address = mmap(
		0, (pages_count * page_size), 
		PROT_READ | PROT_WRITE,
		MAP_ANONYMOUS | MAP_PRIVATE,
		-1, 0);
	return(address);
}
