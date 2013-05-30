#ifndef __KERNEL__
#define __KERNEL__
#endif

#ifndef MODULE
#define MODULE
#endif

#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/mm.h>
#include<linux/init.h>
#include<linux/syscalls.h>
#include<asm/unistd.h>

#include"my_sys_call.h"     //自定义调用表
#include"get_call_table.h"  //获取系统调用表

// 模块信息
MODULE_LICENSE("GPL");
MODULE_AUTHOR("github/gwcai 2013/4");

void **my_call_table;//存放系统调用表
unsigned int orig_cr0;

/*clear up WP bit of CR0*/
unsigned int clear_and_return_cr0(void)
{
	unsigned int cr0 = 0;
	unsigned int ret;
	asm volatile("movl %%cr0,%%eax"
			: "=a"(cr0)
		     );
	ret = cr0;
	cr0 &= 0xfffeffff;//clear the 16 bit of CR0
	asm volatile("movl %%eax,%%cr0"
			:
			: "a"(cr0)
		    );
	return ret;
}
/*set cr0 with new value*/
void setback_cr0(unsigned val)
{
	asm volatile("movl %%eax,%%cr0"
			:
			: "a"(val)
		    );
}
/*替换函数*/
static int intercept_init(void)
{
	my_call_table = (void **)get_call_table();
        if(my_call_table == NULL)
                return -1;
        else printk("sys_call_table\t%x\n",(unsigned)my_call_table);
	
	#define REPLACE(x) orig_##x = my_call_table[__NR_##x];\
		my_call_table[__NR_##x] = my_##x

	REPLACE(open); //替换open函数
//	REPLACE(write);//替换write函数
//	REPLACE(creat);//替换creat函数
//	REPLACE(unlink);//替换unlink函数
//	REPLACE(mkdir);
	return 0;
}
// 模块载入时被调用
static int __init init_my_module(void)

{
	int ret;
	/*创建套接字,nl_fd在transmit.h内定义*/
	nl_fd = netlink_kernel_create(&init_net,NETLINK_TEST,0,kernel_receive,NULL,THIS_MODULE);

	if(!nl_fd)
		printk("can not create a netlink socket!\n");
	orig_cr0 = clear_and_return_cr0();
	ret = intercept_init();
	setback_cr0(orig_cr0);
	return ret;
}



/* 模块卸载时被调用，主要是还原系统调用*/
static void __exit clean_my_module(void)
{
	#define RESTORE(x) my_call_table[__NR_##x] = orig_##x

	orig_cr0 = clear_and_return_cr0();   
	RESTORE(open);
//	RESTORE(write);
//	RESTORE(unlink);
//	RESTORE(creat);
//	RESTORE(mkdir);
	setback_cr0(orig_cr0);
	sock_release(nl_fd->sk_socket);//关闭套接字
}

module_init(init_my_module); //初始化模块
module_exit(clean_my_module); //卸载模块
