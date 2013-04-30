/*communication.h*/

#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

#define OPS_BASIC	128
#define SEND	OPS_BASIC
#define GET    OPS_BASIC
#define MAX    OPS_BASIC+1

#endif

#include<linux/netfilter_ipv4.h>
#include<linux/string.h>

static void data_to_kernel(char *buf,void *user)
{
	copy_from_user(buf,user,MAX*sizeof(char));
	printk("umsg:%s\n",buf);
}
static void data_from_kernel(char *buf,void *user,unsigned len)
{
	copy_to_user(user,buf,len);
}
/*用于内核与用户进程通信的socket结构*/

static struct nf_sockopt_ops my_sockops =
{
  .pf = PF_INET,
  .set_optmin = SEND,
  .set_optmax = MAX,
  .set = data_to_kernel,
  .get_optmin = GET,
  .get_optmax = MAX,
  .get = data_from_kernel,
};
