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
#include "yramfs_file.h"

/*
 * @brief this function add a path to a dir's vector module, the vector module
 *          which is contained by the dir->i_private stores information of all
 *          sub entries
 *
 * @param dir         pointer to an instance of dir inode
 * @param entry     new entry to be added
 * @param type      type of this path, currently only file, folder is supported
 *
 * @returns error codes
 */
int yramfs_dir_add_path(struct inode *dir, struct dentry  *entry,
                                unsigned char type)
{
    yramfs_dir_info_t       *pInfo = NULL;
    yramfs_vector_t         *pVector = NULL;
    yramfs_sb_info_t        *pSuperInfo  = NULL;
    int                      err = 0;

    if ((NULL == dir) || (NULL == entry)) {
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
    DBG_PRINT("vector is %p, add path:%s", pVector, entry->d_name.name);
    if (NULL == pVector) {
        return ENOMEM;
    }
    pSuperInfo = (yramfs_sb_info_t*)dir->i_sb->s_fs_info;
    pInfo = kzalloc(sizeof(yramfs_dir_info_t), GFP_KERNEL);
    pInfo->entry = entry;
    pInfo->ftype = type;

    return yramfs_vector_add(pVector, (uint32_t)pInfo, (free_func_t)kfree);
}

/*
 * @brief this function remove a path from a dir's vector module
 *
 * @param dir         pointer to an instance of dir inode
 * @param entry     new entry to be added
 *
 * @returns error codes
 */
int yramfs_dir_remove_path(struct inode *dir, struct dentry  *entry)
{
    yramfs_vector_t     *pVector = NULL;
    uint32_t             i = 0, count = 0;
    int                  err = 0;
    yramfs_dir_info_t   *pinfo = NULL;

    pVector = dir->i_private;
    if (NULL == pVector) {
        return EINVAL;
    }

    err = yramfs_vector_count(pVector, &count);
    if (err) {
        DBG_PRINT("count vector failed:%d", err);
        return err;
    }

    for (i = 0; i < count; i ++){
        err = yramfs_vector_get_at(pVector, i, (uint32_t *) &pinfo);
        if (err) {
            DBG_PRINT("count vector failed:%d", err);
            return err;
        }
        if (0 == strcmp(entry->d_name.name, pinfo->entry->d_name.name)){
            DBG_PRINT("entry founded %p, info entry:%p", entry, pinfo->entry);
            yramfs_vector_remove_data(pVector, (uint32_t)pinfo);
            break;
        }
    }

    if(i >= count) {
        return EEXIST;
    }

    return 0;
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

    DBG_PRINT("dir mknod");
    if (inode) {
        if (dir->i_mode & S_ISGID) {
            inode->i_gid = dir->i_gid;
            if (S_ISDIR(mode)){
                inode->i_mode |= S_ISGID;
            }
        }
        d_instantiate(dentry, inode);
        dget(dentry);   /* Extra count - pin the dentry in core */
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
    int err = 0;

    DBG_PRINT("mking dir:%s", dentry->d_name.name);
    err = yramfs_dir_mknod(dir, dentry, mode | S_IFDIR, 0);
    if (err) {
        DBG_PRINT("mk nod failed:%d", err);
        return err;
    }
    err = yramfs_dir_add_path(dir, dentry, DT_DIR);
    if (err) {
        DBG_PRINT("add path failed: %d", err);
    }

    return err;
}

static int yramfs_dir_rmdir(struct inode *dir, struct dentry *dentry)
{
    int err = 0;

    DBG_PRINT("rmdir %s", dentry->d_name.name);
    err = yramfs_dir_remove_path(dir, dentry);
    if (err) {
        DBG_PRINT("remove path failed:%d", err);
    }

    return simple_rmdir(dir, dentry);
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
                            int mode, struct nameidata * namei)
{
    int err = 0;

    DBG_PRINT("dir create file %s", dentry->d_name.name);
    err = yramfs_dir_mknod(dir, dentry, 0777 | S_IFREG, 0);
    if (err) {
        DBG_PRINT("mk node failed: %d", err);
        return err;
    }

    err = yramfs_dir_add_path(dir, dentry, DT_REG);
    if (err) {
        DBG_PRINT("add path failed: %d", err);
    }

    return err;
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
 * @param currInode    current inode instance
 * @param dentry        a pear entry to be referenced for looking up
 * @param namei
 *
 * @returns dentry
 */
static struct dentry * yramfs_dir_inode_lookup(struct inode *currInode,
                            struct dentry *entry, struct nameidata *namei)
{
    uint32_t            i = 0, count = 0;
    yramfs_vector_t    *pVector = NULL;
    struct inode       *inode = NULL;
    yramfs_dir_info_t  *pinfo = NULL;
    int                 mode  = YRAMFS_DEFAULT_MODE;

    DBG_PRINT(" looking up %s... ", entry->d_name.name);

    pVector = currInode->i_private;
    if (NULL == pVector) {
        goto exit;
    }

    yramfs_vector_count(pVector, &count);
    for (i = 0; i < count; i ++){
        yramfs_vector_get_at(pVector, i, (uint32_t *) &pinfo);
        if (0 == strcmp(entry->d_name.name, pinfo->entry->d_name.name)){
            DBG_PRINT("entry founded %p, info entry:%p", entry, pinfo->entry);
            break;
        }
    }

    // failed on finding the path
    if (i >= count) {
        goto exit;
    }

    if(DT_DIR == pinfo->ftype) {
        mode |= S_IFDIR;
    } else if (DT_REG == pinfo->ftype) {
        mode |= DT_REG;
    }
    inode = yramfs_get_inode(currInode->i_sb, currInode,
                             mode, 0);

exit:
    d_add(entry, inode);
    return NULL;
}

ssize_t yramfs_dir_read(struct file *filp, char __user *buf, size_t siz,
loff_t *ppos)
{
    DBG_PRINT(" yramfs dir read %s... ", filp->f_dentry->d_name.name);
    return generic_read_dir(filp, buf, siz, ppos);
}

/*
 * Read the contents of the directory and pass it back to the user
 */
int yramfs_dir_readdir(struct file* fp, void* dirent, filldir_t filldir)
{
    struct dentry     *currEntry = fp->f_dentry;
    struct inode      *currNode = currEntry->d_inode;
    uint32_t           currentFileIndex = fp->f_pos;
    uint32_t           fileCount;
    yramfs_vector_t   *pVector = NULL;
    int                err = 0;
    yramfs_dir_info_t *pInfo = NULL;

    DBG_PRINT(" reading dir file=%s,%d ", currEntry->d_name.name,
                                          currentFileIndex);

    // current directory, displayed by 'ls -a'
    if (fp->f_pos == 0) {
        err = filldir(dirent, ".", 1,fp->f_pos,
                  currEntry->d_inode->i_ino, DT_DIR);
        if(err) {
            return 0;
        }
        fp->f_pos ++;
    }

    // parent directory, displayed by 'ls -a'
    if(fp->f_pos == 1) {
        err = filldir(dirent, "..", 2,fp->f_pos,
                  parent_ino(currEntry), DT_DIR);
        if(err) {
            return 0;
        }
        fp->f_pos ++;
    }

    pVector = currNode->i_private;
    if (NULL == pVector) {
        return 0;
    }

    err = yramfs_vector_count(pVector, &fileCount);
    if (err) {
        DBG_PRINT("failed to count vector%d", err);
        return err;
    }

    DBG_PRINT("start browsing dir %d(%d)", currentFileIndex, fileCount);
    currentFileIndex = fp->f_pos - 2;
    fp->f_pos ++;
    err = yramfs_vector_get_at(pVector, currentFileIndex, (void*)&pInfo);
    if (err) {
        DBG_PRINT("failed to get dir info:%d", err);
        return 0;
    }
    DBG_PRINT("fill dir %s,%d", pInfo->entry->d_name.name,
                                currentFileIndex);
    err = filldir(dirent,pInfo->entry->d_name.name,
                    strlen(pInfo->entry->d_name.name),
                    currentFileIndex,
                    pInfo->entry->d_inode->i_ino,
                    pInfo->ftype);
    if(err) {
        return err;
    }

    return 0;
}

int yramfs_dir_link(struct dentry *old_dentry, struct inode *dir,
                        struct dentry * dentry)
{
    DBG_PRINT("linking for %s", dentry->d_name.name);
    struct inode *inode = old_dentry->d_inode;

    inode->i_ctime = dir->i_ctime = dir->i_mtime = CURRENT_TIME;
    inc_nlink(inode);
    ihold(inode);
    dget(dentry);
    d_instantiate(dentry, inode);
    return 0;
}

int yramfs_dir_unlink(struct inode *dir, struct dentry *dentry)
{
    struct inode *inode = dentry->d_inode;
    int err;

    DBG_PRINT("unlink for %s", dentry->d_name.name);
    inode->i_ctime = dir->i_ctime = dir->i_mtime = CURRENT_TIME;
    err = yramfs_dir_remove_path(dir, dentry);
    if (err) {
        DBG_PRINT("remove path failed:%d", err);
    }
    drop_nlink(inode);
    dput(dentry);

    return 0;
}

const struct inode_operations yramfs_dir_inode_operations = {
    .create     = yramfs_dir_create,
    .lookup     = yramfs_dir_inode_lookup,
    .mkdir      = yramfs_dir_mkdir,
    .rmdir      = yramfs_dir_rmdir,
    .mknod      = yramfs_dir_mknod,
    .link       = yramfs_dir_link,
    .unlink     = yramfs_dir_unlink,
};


const struct file_operations yramfs_dir_operations = {
    .read       = yramfs_dir_read,
    .readdir    = yramfs_dir_readdir,
};
