#ifndef _ramfs_inode_h
#define _ramfs_inode_h

#include "ramfs_common.h"
#include "ramfs_utils.h"

/* In-memory inode structure */
typedef struct ramfs_inode_info {
    struct inode vfs_inode;   /* inode structure */
}ramfs_inode_info_t;

/*
 * @brief this function allocates a ramfs_inode_into_t which has an inode
 *        embeded
 *
 */
struct inode* ramfs_inode_alloc(struct super_block* sb);
void ramfs_inode_destroy(struct inode* inode);
/*
 * @brief This function is called when the given inode is
 *  to be read from the 'disk' into memory(i.e. inode cache)
 *  it convert the given inode number into block number and
 *  read the inode's info from the block
 */
void ramfs_inode_read(struct inode* inode);
#endif
