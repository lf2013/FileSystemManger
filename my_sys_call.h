#include<linux/user.h>
#include<linux/types.h>
#include<linux/fcntl.h>
#include"transmit.h"   //内核与用户进程通信数据定义

#define MAX_MSG 50

int ret = 0;  //返回值
char data[MAX_MSG];

/******************************原始系统调用函数表******************************/

asmlinkage ssize_t (*orig_write)(int fd,const void *buf,ssize_t count);
asmlinkage int (*orig_open)(char __user *filename,int flags,mode_t mode);
asmlinkage ssize_t (*orig_read)(int fd,void *buf,size_t count);
asmlinkage int (*orig_creat)(const char *filename,mode_t mode);
asmlinkage int (*orig_unlink)(const char *filename);
asmlinkage int (*orig_mkdir)(const char *pathname,mode_t mode);

/******************************自定义系统调用函数表****************************/
static int handle_msg(char *filename,const char *data1)
{
	int ret = 0;
	int i = 0;
	//memset(data,'\0',MAX_MSG*sizeof(char));
	//memcpy(data,filename,strlen(filename));
	//if(filename != NULL)
            //ret = send_to_user(filename);
        for(i =0 ;i < count; i++)
	{
	     if(strncmp(file_list[i],filename,strlen(file_list[i])) == 0)
	     {
		printk("file_list[%d]=%s,%s\n",i,file_list[i],filename);
		//memcpy(data,filename,strlen(filename));
		//ret = send_to_user(filename);
		ret = 1;
	     }
	}
	return ret;
}

asmlinkage int my_open(char __user *filename,int flags,mode_t mode)
{
    	const char *data1 = " exist";
	const char *data2 = " create";
	const char *data3 = " opened" ;
	memset(data,'\0',MAX_MSG*sizeof(char));	
	ret = 0;
	printk("call open()\n");
	//send_to_user("call open()");
	if(filename != NULL)
	{
		ret = orig_open(filename, flags, mode);

		if((O_WRONLY | O_CREAT | O_TRUNC) && flags)	//相当于creat(char *filename,mode_t mode)
		{
			if(ret == -1) handle_msg(filename,data1);
			else handle_msg(filename,data2);

		}else handle_msg(filename,data3);
	}

    return ret;
}
/*
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
*/
