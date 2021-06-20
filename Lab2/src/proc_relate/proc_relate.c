#include "proc_relate.h"

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
MODULE_DESCRIPTION("proc_relate kernel module");

static int proc_relate_open(struct inode *inode, struct file *file) {
  struct proc_info *buf;
  int err = 0;
  buf = kmalloc(sizeof(struct proc_info) * 30, GFP_KERNEL);
  file->private_data = buf;
  return err;
}

static ssize_t proc_relate_read(struct file *file, char __user *out, size_t size, loff_t *off) {
  struct proc_info *buf = file->private_data;
  int cur_sessionid = current->sessionid;
  struct task_struct *p;
  int cnt = 0;
  struct proc_info *info = buf;
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
      if (cnt >= 30) {
        break;
      }
    }
  }
  copy_to_user(out, buf, sizeof(struct proc_info) * cnt);
  return sizeof(struct proc_info) * cnt;
}

static int proc_relate_close(struct inode *inode, struct file *file) {
  struct buffer *buf = file->private_data;
  kfree(buf);
  return 0;
}

static struct file_operations proc_relate_fops = {
    .owner = THIS_MODULE,
    .open = proc_relate_open,
    .read = proc_relate_read,
    .release = proc_relate_close,
    .llseek = noop_llseek,
};

static struct miscdevice proc_relate_misc_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "proc_relate",
    .fops = &proc_relate_fops,
};

static int __init proc_relate_init(void) {
  misc_register(&proc_relate_misc_device);
  printk(KERN_INFO "proc_relate device has been registered.\n");

  return 0;
}

static void __exit proc_relate_exit(void) {
  misc_deregister(&proc_relate_misc_device);
  printk(KERN_INFO "proc_relate device has been unregistered.\n");
}

module_init(proc_relate_init);
module_exit(proc_relate_exit);
