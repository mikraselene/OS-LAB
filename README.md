OS-LAB
---
本学期操作系统的实验，暂且存在这里。里面大多是一些简单的 Linux 内核模块。

Some simple Linux kernel modules developed for OS labs.

Lab 1
---
- 就是编译一个内核，所以没有代码也没有实验报告。

Lab 2
---
1. 编写一个内核模块，创建一个内核线程，并在模块退出时杀死该线程。
2. 编写一个内核模块，实现读取系统一分钟内的 CPU 负载。
3. 编写一个内核模块，打印当前系统处于运行状态的进程的 PID 和名字。
4. 编写一个设备驱动，实现读取当前会话中的所有进程的信息。
5. 编写用户态应用程序，解决生产者-消费者问题、理发师问题、读者-写者问题。

Lab 3
---
1. 使用 `kmalloc` 分配 1KB、8KB 的内存，打印指针地址。
2. 使用 `vmalloc` 分配 8KB、1MB、64MB的内存，打印指针地址。
3. 编写一个设备驱动 pfstat.ko，用于读取物理页框属性。
4. 编写一个设备驱动 vma.ko，用于获得每个 VMA 的信息。

Lab 4
---
1. 编写内核模块，使用 tasklet 实现打印 helloworld。
2. 编写内核模块，用工作队列实现周期打印 helloworld。
3. 在用户态编写一个信号捕获程序，捕获终端按键信号（中止进程、中止进程并保存内存信息、停止进程）。
4. 为 openEuler 增加一个系统调用，当用户程序调用这个系统调用时，返回给用户程序当前进程、当前进程的父进程以及当前进程的所有子进程信息。
