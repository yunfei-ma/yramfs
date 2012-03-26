//
//  Created by Yunfei on 12-3-24.
//  Copyright (c) 2012 Yunfei. All rights reserved.
//
//  this file defines dir operations
#include "yramfs_common.h"
#include "yramfs_inode.h"
#include "yramfs_utils.h"
#include "yramfs_super.h"
#include "yramfs_dir.h"

/*
 * @brief this function add a path to a dir's vector module
 *
 * @param pVector  pointer to an instance of yramfs_vector_t 
 * @param path     new path to be added
 * @param type     type of this path, currently only file, folder is supported
 *
 * @returns error codes
 */
int yramfs_dir_add_path(struct inode *dir, char *path, unsigned char type)
{
    yramfs_dir_info_t       *pInfo = NULL;
    yramfs_vector_t         *pVector = NULL;
    yramfs_sb_info_t        *pSuperInfo  = NULL;
    int                      err = 0;

    if ((NULL == dir) || (NULL == path)) {
        return EINVAL;
    }

    pVector = (yramfs_vector_t*) dir->i_private;
    if (NULL == pVector) {
        err = yramfs_vector_create(&pVector);
        dir->i_private = pVector;
    }
    if (err) {
        DBG_PRINT("failed at creating vector%d", err);
        return err;
    }
    pSuperInfo = (yramfs_sb_info_t*)dir->i_sb->s_fs_info;
    pInfo = kzalloc(sizeof(yramfs_dir_info_t), GFP_KERNEL);
    strncpy(pInfo->name, path, YRAMFS_MAX_PATH_LEN);
    pInfo->ftype = type;
    pInfo->ino  = pSuperInfo->nodeSerialNum ++;

    return yramfs_vector_add(pVector, (uint32_t)pInfo, (free_func_t)kfree);
}

/*
 * @brief This function creates a new file, allocate an inode and instantiate
 *        it with dentry
 *
 * @param dir           parent directory node
 * @param dentry        new file dentry
 * @param mode          dentry mode
 * @param dev           device
 *
 * @returns error codes
 */
static int yramfs_dir_mknod(struct inode *dir, struct dentry *dentry,
                            int mode, dev_t dev)
{
    struct inode * inode = yramfs_get_inode(dir->i_sb, dir, mode, dev);
    int error = ENOSPC;

    if (inode) {
        if (dir->i_mode & S_ISGID) {
            inode->i_gid = dir->i_gid;
            if (S_ISDIR(mode)){
                error = yramfs_dir_add_path(dir, ".", DT_DIR);
                if (error) {
                    DBG_PRINT("failed on adding path .");
                    return error;
                }

                error = yramfs_dir_add_path(dir, "..", DT_DIR);
                if (error) {
                    DBG_PRINT("failed on adding path '..'");
                    return error;
                }

                inode->i_mode |= S_ISGID;
            }
        }
        d_instantiate(dentry, inode);
        dget(dentry);	/* Extra count - pin the dentry in core */
        error = 0;
    }
    return error;
}

/*
 * @brief this function is called by the mkdir(2) system call.
 *        Only required if you want to support creating subdirectories.
 *        You will probably need to call d_instantiate() just as you would
 *        in the create() method
 *
 * @param dir       parent dir node
 * @param dentry    directory entry
 * @param mode      mode for dentry
 *
 * @returns error codes
 */
static int yramfs_dir_mkdir(struct inode * dir, struct dentry * dentry,
                             int mode)
{
    int retval = yramfs_dir_mknod(dir, dentry, mode | S_IFDIR, 0);
    if (!retval)
        dir->i_nlink++;
    return retval;
}

/*
 * @brief Create the file with the file name given in the dentry,
 *          under the directory dir with the permissions in mode.
 *          It is called by the open(2) and creat(2) system calls. Only
 *          required if you want to support regular files. The dentry you
 *          get should not have an inode (i.e. it should be a negative
 *          dentry). Here you will probably call d_instantiate() with the
 *          dentry and the newly created inode
 *  @param dir      the parent directory
 *  @param dentry   the entry info
 *  @param symname  the file operation mode
 *
 * @returns error codes
 */
