#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

#define BUF_SIZE 20

static struct task_struct *my_thread = NULL;

static int print(void *data) {
  while (!kthread_should_stop()) {
    printk("New kthread is running.");
    msleep(2000);
  }
  return 0;
}

static int __init kthread_init(void) {
  printk("Create kernel thread!\n");
  my_thread = kthread_run(print, NULL, "new_kthread");
  return 0;
}

static void __exit kthread_exit(void) {
  printk("Kill new kthread.\n");
  if (my_thread) kthread_stop(my_thread);
}

module_init(kthread_init);
module_exit(kthread_exit);
