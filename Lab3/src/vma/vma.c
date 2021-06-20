#include "vma.h"

#include <linux/fs.h>          /* struct file_operations, struct file */
#include <linux/init.h>        /* __init and __exit macroses */
#include <linux/kernel.h>      /* KERN_INFO macros */
#include <linux/miscdevice.h>  /* struct miscdevice and misc_[de]register() */
#include <linux/module.h>      /* required for all kernel modules */
#include <linux/moduleparam.h> /* module_param() and MODULE_PARM_DESC() */
#include <linux/slab.h>        /* kzalloc() function */
#include <linux/uaccess.h>     /* copy_{to,from}_user() */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mikra Selene");
MODULE_DESCRIPTION("vma kernel module");

static int vma_open(struct inode *inode, struct file *file) {
  struct buffer *buf;
  int nvma = current->mm->map_count;
  int err = 0;
  buf = kmalloc(sizeof(struct buffer) + nvma * sizeof(struct vma_struct), GFP_KERNEL);

  file->private_data = buf;
  return err;
}

static ssize_t vma_read(struct file *file, char __user *out, size_t size, loff_t *off) {
  struct mm_struct *cur_mm = current->mm;
  struct buffer *buf = file->private_data;
  if (size == sizeof(struct buffer)) {
    buf->map_count = cur_mm->map_count;
    printk(KERN_INFO "OK: %d vma(s) found.\n", buf->map_count);
  } else {
    buf->task_size = current->mm->task_size;
    buf->highest_vm_end = cur_mm->highest_vm_end;
    buf->mm_users = cur_mm->mm_users.counter;
    buf->mm_count = cur_mm->mm_count.counter;
    buf->pgtables_bytes = cur_mm->pgtables_bytes.counter;
    buf->map_count = cur_mm->map_count;
    buf->total_vm = cur_mm->total_vm;
    buf->locked_vm = cur_mm->locked_vm;
    buf->pinned_vm = cur_mm->pinned_vm.counter;
    buf->data_vm = cur_mm->data_vm;
    buf->exec_vm = cur_mm->exec_vm;
    buf->stack_vm = cur_mm->stack_vm;
    buf->start_code = cur_mm->start_code;
    buf->end_code = cur_mm->end_code;
    buf->start_data = cur_mm->start_data;
    buf->end_data = cur_mm->end_data;
    buf->start_brk = cur_mm->start_brk;
    buf->brk = cur_mm->brk;
    buf->start_stack = cur_mm->start_stack;
    buf->arg_start = cur_mm->arg_start;
    buf->arg_end = cur_mm->arg_end;
    buf->env_start = cur_mm->env_start;
    buf->env_end = cur_mm->env_end;
    printk(KERN_INFO "OK: information of mm_struct got.\n");

    int nvma = (size - sizeof(struct buffer)) / sizeof(struct vma_struct);
    struct vma_struct *ret_vma_list = (struct vma_struct *)(buf + 1);
    struct vm_area_struct *vma_list = cur_mm->mmap;
    for (int i = 0; i < nvma; i++) {
      ret_vma_list[i].vm_start = vma_list[i].vm_start;
      ret_vma_list[i].vm_end = vma_list[i].vm_end;
      ret_vma_list[i].vm_flags = vma_list[i].vm_flags;
      if (vma_list[i].vm_file && vma_list[i].vm_file->f_path.dentry) {
        strncpy(ret_vma_list[i].filename, vma_list[i].vm_file->f_path.dentry->d_iname, 40);
      } else {
        strncpy(ret_vma_list[i].filename, "NULL", 5);
      }
    }
    printk(KERN_INFO "OK: information of %d vma(s) got.\n", nvma);
  }
  copy_to_user(out, buf, size);
  printk(KERN_INFO "OK: copy_to_user done.\n");
  return size;
}

static int vma_close(struct inode *inode, struct file *file) {
  struct buffer *buf = file->private_data;
  kfree(buf);
  return 0;
}

static struct file_operations vma_fops = {
    .owner = THIS_MODULE,
    .open = vma_open,
    .read = vma_read,
    .release = vma_close,
    .llseek = noop_llseek,
};

static struct miscdevice vma_misc_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "vma",
    .fops = &vma_fops,
};

static int __init vma_init(void) {
  misc_register(&vma_misc_device);
  printk(KERN_INFO "vma device has been registered.\n");

  return 0;
}

static void __exit vma_exit(void) {
  misc_deregister(&vma_misc_device);
  printk(KERN_INFO "vma device has been unregistered.\n");
}

module_init(vma_init);
module_exit(vma_exit);
