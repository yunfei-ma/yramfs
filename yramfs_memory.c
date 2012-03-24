//
//  yramfs_memory.c
//  ramfs
//
//  Created by Yunfei Ma on 12-3-24.
//  Copyright (c) 2012 yunfei. All rights reserved.
//

#include <stdio.h>
#include "yramfs_utils.h"

static struct kmem_cache *pRamfsPool = NULL;

typedef struct {
    struct kmem_cache  *pRamfsPool;
    uint32_t            size;
}ramfs_memory;

int yramfs_memory_initialize(void *data)
{
	pRamfsPool = kmem_cache_create("ramfsPool",
                                   sizeof(yramfs_inode_info_t),
                                   0, SLAB_PANIC, init_once);
	return 0;
}

void yramfs_memory_deinitialize(void)
{
	kmem_cache_destroy(pRamfsPool);
}

void *yramfs_alloc(uint32_t size, char *fileName, int lineNumber)
{
    void *ptr = NULL;
    if (NULL == pRamfsPool) {
        DBG_PRINT("pool is not initialized");
        return NULL;
    }
    
    ptr = kmem_cache_alloc(pRamfsPool, SLAB_KERNEL);
    DBG_PRINT("mem -- alloc %x, in-%s-at-%d", ptr, size, fileName, lineNumber);
    return ptr;
}

void yramfs_free(void *ptr)
{
    DBG_PRINT("mem -- free %x", ptr);
    kmem_cache_free(pRamfsPool, ptr);
}