static int yramfs_dir_create(struct inode *dir, struct dentry *dentry,
                            int mode, const char symname)
{
    return yramfs_dir_mknod(dir, dentry, mode | S_IFREG, 0);
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
 * @param dentry        a pear entry to be referenced for looking up
 *
 * @returns dentry
 */
static struct dentry * yramfs_dir_inode_lookup(struct inode *parent_inode,
                            struct dentry *entry, struct nameidata *name)
{
    DBG_PRINT(" looking up %s... ", entry->d_name.name);
    /*
    if(parent_inode->i_ino != rkfs_root_inode->i_ino)
        return ERR_PTR(-ENOENT);
    if(dentry->d_name.len != strlen("hello.txt") ||
       strncmp(dentry->d_name.name, "hello.txt", dentry->d_name.len))
        return ERR_PTR(-ENOENT);

    file_inode = iget(parent_inode->i_sb, FILE_INODE_NUMBER);
    if(!file_inode)
        return ERR_PTR(-EACCES);
    file_inode->i_size = file_size;
    file_inode->i_mode = S_IFREG|S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH;
    file_inode->i_fop = &rkfs_fops;
    //  file_inode->i_fop
    d_add(dentry, file_inode);
    return NULL;*/
    return simple_lookup(parent_inode, entry, name);
}/*
my_lookup(struct inode *parent_inode, struct dentry *dentry, struct nameidata *nameidata) {

    printk("my_lookup:\t");
    printk(dentry->d_name.name);
    printk("\n");

    // i is a global variable.
    struct inode *file_inode iget(parent_inode->i_sb, FILE_INODE_NUMBER + i);
    i++;

    if(!file_inode) {
        printk("Can't allocate the inode\n");
        return ERR_PTR(-EACCES);
    }

    file_inode->i_size = file_size; // some fixed value
    file_inode->i_mode = S_IFREG|S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH|DT_DIR;

    file_inode->i_op = &my_iops;
    file_inode->i_sb = parent_inode->i_sb;
    d_add(dentry, file_inode);

out:
    return NULL;
}*/

/*
 * Read the contents of the directory and pass it back to the user
 */
int yramfs_dir_readdir(struct file* fp, void* dirent, filldir_t filldir)
{
    struct dentry  *parentEntry = fp->f_dentry;
    struct inode   *parentNode = parentEntry->d_inode;
    uint32_t   currentFileIndex = fp->f_pos;
    uint32_t   fileCount, addedCount = 0;
    ino_t           dino;
    int stored = 0;
    unsigned char ftype=DT_REG;
    char filename[64];
    yramfs_vector_t  *pVector = NULL;
    int             err = 0;
    yramfs_dir_info_t *pInfo = NULL;

    DBG_PRINT(" reading dir file=%s,%d ", parentEntry->d_name.name,
                                          currentFileIndex);

    pVector = parentNode->i_private;
    err = yramfs_vector_count(pVector, &fileCount);
    if (err || (0 == fileCount)) {
        DBG_PRINT("failed to count vector%d", err);
        return ENOSPC;
    }
    
    for (currentFileIndex = fp->f_pos; currentFileIndex < fileCount;
         currentFileIndex++) {
        err = yramfs_vector_get_at(pVector, currentFileIndex, (void*)&pInfo);
        if (err) {
            DBG_PRINT("failed to get dir info:%d", err);
            return err;
        }
        DBG_PRINT("fill dir %s", pInfo->name);
        err = filldir(dirent,pInfo->name,strlen(pInfo->name),currentFileIndex,
                      pInfo->ino,pInfo->ftype);
        if(err) {
            return addedCount;
        }
        addedCount ++;
    }

    return 0;
}

const struct inode_operations yramfs_dir_inode_operations = {
    .create     = yramfs_dir_create,
    .lookup     = yramfs_dir_inode_lookup,
    .mkdir      = yramfs_dir_mkdir,
    .rmdir      = simple_rmdir,
    .mknod      = yramfs_dir_mknod
};


const struct file_operations yramfs_dir_operations = {
    .read       = generic_read_dir,
    .readdir    = yramfs_dir_readdir,
};
