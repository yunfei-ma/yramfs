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

/*
 * @brief Create the file with the file name given
 *  in the dentry, under the directory dir with the
 *  permissions in mode
 *  @param dir      the parent directory
 *  @param dentry
 *  @param mode
 *  @param nd
 */
static int yramfs_dir_inode_create(struct inode* dir,
                          struct dentry* dentry,
                          int mode,
                          struct nameidata* nd)
{
    struct qstr *name = &dentry->d_name;
    struct inode* aNode = NULL;

    DBG_PRINT("create %d,%s", name->len, name->name);
    aNode = yramfs_inode_alloc(dir->i_sb);
}


static int yramfs_dir_inode_lookup(struct inode *dir,
                struct dentry *dentry,
                struct nameidata *nd)
{

}

static int rmfs_dir_inode_unlink(struct inode *dir,
            struct dentry *dentry){

}

/*
 * Read the contents of the directory and pass it back to the user
 */
int lab4fs_dir_read(struct file* fp, void* dirent, filldir_t filldir)
{

}

const struct inode_operations rmfs_dir_inode_operations = {
    .create     = yramfs_dir_inode_create,
    .lookup     = yramfs_dir_inode_lookup,
    .unlink     = rmfs_dir_inode_unlink,
};

const struct file_operations simple_dir_operations = {
    .read       = generic_read_dir,
    .readdir    = yramfs_dir_readdir,
    .fsync      = simple_sync_file,
};
