#ifndef __MEM_H__
#define __MEM_H__

#include <stddef.h>

/* Note: check_memleak_debug_enable is a weak function inside SDK.
 * please copy following codes to user_main.c.
#include "mem.h"

bool ICACHE_FLASH_ATTR check_memleak_debug_enable(void)
{
    return MEMLEAK_DEBUG_ENABLE;
}
*/

void *pvPortMalloc (size_t sz, const char *, unsigned);
void vPortFree (void *p, const char *, unsigned);
void *pvPortZalloc (size_t sz, const char *, unsigned);
void *pvPortRealloc (void *p, size_t n, const char *, unsigned);

#ifndef MEMLEAK_DEBUG
#define MEMLEAK_DEBUG_ENABLE    0
#define os_free(s)        vPortFree(s, "", 0)
#define os_malloc(s)      pvPortMalloc(s, "", 0)
#define os_calloc(l, s)   pvPortCalloc(l, s, "", 0);
#define os_realloc(p, s)  pvPortRealloc(p, s, "", 0)
#define os_zalloc(s)      pvPortZalloc(s, "", 0)
#else
#define MEMLEAK_DEBUG_ENABLE	1

#define os_free(s) \
do{\
	static const char mem_debug_file[] ICACHE_RODATA_ATTR STORE_ATTR = __FILE__;	\
    vPortFree(s, mem_debug_file, __LINE__);\
}while(0)

#define os_malloc(s)	\
	({	\
		static const char mem_debug_file[] ICACHE_RODATA_ATTR STORE_ATTR = __FILE__;	\
		pvPortMalloc(s, mem_debug_file, __LINE__);	\
	})

#define os_calloc(l, s)	\
	({	\
		static const char mem_debug_file[] ICACHE_RODATA_ATTR STORE_ATTR = __FILE__;	\
		pvPortCalloc(l, s, mem_debug_file, __LINE__);	\
	})

#define os_realloc(p, s)	\
	({	\
		static const char mem_debug_file[] ICACHE_RODATA_ATTR STORE_ATTR = __FILE__;	\
		pvPortRealloc(p, s, mem_debug_file, __LINE__);	\
	})

#define os_zalloc(s)	\
	({	\
		static const char mem_debug_file[] ICACHE_RODATA_ATTR STORE_ATTR = __FILE__;	\
		pvPortZalloc(s, mem_debug_file, __LINE__);	\
	})

#endif

#endif

