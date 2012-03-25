//
//  yramfs_module.c
//  ramfs
//
//  Created by Yunfei on 12-3-24.
//  Copyright (c) 2012 Yunfei. All rights reserved.
//
//  This file is to register 'yramfs' file system to linux kernel
//  with the data structure of 'struct file_system_type' which is
//  defined in include/linux/fs.h


#include "yramfs_common.h"
#include "yramfs_super.h"

/*
 * @brief this function make a request to mount a device onto a directory
 *        in filespace, the VFS will call the 'get_sb' method to get super block
 *        the dentry for the mount point will then be updated to point to the
 *        root inode for the new file system
 *        mount_bdev    mount a filesystem residing on a block device
 *        mount_nodev   mount a filesystem that is not backed by a device
 *        mount_single  mount a filesystem as a sington instances
 */
struct dentry* yramfs_mount(struct file_system_type *fs, int flags,
                            const char *dev_name, void *data)
{
    return mount_single(fs, flags, data, yramfs_fill_super);
}

/*
 * @brief yramfs file system mounting structure
 */
static struct file_system_type yramfs = {
    .owner   = THIS_MODULE,
    .name    = "yramfs",
    .mount  =  yramfs_mount, /* allocate the super block */
    .kill_sb = kill_litter_super, /* release the super block */
};

/*
 * @brief ramfs registration, this function will register
 * 'yramfs' to kernel, and allocate super block
 */
static int __init yramfs_init(void){
    return register_filesystem(&yramfs);
}

/*
 * @brief amfs unregistration, this function will be called
 * on the file system unmounting, and will release super block
 */
static void __exit yramfs_exit(void){
    unregister_filesystem(&yramfs);
}

module_init(yramfs_init);
module_exit(yramfs_exit);
