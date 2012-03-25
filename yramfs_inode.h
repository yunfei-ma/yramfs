/*
 * yramfs_inode.h
 *
 *  Created on: Mar 23, 2012
 *      Author: yunfei
 */

#ifndef _ramfs_inode_h
#define _ramfs_inode_h

#include "ramfs_common.h"
#include "ramfs_utils.h"

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
int yramfs_inode_write(struct inode* aNode, int do_sync);

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

/*
 * @brief This function will be called when the kernel is resolving a path. The
 * lookup handler of the inode operation table of the parent inode is called to
 * resolve a child. Remember that the dentry for the root most inode is already
 * available in s_root field of the super block.
 *
 * For example, after mounting the file system under '/mnt/yramfs' if we want to
 * see the contents using 'ls /mnt/rkfs', the kernel has to create a file object
 * for the inode '/mnt/yramfs'. The kernel will create a file object with the
 * dentry of the root most inode of the file system. For the command
 * 'ls -l /mnt/rkfs/hello.txt', the kernel name lookup reaches the root most
 * inode and the lookup handler will be called to set the inode of 'hello.txt'.
 * The kernel allocates the dentry object and passes to the handler.
 * If an inode exists for the name component, the inode has to be added to the
 * dentry using d_add and NULL should be returned. If there is some problem, a
 * suitable error code has to be returned.
 *
 * @param parent_inode  the parent inode instance
 * @param dentry        a pear entry to be referenced for looking up
 *
 * @returns dentry
 */
static struct dentry * yramfs_inode_lookup(struct inode *parent_inode,
                                           struct dentry *dentry,
                                           struct nameidata *nameidata);

#endif
