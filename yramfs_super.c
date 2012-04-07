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
#include "yramfs_super.h"
#include "yramfs_dir.h"

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
    struct inode        *rootNode = NULL;
    yramfs_sb_info_t    *pInfo    = NULL;

    DBG_PRINT("fill super...");
    sb->s_maxbytes = YRAMFS_CONFIG_MAX_FILESIZE; /* Maximum file size */
    sb->s_blocksize = PAGE_CACHE_SIZE; /* Block size */
    sb->s_blocksize_bits = YRAMFS_CONFIG_BLOCK_SIZE_BITS; /* Number of bits used
                                                to represent the block size */
    sb->s_magic = YRAMFS_MAGIC; /* Filesystem magic number */
    sb->s_op = &yramfs_ops; /* Super block operations */
    pInfo = kmalloc(sizeof(yramfs_sb_info_t), GFP_KERNEL);
    if (NULL == pInfo) {
        return ENOMEM;
    }

    pInfo->nodeSerialNum = 1;
    sb->s_fs_info = pInfo; /* Filesystem private data */

    /* initialize root node */
    rootNode = yramfs_get_inode(sb, NULL, S_IFDIR|0777, 0);
    if(NULL == rootNode) {
        DBG_PRINT("get root node failed");
        return ENOMEM;
    }
    sb->s_root = d_alloc_root(rootNode);
    yramfs_dir_add_path(sb->s_root, "..", DT_DIR);
    yramfs_dir_add_path(sb->s_root, ".", DT_DIR);
    return 0;
}

/*
 * @brief Called to state the fs
 *
 * @param dentry    entry to be state
 * @param buf         kstatfs data
 */
int yramfs_statfs(struct dentry *dentry, struct kstatfs *buf)
{
//    struct super_block *sb = dentry->d_sb;

    DBG_PRINT("stat fs...");
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
 * @brief Called when the filesystem is unmounted
 *              Release the memory for the in-memory super block as well as
 *              free the block heads used for reading the super block
 */
static void yramfs_super_put(struct super_block* sb)
{
}

static const struct super_operations yramfs_ops = {
    .put_super      = yramfs_super_put,
    .write_inode    = yramfs_inode_write,
    .put_super      = yramfs_super_put,
    .statfs         = yramfs_statfs,
};
