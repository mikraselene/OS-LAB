#ifndef LAB3_SRC_VMA_VMA_H_
#define LAB3_SRC_VMA_VMA_H_

#ifndef CONFIG_MMU
#define CONFIG_MMU
#endif

struct vma_struct {
  unsigned long vm_start; /* Our start address within vm_mm. */
  unsigned long vm_end;   /* The first byte after our end address within vm_mm. */
  unsigned long vm_flags; /* Flags, see mm.h. */
  char filename[200];
};
struct buffer {
  unsigned long task_size;      /* size of task vm space */
  unsigned long highest_vm_end; /* highest vma end address */
  int mm_users;                 /* */
  int mm_count;                 /* */
  long long pgtables_bytes;     /* PTE page table pages*/
  int map_count;                /* number of VMAs */
  unsigned long total_vm;       /* Total pages mapped */
  unsigned long locked_vm;      /* Pages that have PG_mlocked set */
  unsigned long pinned_vm;      /* Refcount permanently increased */
  unsigned long data_vm;        /* VM_WRITE & ~VM_SHARED & ~VM_STACK */
  unsigned long exec_vm;        /* VM_EXEC & ~VM_WRITE & ~VM_STACK */
  unsigned long stack_vm;       /* VM_STACK */
  unsigned long start_code, end_code, start_data, end_data;
  unsigned long start_brk, brk, start_stack;
  unsigned long arg_start, arg_end, env_start, env_end;
};

#endif  // LAB3_SRC_VMA_VMA_H_
