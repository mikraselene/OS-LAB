#include "sysx.h"

#include <linux/fs.h>          /* struct file_operations, struct file */
#include <linux/init.h>        /* __init and __exit macroses */
#include <linux/init_task.h>   /* ... */
#include <linux/kernel.h>      /* KERN_INFO macros */
#include <linux/miscdevice.h>  /* struct miscdevice and misc_[de]register() */
#include <linux/module.h>      /* required for all kernel modules */
#include <linux/moduleparam.h> /* module_param() and MODULE_PARM_DESC() */
#include <linux/slab.h>        /* kzalloc() function */
#include <linux/uaccess.h>     /* copy_{to,from}_user() */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mikra Selene");
MODULE_DESCRIPTION("sysx kernel module");

static int sysx_open(struct inode *inode, struct file *file) {
  struct buffer *buf;
  int err = 0;
  buf = kmalloc(4096, GFP_KERNEL);
  file->private_data = buf;
  return err;
}

static ssize_t sysx_read(struct file *file, char __user *out, size_t size, loff_t *off) {
  struct buffer *buf = file->private_data;
  struct proc_info *info = buf->procs;
  int cur_sessionid = current->sessionid;
  struct task_struct *p;
  int cnt = 0;
  for_each_process(p) {
    if (p->sessionid == cur_sessionid) {
      info->state = 0;
      info->pid = p->pid;
      info->tgid = p->tgid;
      strncpy(info->comm, p->comm, 16);
      info->prio = p->prio;
      info->static_prio = p->static_prio;
      info->mm = p->mm;
      info->active_mm = p->active_mm;
      info->sessionid = p->sessionid;
      info->real_parent = p->real_parent->pid;
      info->parent = p->parent->pid;
      info->group_leader = p->group_leader->pid;
      info++;
      cnt++;
      if (cnt > ((4096 - sizeof(int)) / sizeof(struct proc_info)) - 1) {
        break;
      }
    }
  }
  buf->count = cnt;
  copy_to_user(out, buf, 4096);
  return 4096;
}

static int sysx_close(struct inode *inode, struct file *file) {
  struct buffer *buf = file->private_data;
  kfree(buf);
  return 0;
}

static struct file_operations sysx_fops = {
    .owner = THIS_MODULE,
    .open = sysx_open,
    .read = sysx_read,
    .release = sysx_close,
    .llseek = noop_llseek,
};

static struct miscdevice sysx_misc_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "sysx",
    .fops = &sysx_fops,
};

static int __init sysx_init(void) {
  misc_register(&sysx_misc_device);
  printk(KERN_INFO "sysx device has been registered.\n");

  return 0;
}

static void __exit sysx_exit(void) {
  misc_deregister(&sysx_misc_device);
  printk(KERN_INFO "sysx device has been unregistered.\n");
}

module_init(sysx_init);
module_exit(sysx_exit);
