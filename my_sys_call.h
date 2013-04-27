#include<linux/user.h>
asmlinkage int (*orig_write)(unsigned int fd,char *buf,unsigned int count);
asmlinkage long (*orig_open)(char __user *filename,int flags,int mode);
