#define _yramfs_file_c
#include "yramfs_common.h"
#include "yramfs_file.h"

void yramfs_file_inode_truncate(struct inode* inode)
{
    yramfs_file_info_t *pInfo = NULL;

    DBG_PRINT("truncating file node %ld.", inode->i_ino);
    if(NULL == inode) {
        return;
    }

    pInfo = inode->i_private;
    if (NULL == pInfo) {
        return;
    }

    kfree(pInfo->pContent);
    pInfo->contentSize = 0;
}

int yramfs_file_inode_get_attr(struct vfsmount *mnt, struct dentry *dentry,
                 struct kstat *stat)
{
    simple_getattr(mnt, dentry, stat);
    DBG_PRINT("%s", dentry->d_name.name);

    return 0;
}

int yramfs_file_inode_set_attr(struct dentry *dentry, struct iattr *iattr)
{
    DBG_PRINT("");
    simple_setattr(dentry, iattr);
    return 0;
}

/**
 * @brief generic llseek implementation for regular files This is a generic implemenation
 *          of ->llseek useable for all normal local filesystems.  It just updates the file
 *          offset to the value specified by
 * @offset and @origin under i_mutex.
 * @param   pfile       file structure to seek on
 * @param   offset    file offset to seek to
 * @param   origin    type of seek
 *
 * @returns file postion after seek
 *
 */
loff_t yramfs_file_llseek(struct file *pfile, loff_t offset, int origin)
{
    DBG_PRINT("seek");
    return generic_file_llseek(pfile, offset, origin);
}

/*
 * @brief this function is called by read(2) and related system calls
 *
 * @param    pfile    file structure to read on
 * @param    buf     buffer address to be filled
 * @param    len     length of buf
 * @param    ppos   current file position
 *
 * @returns   read size
 */
ssize_t yramfs_file_read(struct file *pfile, char __user *buf,
                        size_t len, loff_t *ppos)
{
    DBG_PRINT("read");
    return do_sync_read(pfile, buf, len, ppos);
}

/*
 * @brief write data to a file
 *
 * @param    pfile  file structure to write on
 * @param    buf    buf holds data to be writen
 * @param    len    length of data in buf
 * @param    ppos position in file where to write
 *
 */
ssize_t yramfs_file_write(struct file *pfile, const char __user *buf,
                        size_t len, loff_t *ppos)
{
    DBG_PRINT("write");
    if(*ppos > 0) {
        printk("Positive offset %ld\n", *ppos);
        return 0;
    }
    /*
    count = maxlen > sizeof(file_buf) ? sizeof(file_buf) : maxlen;
    __generic_copy_from_user(file_buf, buf, count);
    copy_from_user(file_buf, buf, maxlen);
    printk( "file_operations.write
        called with maxlen=%d, off=%d\n", maxlen, *offset );
    *ppos += count;
    if(*ppos > file_size)
        file_size = *offset;*/
return len;//do_sync_write(pfile, buf, len, ppos);
}

/*
  * @breif called by the mmap(2) system call ? dont understance...
  *
  * @param pfile    file structure to map on
  * @param vma   virtual memory address?
  *
  * returns error codes
  */
int yramfs_file_mmap(struct file * pfile, struct vm_area_struct * vma)
{
    DBG_PRINT("mmap");
    return generic_file_mmap(pfile, vma);
}

/*
 * @brief Called when an inode is about to be open.
 *
 * @param  inode    inode data structure to be opened
 * @param pfile       file data structure
 */
int yramfs_file_open(struct inode * inode, struct file * pfile)
{
    return generic_file_open(inode, pfile);
}

