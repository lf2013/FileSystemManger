#include<linux/user.h>
#include<linux/types.h>
#include<linux/fcntl.h>
int ret = 0;  //返回值
/******************************原始系统调用函数表******************************/

asmlinkage ssize_t (*orig_write)(int fd,const void *buf,ssize_t count);
asmlinkage int (*orig_open)(char __user *filename,int flags,mode_t mode);
asmlinkage ssize_t (*orig_read)(int fd,void *buf,size_t count);
asmlinkage int (*orig_creat)(const char *filename,mode_t mode);
asmlinkage int (*orig_unlink)(const char *filename);
asmlinkage int (*orig_mkdir)(const char *pathname,mode_t mode);
/******************************自定义系统调用函数表****************************/

asmlinkage int my_open(char __user *filename,int flags,mode_t mode)
{
    ret = 0;
	printk("call open()\n");

	if(filename != NULL)
	{
		ret = orig_open(filename, O_EXCL, mode);
		if(ret == -1)
	        printk("%s was already existed\n", filename);
		ret = orig_open(filename, O_CREAT, mode);
	        printk("%s was created!", filename);
//		printk("flag == %d\n", flags);
	}

    return ret;
}

asmlinkage ssize_t my_write(int fd,const void *buf,ssize_t count)
{
	ssize_t nbytes;
	printk("call write()\n");

	nbytes = orig_write(fd,buf,count);
	if(nbytes != 0)
		printk("向文件 %d 写入 %d 字节\n",fd,nbytes);

	return nbytes;
}

asmlinkage int my_creat(const char *filename,mode_t mode)
{
	ret = 0;
	printk("call creat()\n");
	ret = orig_creat(filename,mode);
	if(ret != -1)
       		 printk("file %s was created!\n",filename);
        return ret;

}

asmlinkage int my_unlink(const char *filename)
{
	ret = 0;
	printk("call unlink()\n");
	ret = orig_unlink(filename);
	if(ret != -1)
       		 printk("file %s was unlinked!\n",filename);
	return ret;
}

asmlinkage int my_mkdir(const char *pathname,mode_t mode)
{
	ret = 0;
	printk("call mkdir()\n");
	if(strcmp(pathname,"mydir") != 0)
	{
		ret = orig_mkdir(pathname,mode);
		if(ret == -1)
			printk("创建目录 %s 失败!\n",pathname);
		else
			printk("创建目录 %s 成功!\n",pathname);
	}else
		printk("拒绝创建!\n");

	return ret;
}
