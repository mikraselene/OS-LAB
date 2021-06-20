#include "pfstat.h"

#include <asm/page.h>
#include <linux/fs.h> /* struct file_operations, struct file */
#include <linux/init_task.h>
#include <linux/kernel.h>     /* KERN_INFO macros */
#include <linux/miscdevice.h> /* struct miscdevice and misc_[de]register() */
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/module.h> /* required for all kernel modules */
#include <linux/pfn.h>
#include <linux/uaccess.h> /* copy_{to,from}_user() */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mikra Selene");
MODULE_DESCRIPTION("page frame stat kernel module");

#ifndef ARCH_PFN_OFFSET
#define ARCH_PFN_OFFSET (0UL)
#endif

static int pfstat_open(struct inode *inode, struct file *file) {
  struct page_info *buf;
  int err = 0;
  buf = kmalloc(sizeof(struct page_info), GFP_KERNEL);
  file->private_data = buf;
  return err;
}

static ssize_t pfstat_read(struct file *file, char __user *out, size_t size, loff_t *off) {
  struct page_info *buf = file->private_data;
  memset(buf, 0, sizeof(struct page_info));

  buf->num_physpages = get_num_physpages();
  printk(KERN_INFO "OK: %lu phys page(s) found. \n", buf->num_physpages);

  for (int cur_pfn = 0; cur_pfn < buf->num_physpages; cur_pfn++) {
    struct page *hoc = pfn_to_page(cur_pfn + ARCH_PFN_OFFSET);
    buf->valid += pfn_valid(cur_pfn + ARCH_PFN_OFFSET);
    buf->free += hoc->_refcount.counter == 0;
    buf->locked += PageLocked(hoc);
    buf->reserved += PageReserved(hoc);
    buf->swapcache += PageSwapCache(hoc);
    buf->referenced += PageReferenced(hoc);
    buf->slab += PageSlab(hoc);
    buf->private += PagePrivate(hoc);
    buf->uptodate += PageUptodate(hoc);
    buf->dirty += PageDirty(hoc);
    buf->active += PageActive(hoc);
    buf->writeback += PageWriteback(hoc);
    buf->mappedtodisk += PageMappedToDisk(hoc);
  }
  printk(KERN_INFO "OK: flags unwrapped in %lu page(s).\n", buf->num_physpages);

  copy_to_user(out, buf, sizeof(struct page_info));
  printk(KERN_INFO "OK: copy_to_user done.\n");
  return size;
}

static int pfstat_close(struct inode *inode, struct file *file) {
  struct buffer *buf = file->private_data;
  kfree(buf);
  return 0;
}

static struct file_operations pfstat_fops = {
    .owner = THIS_MODULE,
    .open = pfstat_open,
    .read = pfstat_read,
    .release = pfstat_close,
    .llseek = noop_llseek,
};

static struct miscdevice pfstat_misc_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "pfstat",
    .fops = &pfstat_fops,
};

static int __init pfstat_init(void) {
  misc_register(&pfstat_misc_device);
  printk(KERN_INFO "pfstat device has been registered.\n");

  return 0;
}

static void __exit pfstat_exit(void) {
  misc_deregister(&pfstat_misc_device);
  printk(KERN_INFO "pfstat device has been unregistered.\n");
}

module_init(pfstat_init);
module_exit(pfstat_exit);
