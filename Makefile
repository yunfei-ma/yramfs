
obj-m := yramfs.o

yramfs-objs := yramfs_module.o\
               yramfs_super.o\
               yramfs_inode.o\
               yramfs_utils.o\
               yramfs_dir.o\
               yramfs_vector.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