int yramfs_file_readpage(struct file* pfile, struct page* page)
{
    struct inode *pNode = page->mapping->host;
    yramfs_file_info_t *pInfo = NULL;

    DBG_PRINT("page index:%ld, %s", page->index, pfile->f_dentry->d_name.name);
    if(page->index > 0) {
        return -ENOSPC;
    }
    pInfo = (yramfs_file_info_t *)pNode->i_private;
    DBG_PRINT("\t----private is %p", pInfo);


    SetPageUptodate(page);
    memcpy(page_address(page), "hello", 5);
    if(PageLocked(page))
        unlock_page(page);
    return 0;
}


/*
  * @brief This will be called when the dirty pages are flushed.
  */
int yramfs_file_writepage(struct page* page, struct writeback_control* wbc)
{
    struct inode *pNode = page->mapping->host;
    yramfs_file_info_t *pInfo = NULL;

    DBG_PRINT("write page %p, %ld", page_address(page), pNode->i_ino);
    pInfo = (yramfs_file_info_t *)pNode->i_private;
    if (NULL == pInfo) {
        return EINVAL;
    }

    if (pInfo->pContent == NULL) {
        pInfo->pContent = kmalloc(sizeof(PAGE_SIZE),
                                           GFP_KERNEL);
        pInfo->contentSize = PAGE_SIZE;
    }

  memcpy(pInfo->pContent, page_address(page), PAGE_SIZE);
  ClearPageDirty(page);
  if(PageLocked(page))
    unlock_page(page);
  return 0;
}

int yramfs_file_writebegin(struct file *pfile, struct address_space *mapping,
            loff_t pos, unsigned len, unsigned flags,
            struct page **pagep, void **fsdata)
{
    DBG_PRINT("write begin");
    return simple_write_begin(pfile, mapping, pos, len, flags, pagep, fsdata);
}


int yramfs_file_writeend(struct file *file, struct address_space *mapping,
    loff_t pos, unsigned len, unsigned copied,
    struct page *page, void *fsdata)
{
    DBG_PRINT("write end");
    return simple_write_end(file, mapping, pos, len, copied, page, fsdata);
}

/*
 * For address_spaces which do not use buffers nor write back.
 */
int yramfs_file_set_page_dirty_no_writeback(struct page *page)
{
    if (!PageDirty(page))
        return !TestSetPageDirty(page);
    return 0;
}

#if 0
const struct inode_operations yramfs_file_inode_operations = {
  .truncate = yramfs_file_inode_truncate,
  .setattr  = yramfs_file_inode_set_attr,
  .getattr = yramfs_file_inode_get_attr
};

const struct file_operations yramfs_file_operations = {
  .llseek = yramfs_file_llseek,
  .read = yramfs_file_read,
  .write = yramfs_file_write,
  .mmap = yramfs_file_mmap,
  .open = yramfs_file_open
};

const struct address_space_operations yramfs_file_addr_operations = {
  .readpage = yramfs_file_readpage,
  .writepage = yramfs_file_writepage,
//  .sync_page = block_sync_page,
//  .prepare_write = yramfs_file_prepare_writepage,
//  .commit_write = generic_commit_write,
  .write_begin = yramfs_file_writebegin,
  .write_end    = yramfs_file_writeend,
//  .set_page_dirty = __set_page_dirty_no_writeback,
};
#endif

const struct address_space_operations yramfs_file_addr_operations = {
    .readpage       = simple_readpage,
    .write_begin    = yramfs_file_writebegin,
    .write_end      = yramfs_file_writeend,
    .set_page_dirty = yramfs_file_set_page_dirty_no_writeback,
};

const struct file_operations yramfs_file_operations = {
    .read       = do_sync_read,
    .aio_read   = generic_file_aio_read,
    .write      = do_sync_write,
    .aio_write  = generic_file_aio_write,
    .mmap       = generic_file_mmap,
    .fsync      = noop_fsync,
    .splice_read    = generic_file_splice_read,
    .splice_write   = generic_file_splice_write,
    .llseek         = generic_file_llseek,
};

const struct inode_operations yramfs_file_inode_operations = {
    .setattr    = simple_setattr,
    .getattr    = simple_getattr,
};


