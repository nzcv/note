# 数据宽度

![20211213175101](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211213175101.png)

    DCB data control byte(byte)
    DCW data control half word(short)
    DCD data control word(int)
    DCQ data control (long)
![20220116224329](https://cdn.jsdelivr.net/gh/nzcv/picgo/20220116224329.png)


## KEIL汇编和GNU汇编的区别

![20220116224519](https://cdn.jsdelivr.net/gh/nzcv/picgo/20220116224519.png)


## IDA关于please position the cursor within a funtion的解决

```shell
我是在C/C++反编译中出现的问题，因为在动态调试的时候IDA不能解析出函数段导致：

解决办法：

步骤1：右键code，会出现汇编代码；

步骤2：右键creat fun 

完成步骤1和步骤2，就可以正常的反编译出伪代码。
```

https://blog.csdn.net/yiheliangmaoqian/article/details/78140793


## IDA Pro F5 出现 positive sp value has been found

https://bbs.pediy.com/thread-254990.htm

## IDA查看机器指令

![20220124223257](https://cdn.jsdelivr.net/gh/nzcv/picgo/20220124223257.png)

## IDA切换主题

Options->Color

![20220120113228](https://cdn.jsdelivr.net/gh/nzcv/picgo/20220120113228.png)

## 在十六进制窗口中，这个内存位置的内容显示为 ?? ?? ?? ??

![20220124222520](https://cdn.jsdelivr.net/gh/nzcv/picgo/20220124222520.png)

因为 IDA 是一种静态分析工具，它无法获知程序在执行时会在这个内存位置输入什么地址

导入窗口还提供与 objdump (-T) 、 readelf (-s) 和 dumpbin (/IMPORTS) 等命令行工具类似的功能

导入窗口仅显示二进制文件想要动态加载器自动处理的符号，二进制文件选择使用 dlopen/dlsym 或 LoadLibrary/GetProcAddress 等机制自行加载的符号将不会在导入窗口中显示


## 实线箭头表示非条件跳转，虚线箭头则表示条件跳转

![20220124224249](https://cdn.jsdelivr.net/gh/nzcv/picgo/20220124224249.png)

## IAD文本视图

![20220124224532](https://cdn.jsdelivr.net/gh/nzcv/picgo/20220124224532.png)


# HelpLink

[详细解释每个窗体的含义](https://www.cnblogs.com/sch01ar/p/9476340.html)

