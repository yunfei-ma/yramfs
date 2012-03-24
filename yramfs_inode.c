#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/pagemap.h>
#include <linux/fs.h>
#include <linux/buffer_head.h>
#include <linux/smp_lock.h>
#include <asm/atomic.h>
#include <asm/uaccess.h>
#include "yramfs_inode.h"
#include "yramfs_utils.h"

MODULE_LICENSE("Yunfei Task @ BOSCH");

/*
 * @brief This function is called when the given inode is
 *  to be read
 *  it convert the given inode number into block number and
 *  read the inode's info from the block
 */
void yramfs_inode_read(struct inode* inode)
{
    DBG_PRINT("%s", __FUNCTION__);
}

/*
 * Called when the given inode info is to be written to the disk
 */
int yramfs_inode_write(struct inode* inode, int do_sync)
{
    DBG_PRINT("%s", __FUNCTION__);
}
/*
 * Called when the given inode is to be deleted
 * Mark this inode as free in the inode bitmap
 * Mark the data blocks as free in the data bitmap
 */
void yramfs_inode_delete(struct inode* inode)
{
    DBG_PRINT("%s", __FUNCTION__);
}

/*
 * Truncate the inode - freeing up all the data blocks associated
 * with this inode, thereby making it a 0-byte file/directory
 */
void yramfs_inode_truncate(struct inode* inode)
{
    DBG_PRINT("%s", __FUNCTION__);
}

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
 * @param dentry        entry to be looked up
 * @param nameidata     name
 */
static struct dentry * yramfs_inode_lookup(struct inode *parent_inode,
                        struct dentry *dentry, struct nameidata *nameidata)
{

}

struct inode_operations dir_iops = {
  .create = yramfs_inode_create,
  .lookup = yramfs_iop_lookup,
  .link = yramfs_iop_link,
  .unlink = lab4fs_iop_unlink,
  .mkdir = lab4fs_iop_mkdir, /* Can be removed in the end */
  .rmdir = lab4fs_iop_rmdir, /* Also can be removed in the end */
  .rename = lab4fs_iop_rename
};
