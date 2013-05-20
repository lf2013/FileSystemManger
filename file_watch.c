#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <linux/netlink.h>
#include <fcntl.h>
#define NETLINK_TEST 17
#define MSG_LEN 100
#define MAX_MSG_SIZE 1024
struct sockaddr_nl local;
struct sockaddr_nl kaddr;
int skfd;
char file_list[MSG_LEN];  //用于获取配置文件
struct nlmsghdr *nlh;
struct msghdr msg;
struct iovec iov;

int init_sock()
{
	skfd = socket(PF_NETLINK, SOCK_RAW, NETLINK_TEST);
        if(skfd < 0){
            printf("can not create a netlink socket\n");
            return -1;
        }
        nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_MSG_SIZE));

        memset(&local, 0, sizeof(local));
        local.nl_family = AF_NETLINK;
        local.nl_pid = getpid();
        local.nl_groups = 0;
        if(bind(skfd, (struct sockaddr *)&local, sizeof(local)) != 0){
            printf("bind() error\n");
            return -1;
        }
        memset(&kaddr, 0, sizeof(kaddr));
        kaddr.nl_family = AF_NETLINK;
        kaddr.nl_pid = 0;
        kaddr.nl_groups = 0;
}
void init_nlmsg()
{
	memset(&msg,0,sizeof(msg));
	memset(nlh, 0, NLMSG_SPACE(MAX_MSG_SIZE));
        nlh->nlmsg_len = NLMSG_SPACE(MAX_MSG_SIZE);
        nlh->nlmsg_flags = 0;
        nlh->nlmsg_type = 0;
        nlh->nlmsg_seq = 0;
        nlh->nlmsg_pid = local.nl_pid;
	msg.msg_name = (void *)&kaddr;
        msg.msg_namelen = sizeof(kaddr);
	iov.iov_base = (void *)nlh;
        iov.iov_len = NLMSG_SPACE(MAX_MSG_SIZE);
        msg.msg_name = (void *)&kaddr;
        msg.msg_namelen = sizeof(kaddr);
	msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
}
int send_to_kernel(int skfd,char *buf)  //发送信息到内核进程
{
	int ret;
       memset(&msg,0,sizeof(msg));
	memset(nlh, 0, NLMSG_SPACE(MAX_MSG_SIZE));
	

       nlh->nlmsg_len = NLMSG_SPACE(MAX_MSG_SIZE);
       nlh->nlmsg_flags = 0;
       nlh->nlmsg_type = 0;
       nlh->nlmsg_seq = 0;
       nlh->nlmsg_pid = local.nl_pid;
	strcpy(NLMSG_DATA(nlh), buf);
	msg.msg_name = (void *)&kaddr;
       msg.msg_namelen = sizeof(kaddr);
	iov.iov_base = (void *)nlh;
       iov.iov_len = NLMSG_SPACE(MAX_MSG_SIZE);
       msg.msg_name = (void *)&kaddr;
       msg.msg_namelen = sizeof(kaddr);
	msg.msg_iov = &iov;
       msg.msg_iovlen = 1;
       ret = sendmsg(skfd,&msg,0); //发送消息
       printf("to kernel:%s, len:%d\n",(char *)NLMSG_DATA(nlh),nlh->nlmsg_len);
       return ret;
}
int receive_message(int skfd,char* buf)
{
        int ret;
         //接受内核态确认信息
      //  init_nlmsg(); 
	 ret = recvmsg(skfd, &msg,0);
        strcpy(buf,NLMSG_DATA(nlh));
        printf("receive:%s,len:%d\n",buf,strlen(buf));
        return ret;
}
int setProfile(int skfd)
{
        FILE *file = fopen("etc","r");
	int ret;
	if(file == NULL)
	{
		printf("open file \"etc\" error!\n");
		return ;
	}
        memset(file_list,'\0',MSG_LEN*sizeof(char));
        while(fgets(file_list,MSG_LEN,file) != NULL)
        {
                file_list[strlen(file_list) -1] = '\0';//去除回车符
                printf("%s\n",file_list);
                ret = send_to_kernel(skfd,file_list);
        }
	fclose(file);
	return ret;
}

int main(int argc, char* argv[])
{
	char *data = "Let's begin to communicate!";
	char buf[100];
        //初始化
        int ret1,ret2;
	int i = 3;
	init_sock();
	
	/*开始通信之前需要将用户进程ID发送给内核*/
	//ret1 = send_to_kernel(skfd,data); 
       ret1 = setProfile(skfd);
	if(!ret1){
           	perror("send pid:");
           	exit(-1);
	}
       while(1)
	{
		bzero(buf,sizeof(buf));
		 //接受内核态确认信息
                ret2 = receive_message(skfd,buf);
                if(!ret2){
                        perror("recv form kerner:");
                        exit(-1);
                }
                printf("->from kernel:%s\n",buf);

	} 
	close(skfd);
        return 0;
}
