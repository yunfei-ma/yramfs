#ifndef _yramfs_utils_h
#define _yramfs_utils_h

#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
#define DBG_PRINT(x, ...); do {\
                printk(KERN_INFO "[ramfs]:" x , ##__VA_ARGS__);\
                printk(KERN_INFO "\t@%s%d\n", __FUNCTION__, __LINE__);\
            }while(0)
#else
#define DBG_PRINT(x, ...); do {} while(0);
#endif

typedef void (*free_func_t)(void*);

typedef struct _yramfs_vector_t{
    struct _yramfs_vector_t     *pNext;
    uint32_t                     data;
    free_func_t                  free;
}yramfs_vector_t;

#endif
