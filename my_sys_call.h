#include<linux/user.h>
#include<linux/types.h>
#include<linux/fcntl.h>
#include"transmit.h"   //内核与用户进程通信数据定义

#define MAX_MSG 1024

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

static int in_black_filelist(char *filename,char *buf)  //是否在黑名单列表中
{
	int i = 0;
	int ret = 0;
	for(i =0;i < count_b;i++)
	{
		if(strncmp(file_list_b[i],filename,strlen(file_list_b[i])) == 0)
		{
		memset(data,'\0',sizeof(data));
                strcpy(data,filename);
                strcat(data,buf);
                printk("%s\n",data);
		ret = 1;
		break;
		}
	}
	return ret;
}
static int handle_msg(char *filename,char *buf) //在白名单中
{
	int i = 0;
	int ret = 0;
       for(i =0 ;i < count_w; i++)
	{
	     if(strcmp(file_list_w[i],filename) == 0)
	     {
		//printk("file_list[%d]=%s,%s\n",i,file_list[i],filename);
		memset(data,'\0',sizeof(data));
		strcpy(data,filename);
		strcat(data,buf);
		printk("%s\n",data);
		ret = 1;
		break;
	     }
	}
	return ret;
}

asmlinkage int my_open(char __user *filename,int flags,mode_t mode)
{
	ret = 0;
	//printk("call open()\n");
	if(filename != NULL)
	{
		if(in_black_filelist(filename," 在黑名单中，你无法打开该文件或目录!"))
		{	
			send_to_user(data);
			return 0;
		}
		ret = orig_open(filename, flags, mode);

		if((O_WRONLY | O_CREAT | O_TRUNC) & flags)//相当于creat(char *filename,mode_t mode)
		{
			if(ret == -1){
				 if(handle_msg(filename," exist!"))//在白名单中
				{
					send_to_user(data);
				}
			}else{
				if(handle_msg(filename," create!"))//在白名单中
				{
					send_to_user(data);
				}
			}
		}else{
			if(handle_msg(filename," open!"))//在白名单中
			{
				send_to_user(data);
			}
		}
	}

    return ret;
}
/*
asmlinkage ssize_t my_write(int fd,const void *buf,ssize_t count)
{
	ssize_t nbytes;
	//printk("call write()\n");

	nbytes = orig_write(fd,buf,count);
	if(nbytes != 0)
		printk("向文件 %d 写入 %d 字节\n",fd,nbytes);

	return nbytes;
}

asmlinkage int my_creat(char *filename,mode_t mode)
{
	ret = 0;
	//printk("call creat()\n");
	if(in_black_filelist(filename," 当前目录在黑名单中，你无法在该目录创建文件或目录!"))
        {
                send_to_user(data);
                return 0;
        }

	ret = orig_creat(filename,mode);
	if(ret != -1 && handle_msg(filename," created by system call of create!"))
		send_to_user(data);
        return ret;

}

asmlinkage int my_unlink(char *filename)
{
	ret = 0;
	//printk("call unlink()\n");
	if(in_black_filelist(filename," in the black file list,you cannot unlink it!"))
        {
                send_to_user(data);
                return 0;
        }
	
	ret = orig_unlink(filename);
	if(ret != -1 && handle_msg(filename," was unlinked!"))
		send_to_user(data);
	return ret;
}

asmlinkage int my_mkdir(char *pathname,mode_t mode)
{
	ret = 0;
	//printk("call mkdir()\n");
	if(in_black_filelist(pathname," in the black file list,you cannot create it!"))
	{
		send_to_user(data);
		return 0;
	}
	ret = orig_mkdir(pathname,mode);
	if(ret != -1 && handle_msg(pathname," dir create success!"))
                send_to_user(data);
	else
		send_to_user(strcat(pathname," create failed!"));

	return ret;
}*/
