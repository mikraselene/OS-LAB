#ifndef LAB2_SRC_PROC_RELATE_PROC_RELATE_H_
#define LAB2_SRC_PROC_RELATE_PROC_RELATE_H_

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
};

#endif  // LAB2_SRC_PROC_RELATE_PROC_RELATE_H_
