//
//  Created by Yunfei on 12-3-24.
//  Copyright (c) 2012 Yunfei. All rights reserved.
//
//  The super block stores and manages information about the
//  mounted file system. The super block contains a operation
//  table 's_ops' field and the root dentry s_root'.' The are
//  initialized at the mounting time.

#include "yramfs_common.h"
#include "yramfs_inode.h"

static const struct super_operations yramfs_ops;

/*
 * @brief this file initialiazes the super block on mounting time
 *      with 's_op' and 's_root' initialized
 *
 * @param sb    super_block instance to be initialized
 * @param data  
 * @param silent
 */
int yramfs_fill_super(struct super_block *sb, void *data, int silent)
{
    struct inode *rootNode = NULL;
    sb->s_maxbytes = YRAMFS_CONFIG_MAX_FILESIZE; /* Maximum file size */
    sb->s_blocksize = PAGE_CACHE_SIZE; /* Block size */
    sb->s_blocksize_bits = YRAMFS_CONFIG_BLOCK_SIZE_BITS; /* Number of bits used
                                                to represent the block size */
    sb->s_magic = YRAMFS_MAGIC; /* Filesystem magic number */
    sb->s_op = &yramfs_ops; /* Super block operations */
    sb->s_fs_info = NULL; /* Filesystem private data */

    /* initialize root node */
    rootNode = yramfs_get_inode(sb, NULL, S_IFDIR|0755, 0);
    if(NULL == rootNode) {
        return ENOMEM;
    }
    sb->s_root = d_alloc_root(rootNode);
    return 0;
}

int yramfs_statfs(struct dentry *dentry, struct kstatfs *buf)
{
//	struct super_block *sb = dentry->d_sb;

	buf->f_type =  YRAMFS_MAGIC;
	buf->f_bsize = PAGE_CACHE_SIZE;
	buf->f_blocks = 2048;
	buf->f_bfree = 0;
	buf->f_bavail = 0;
	buf->f_files = 5;
	buf->f_ffree = 0;
	buf->f_namelen = YRAMFS_MAX_PATH_LEN;
	return 0;
}

/*
 * Called when the filesystem is unmounted
 * Release the memory for the in-memory super block as well as
 * free the block heads used for reading the super block
 */
static void yramfs_super_put(struct super_block* sb)
{
}

static const struct super_operations yramfs_ops = {
	.put_super  = yramfs_super_put,
    .write_inode = yramfs_inode_write,
    .put_super = yramfs_super_put,
    .statfs = yramfs_statfs

};
