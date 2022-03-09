# mmap文件映射

![20211226221439](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211226221439.png)

1. 映射大小超过文件大小1个page, 读取的时候会出现Bus Error. 因为并没有分配真实的物理内存.

2. Segment Error, 代表有真实的物理内存. 但是权限不匹配

3. 按页分配, 且页边界对齐

![20211230091945](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211230091945.png)

## mmap的一些原理

早期搞内核的时候, 已经详细的了解对应的使用规则, https://bbs.pediy.com/thread-269784.htm

## 一些mmap值得考虑的问题

为什么mmap能防止用户态和内核态的内存拷贝


## linker load elf with mmap

todo??