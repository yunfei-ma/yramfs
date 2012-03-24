/*
 * ramfs_address_space.c
 *
 *  Created on: Mar 23, 2012
 *      Author: Yunfei
 */


#include <linux/mm.h>
#include <linux/pagemap.h>
#include <linux/swap.h>
#include <linux/buffer_head.h>

int ramfs_readpage(struct file *file, struct page *page)
{

}

int ramfs_write_begin(struct file *file, struct address_space *mapping,
            loff_t pos, unsigned len, unsigned flags,
            struct page **pagep, void **fsdata)
{

}



