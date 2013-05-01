#include<linux/mm.h>
// 中断描述符表寄存器结构

struct {
        unsigned short limit;  //界限
        unsigned int base;       //基地址
        } __attribute__((packed)) idtr;

// 中断描述符表结构

struct {
        unsigned short off1;
        unsigned short sel;
        unsigned char none, flags;
        unsigned short off2;
        } __attribute__((packed)) idt;

//搜索前三个字节等于\xff\x14\x85的内存地址函数
static void *memsearch(const char *startaddr, size_t offset,
            const char *needle, size_t needle_len)
{
    const char *begin; 		//起始地址
    const char *end = (const char *) startaddr + offset - needle_len;//终止地址
    if (needle_len == 0) 	//无需搜索
        return (void *) startaddr;    
    if (__builtin_expect(offset < needle_len, 0))/* offset < needle_len 则返回*/
        return NULL;

    for (begin = (const char *) startaddr; begin <= end ; ++begin)
	/*判断内存地址前三个字节是否等于\xff\x14\x85*/
        if (begin[0] == ((const char *) needle)[0]
            && !memcmp((const void *) &begin[1],
                   (const void *) ((const char *) needle+1),needle_len-1))
            return (void *) begin;

    return NULL;
}

// 查找sys_call_table的地址
static unsigned long get_call_table(void)
{
	unsigned int sys_call_off;
	unsigned int sys_call_table;
	char sc_asm[100],*p;

	// 获取中断描述符表寄存器的地址
	asm("sidt %0":"=m"(idtr));
	printk("addr of idtr: %x\n", (unsigned)&idtr);
	// 获取0x80中断处理程序的地址,即system_call的地址
	memcpy(&idt, (char *)idtr.base+8*0x80, sizeof(idt));
	sys_call_off=((idt.off2<<16)|idt.off1);

	printk("addr of idt 0x80: %x\n", sys_call_off);
	// 从0x80中断服务例程中搜索sys_call_table的地址
	memcpy(sc_asm,(void *)sys_call_off,100);
	p = (char *)memsearch(sc_asm,100,"\xff\x14\x85",3);
	if(p == NULL)
		return 0;
	sys_call_table = *(unsigned *)(p+3);
	if(p)
		printk("addr of sys_call_table: %x\n",sys_call_table);
	return sys_call_table;		
}
