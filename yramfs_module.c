//
//  yramfs_module.c
//  ramfs
//
//  Created by Yunfei on 12-3-24.
//  Copyright (c) 2012年 Yunfei. All rights reserved.
//
//  This file is to register 'yramfs' file system to linux kernel
//  with the data structure 'yramfs' of 'struct file_system_type'
//  defined in include/linux/fs.h


#include <linux/fs.h>
#include <linux/pagemap.h>
#include <linux/highmem.h>
#include <linux/time.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/backing-dev.h>
#include <linux/ramfs.h>
#include <linux/sched.h>
#include <linux/parser.h>
#include <linux/magic.h>
#include <linux/slab.h>

#include <asm/uaccess.h>

/*
 * @brief yramfs file system mounting structure
 */
const static struct file_system_type yramfs = {
    .owner   = THIS_MODULE,
    .name    = "yramfs",
    .get_sb  = yramfs_get_sb, /* allocate the super block */
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
    return unregister_filesystem(&yramfs);
}

module_init(yramfs_init);
module_exit(yramfs_exit);
