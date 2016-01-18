#include <linux/keyboard.h>

static struct notifier_block keyboard_notifier = {
  .notifier_call = keyboard_hook
};

int keyboard_hook(struct notifier_block *, unsigned long code, void *);

register_keyboard_notifier(&keyboard_notifier);
