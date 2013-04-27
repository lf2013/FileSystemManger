#include<linux/user.h>
#include<linux/types.h>
#include<linux/fcntl.h>

/******************************原始系统调用函数表******************************/

asmlinkage size_t (*orig_write)(int fd,const void *buf,size_t count);
asmlinkage long (*orig_open)(char __user *filename,int flags,mode_t mode);
asmlinkage ssize_t (*orig_read)(int fd,void *buf,size_t count);
asmlinkage int (*orig_creat)(const char *filename,mode_t mode);
asmlinkage int (*orig_unlink)(const char *filename);

/******************************自定义系统调用函数表****************************/

asmlinkage long my_open(char __user *filename,int flags,mode_t mode)
{
        printk("call open()\n");
	if(filename != NULL)
	        printk("%s was opened!\n",filename);
        return orig_open(filename,flags,mode);
}
asmlinkage size_t my_write(int fd,const void *buf,size_t count)
{
	printk("call write()\n");
	if(count != 0)
		printk("向文件 %d 写入 %d 字节\n",fd,count);
	return orig_write(fd,buf,count);
}
asmlinkage int my_creat(const char *filename,mode_t mode)
{
	 printk("call creat()\n");
	if(filename != NULL)
       		 printk("file %s was created!\n",filename);
        return orig_creat(filename,mode);

}
asmlinkage int my_unlink(const char *filename)
{
	 printk("call unlink()\n");
	if(filename != NULL)
       		 printk("file %s was removed!\n",filename);
	return orig_unlink(filename);
}
