#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/mm.h>

#include"my_sys_call.h"
#include"get_call_table.h"
// 模块信息
MODULE_LICENSE("GPL");

void **my_call_table;//存放系统调用表

// 模块载入时被调用

static int __init init_my_module(void)

{
	my_call_table =	(void **)get_call_table();
	if(my_call_table == NULL)
		return -1;
	else printk("sys_call_table\t%X\n",(unsigned)my_call_table);
	return 0;
}



// 模块卸载时被调用

static void __exit clean_my_module(void)
{
}

module_init(init_my_module);
module_exit(clean_my_module);
