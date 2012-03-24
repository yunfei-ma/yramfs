//
//  Created by Yunfei on 12-3-24.
//  Copyright (c) 2012 Yunfei. All rights reserved.
//
//  The super block stores and manages information about the
//  mounted file system. The super block contains a operation
//  table 's_ops' field and the root dentry s_root'.' The are
//  initialized at the mounting time.

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/pagemap.h>
#include <linux/fs.h>
#include <asm/atomic.h>
#include <asm/uaccess.h>

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
    sb->s_maxbytes = YRAMFS_CONFIG_MAX_FILESIZE; /* Maximum file size */
    sb->s_blocksize = PAGE_CACHE_SIZE; /* Block size */
    sb->s_blocksize_bits = YRAMFS_CONFIG_BLOCK_SIZE_BITS; /* Number of bits used
                                                to represent the block size */
    sb->s_magic = YRAMFS_MAGIC; /* Filesystem magic number */
    sb->s_op = &yramfs_ops; /* Super block operations */
    sb->s_fs_info = NULL; /* Filesystem private data */

    /* initialize root node */
    sb->s_root = d_alloc_root(iget(sb,YRAMFS_ROOT_INODE_NUMBER));
    return 0;
}

struct super_block* yramfs_get_sb(struct file_system_type *fs_type,
		int flags, const char *dev_name,
		void *data, struct vfsmount *mnt)
{
	 return get_sb_nodev(fs_type, flags, dev_name, data,
			 yramfs_fill_super, mnt);
}

int yramfs_statfs(struct dentry *dentry, struct kstatfs *buf)
{
	buf->f_type = dentry->d_sb->s_magic;
	buf->f_bsize = PAGE_CACHE_SIZE;
	buf->f_namelen = NAME_MAX;
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

static void yramfs_super_write(struct super_block *sb)
{
}

static const struct super_operations yramfs_ops = {
	.statfs		= yramfs_statfs,
	.alloc_inode	= yramfs_inode_delete,
	.put_super  = yramfs_super_put
    .alloc_inode = yramfs_inode_alloc,
    .destroy_inode = yramfs_inode_destroy,
    .read_inode = yramfs_inode_read,
    .write_inode = yramfs_inode_write,
    .delete_inode = yramfs_inode_delete,
    .put_super = yramfs_super_put,
    .write_super = yramfs_super_write,
    .statfs = yramfs_statfs

};
