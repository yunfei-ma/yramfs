/*
 * yramfs_inode.h
 *
 *  Created on: Mar 23, 2012
 *      Author: yunfei
 */

#ifndef _ramfs_inode_h
#define _ramfs_inode_h

#include "yramfs_common.h"
#include "yramfs_utils.h"

/* In-memory inode structure */
typedef struct ramfs_inode_info {
    struct inode vfs_inode;   /* inode structure */
}ramfs_inode_info_t;

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
                const struct inode *dir, int mode, dev_t dev);

/*
 * @brief This function is called when the given inode is
 *  to be read
 *  it converts the given inode number into block number and
 *  read the inode's info from the 'memory block'
 *
 * @param aNode an instance of struct inode
 * @returns none
 */
void yramfs_inode_read(struct inode* aNode);

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
int yramfs_inode_write(struct inode* aNode, struct writeback_control *wbc);

/*
 * @brief this function is called when the given inode is to be deleted
 *          we may mark a block or disk free here if a device is supported
 *
 * @param inode an instance of struct inode
 *
 * @returns none
 */
void yramfs_inode_delete(struct inode* aNode);

/*
 * @brief Truncate the inode - freeing up all the data blocks associated
 *          with this inode, thereby making it a 0-byte file/directory
 *
 * @param aNode an instance of struct inode
 *
 * returns none
 */
void yramfs_inode_truncate(struct inode* aNode);

#endif
