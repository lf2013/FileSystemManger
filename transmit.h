#include<linux/netlink.h>
#include<net/sock.h>
#include<linux/string.h>
#include <linux/spinlock.h>
#include <linux/semaphore.h>
#define NETLINK_TEST 17

#define F_NUM 20  //文件数
#define F_LEN 100 //文件名长度
#define MAX_MSG_SIZE 1024

DEFINE_RWLOCK(lock); //定义读写锁,相当于rwlock_t lock;rwlock_init(lock);
DECLARE_MUTEX(receive_sem); //声明信号量

char file_list[F_NUM][F_LEN];
int count = 0;
/*用户进程ID结构*/
struct {
        __u32 pid;
}user_process;

static struct sock *nl_fd = NULL;//内核socket

static int send_to_user(char *buf)
{
	int size;
	struct sk_buff *skb;
	struct nlmsghdr *nlh ;   //报文头
	int ret;
	int len = strlen(buf);
	size = NLMSG_SPACE(MAX_MSG_SIZE); //报文大小
	if(user_process.pid <= 0 || nl_fd == NULL || buf == NULL)
    	{
        	return 0;
    	}
	/*分配新的套接字缓存*/
	skb = alloc_skb(size,GFP_KERNEL);
	buf[len] = '\0';
	/*初始化一个netlink消息首部*/
	nlh = nlmsg_put(skb,0,0,0,MAX_MSG_SIZE,0);
	memcpy(NLMSG_DATA(nlh),buf,len+1); //填充数据区
	
	//设置控制字段
	NETLINK_CB(skb).pid = 0;
	NETLINK_CB(skb).dst_group = 0;
	
	//printk("skb->data:%s\n",(char *)NLMSG_DATA((struct nlmsghdr *)skb->data));
	//发送数据
	ret = netlink_unicast(nl_fd,skb,user_process.pid,MSG_DONTWAIT);
	return ret;
}

static void kernel_receive(struct sk_buff *skb_1)
{
	struct sk_buff *skb;
	struct nlmsghdr *nlh = NULL;
	char *data = "this is a test message from kernel!";
	//printk("begin kernel_receive!\n");
	if(down_trylock(&receive_sem))
		return;
	write_lock(&lock);
	skb = skb_get(skb_1);
	memset(file_list[count],'\0',sizeof(file_list[count]));

	if(skb->len >= sizeof(struct nlmsghdr)){
		nlh = nlmsg_hdr(skb);

		user_process.pid = nlh->nlmsg_pid;

		printk("from_user:%s\n",(char *)NLMSG_DATA(nlh));
		memcpy(file_list[count],NLMSG_DATA(nlh),strlen(NLMSG_DATA(nlh)));
		printk("file_list[%d]:%s\n",count,file_list[count]);
		count++;	
		 //printk("user_pid:%d\n",user_process.pid);
		send_to_user(file_list[count-1]);
	}
	kfree_skb(skb);
	write_unlock(&lock);	
	up(&receive_sem);
	return ;
}
