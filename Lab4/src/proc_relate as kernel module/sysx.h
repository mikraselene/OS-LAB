#ifndef LAB4_SRC_PROC_RELATE_AS_KERNEL_MODULE_SYSX_H_
#define LAB4_SRC_PROC_RELATE_AS_KERNEL_MODULE_SYSX_H_

// size = 88
struct proc_info {
  volatile long state;
  int pid;
  int tgid;
  char comm[30];    // 进程所执的程序
  int prio;         // 动态优先级
  int static_prio;  // 静态优先级
  struct mm_struct *mm;
  struct mm_struct *active_mm;
  int sessionid;
  int real_parent;
  int parent;
  int group_leader;
} __attribute__((packed));

struct buffer {
  int count;
  struct proc_info procs[];
} __attribute__((packed));

#endif  // LAB4_SRC_PROC_RELATE_AS_KERNEL_MODULE_SYSX_H_
