//
//  yramfs_memory.c
//  ramfs
//
//  Created by Yunfei Ma on 12-3-24.
//  Copyright (c) 2012 yunfei. All rights reserved.
//

#include <stdio.h>
#include "yramfs_utils.h"


struct inode* yramfs_inode_alloc(struct inode *parentNode,
                                struct dentry *dentry,
                                int mode)
{
	yramfs_inode_info_t  *pNodeInfo = NULL;
    struct super_block  *sb = parentNode->i_sb;
    struct inode        *pNode = NULL;
    
	DBG_PRINT("%s", __FUNCTION__);
	pNodeInfo = kmem_cache_alloc(pRamfsInodeCache, SLAB_KERNEL);
    if (NULL == pNodeInfo) {
    	DBG_PRINT("alloc memory failed");
    	return NULL;
    }
    
    pNode = pNodeInfo->vfs_inode;
    pNode->i_mode = mode;
    pNode->i_uid = current_fsuid();
    pNode->i_gid = current_fsgid();
    pNode->i_mapping->backing_dev_info = &dlmfs_backing_dev_info;
    pNode->i_atime = pNode->i_mtime = pNode->i_ctime = CURRENT_TIME;
    
    return &pNodeInfo->vfs_inode;
}

void yramfs_inode_destroy(struct inode* inode)
{
    DBG_PRINT("%s", __FUNCTION__);
    yramfs_inode_info_t *pNode = (yramfs_inode_info_t*)inode;
	kmem_cache_free(pRamfsInodeCache, pNode);
}