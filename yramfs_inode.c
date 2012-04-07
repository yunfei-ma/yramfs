//
//  Created by Yunfei on 12-3-24.
//  Copyright (c) 2012 Yunfei. All rights reserved.
//
//  this file defines inode operations for super block, it
//  provides basic functions to manage an inode life time

#include "yramfs_common.h"
#include "yramfs_inode.h"
#include "yramfs_utils.h"
#include "yramfs_super.h"
#include "yramfs_file.h"

extern const struct inode_operations yramfs_dir_inode_operations;
extern const struct file_operations yramfs_dir_operations;

static struct backing_dev_info yramfs_backing_dev_info = {
    .name       = "yramfs",
    .ra_pages   = 0,    /* No readahead */
    .capabilities   = BDI_CAP_NO_ACCT_AND_WRITEBACK |
                BDI_CAP_MAP_DIRECT | BDI_CAP_MAP_COPY |
                BDI_CAP_READ_MAP | BDI_CAP_WRITE_MAP | BDI_CAP_EXEC_MAP,
};


/*
 * @brief This function is called when the given inode is
 *  to be read
 *  it converts the given inode number into block number and
 *  read the inode's info from the 'memory block'
 *
 * @param aNode an instance of struct inode
 * @returns none
 */
struct inode *yramfs_get_inode(struct super_block *sb,
                const struct inode *dir, int mode, dev_t dev)
{
    struct inode     *inode = new_inode(sb);
    yramfs_sb_info_t *psbinfo = (yramfs_sb_info_t*)sb->s_fs_info;

    if (inode) {
        inode->i_ino = psbinfo->nodeSerialNum ++;
        inode_init_owner(inode, dir, mode);
        inode->i_mapping->a_ops = &yramfs_file_addr_operations;
        inode->i_mapping->backing_dev_info = &yramfs_backing_dev_info;
        mapping_set_gfp_mask(inode->i_mapping, GFP_HIGHUSER);
/*
    Unevictable pages are simply pages that can't be paged out for a variety of
    reasons. It might mean that the page belongs to a ramdisk, was protected by a
    call to mlock(), shared and locked, or any other circumstances where the
    kernel was told 'don't touch'.
*/
        mapping_set_unevictable(inode->i_mapping);
        inode->i_atime = inode->i_mtime = inode->i_ctime = CURRENT_TIME;
        switch (mode & S_IFMT) {
        default:
            init_special_inode(inode, mode, dev);
            break;
        case S_IFREG:
            inode->i_mode |= S_IFREG;
            inode->i_op = &yramfs_file_inode_operations;
            inode->i_fop = &yramfs_file_operations;
            break;
        case S_IFDIR:
            inode->i_mode |= S_IFDIR;
            inode->i_op = &yramfs_dir_inode_operations;
            inode->i_fop = &yramfs_dir_operations;

            /* directory inodes start off with i_nlink == 2 (for "." entry) */
            inc_nlink(inode);
            break;
        case S_IFLNK:
//            inode->i_op = &page_symlink_inode_operations;
            break;
        }
    }
    return inode;
}

/*
 * @brief This function is called when the given inode is
 *  to be read
 *  it converts the given inode number into block number and
 *  read the inode's info from the 'memory block'
 *
 * @param aNode an instance of struct inode
 * @returns none
 */
void yramfs_inode_read(struct inode* aNode)
{
    DBG_PRINT(" reading ...");
}

/*
 * @brief This function is called when the given inode info
 *        is to be written to the disk
 *
 * @param aNode an instance of struct inode
 * @param do_sync synchronization
 *
 * @returns error codes
 *
 */
int yramfs_inode_write(struct inode* aNode, struct writeback_control *wbc)
{
    DBG_PRINT(" writing sync%p...", wbc);
    return 0;
}

/*
 * @brief this function is called when the given inode is to be deleted
 *          we may mark a block or disk free here if a device is supported
 *
 * @param inode an instance of struct inode
 *
 * @returns none
 */
void yramfs_inode_delete(struct inode* aNode)
{
    DBG_PRINT(" deleting ...");
}

/*
 * @brief Truncate the inode - freeing up all the data blocks associated
 *          with this inode, thereby making it a 0-byte file/directory
 *
 * @param aNode an instance of struct inode
 *
 * returns none
 */
void yramfs_inode_truncate(struct inode* aNode)
{
    DBG_PRINT(" truncating ...");
}



