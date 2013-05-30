#include<linux/netlink.h>
#include<net/sock.h>
#include<linux/string.h>
#include <linux/spinlock.h>
#include <linux/semaphore.h>
#define NETLINK_TEST 17

#define F_NUM 20  //文件数
#define F_LEN 100 //文件名长度
#define MAX_MSG_SIZE 1024
#define MSG_CLOSE 101
DEFINE_RWLOCK(lock); //定义读写锁,相当于rwlock_t lock;rwlock_init(lock);
//DECLARE_MUTEX(receive_sem); //声明信号量

char file_list_b[F_NUM][F_LEN]; //黑名单列表
char file_list_w[F_NUM][F_LEN]; //白名单列表

int count_b = 0; //黑名单数量
int count_w = 0; //白名单数量

char receive[100];
struct sk_buff *skb_1;

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
	int len,u_len;
	//char *data = "this is a test message from kernel!";
	//printk("begin kernel_receive!\n");
	//if(down_trylock(&receive_sem))
	//	return;
	write_lock(&lock);
	skb = skb_get(skb_1);

	memset(file_list_w[count_w],'\0',sizeof(file_list_w[count_w]));
	memset(file_list_b[count_b],'\0',sizeof(file_list_b[count_b]));
	memset(receive,'\0',sizeof(receive));
	if(skb->len >= sizeof(struct nlmsghdr)){
		nlh = nlmsg_hdr(skb);
		if(nlh->nlmsg_type == MSG_CLOSE)  //应用程序关闭
		{
		  if(nlh->nlmsg_pid == user_process.pid)
		  {
		     printk("%d %s\n",nlh->nlmsg_pid,(char*)NLMSG_DATA(nlh));
		     user_process.pid = 0;
			/*计数清零*/
		     count_b = 0;
		     count_w = 0;
		  }
		}else{
			user_process.pid = nlh->nlmsg_pid;

			printk("from_user:%s\n",(char *)NLMSG_DATA(nlh));
			len = strlen(NLMSG_DATA(nlh));
			memcpy(receive,NLMSG_DATA(nlh),len);
			if(receive[len - 1] == 'W') //最后一个字符
			{
				receive[len -1] = '\0';
				memcpy(file_list_w[count_w],receive,len-1);
			
				printk("file_list_w[%d]:%s\n",count_w,file_list_w[count_w]);
				count_w++;	
		 		//printk("user_pid:%d\n",user_process.pid);
		 	}else if(receive[len - 1] == 'B'){
				receive[len -1] = '\0';
                        	memcpy(file_list_b[count_b],receive,len-1);

                        	printk("file_list_b[%d]:%s\n",count_b,file_list_b[count_b]);
                        	count_b++;
			}		
			send_to_user(receive);
		}
	}
	kfree_skb(skb);
	write_unlock(&lock);	
	//up(&receive_sem);
	return ;
}
