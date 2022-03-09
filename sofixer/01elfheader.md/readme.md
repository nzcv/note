# ELF修复基本工作原理

![20220107175810](https://cdn.jsdelivr.net/gh/nzcv/picgo/20220107175810.png)

本文参考[https://github.com/F8LEFT/SoFixer](https://github.com/F8LEFT/SoFixer)相关代码进行了代码的review, 尝试梳理内部流程, 也算是自己对于linker的一个阶段性总结

## 分析

# tmp.so 和原始文件比较
![20220107174615](https://cdn.jsdelivr.net/gh/nzcv/picgo/20220107174615.png)

![20211231165901](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211231165901.png)

![20211231165939](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211231165939.png)

#帮助文档

http://pareto.fun/?p=292

# elf剔除

[android-elf-cleaner](https://github.com/kost/android-elf-cleaner)

# elf修复

1. 由于从内存中dump，将文件偏移和大小 覆写为 内存大小和偏移。

2. 从program header 中获取找到PT_DYNAMIC 。将dynamic 作为构建section header 的关键信息。

3. PT_DYNAMIC包含了部分节信息, 还原这部分内容

4. 


[frida_dump](https://github.com/lasting-yang/frida_dump)