# 符号和函数存在多对一的关系

![20211229150451](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211229150451.png)

# 符号表

本质: 地址和字符串的对应关系

```c
/* Symbol table entry.  */
typedef struct
{
  Elf32_Word	st_name;		/* Symbol name (string tbl index) */
  Elf32_Addr	st_value;		/* Symbol value */
  Elf32_Word	st_size;		/* Symbol size */
  unsigned char	st_info;		/* Symbol type and binding */
  unsigned char	st_other;		/* Symbol visibility */
  Elf32_Section	st_shndx;		/* Section index */
} Elf32_Sym;
```

![20211227081625](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211227081625.png)

st_info(符号绑定信息)，低4位表示符号类型（Symbol Type）,高28位表示符号绑定信息（Symbol Binding）

![20211227081859](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211227081859.png)

![20211227081913](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211227081913.png)

st_shndx(符号所在段) 如果符号是定义在目标文件中，那么符号的意思就是符号所在的段的在段表中的下标 ，此外还有几种特殊的定义如下：

![20211227081954](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211227081954.png)

st_value(符号值) 符号如果是一个函数或者变量，符号值表示函数或者变量的地址，特殊地st_value还有以下几种可能：

* 在目标文件中，符号类型不是COMMON类型（st_shndx!=SHN_COMMON），st_value表示符号在段中的偏移，即符号位于st_shndx所在的段偏移st_value，在该例子中的global_init_var、func1、main都属于这种类型。
* 在目标文件中，符号类型是COMMON类型（st_shndx=SHN_COMMON），st_value表示符号的对其属性，在该例子中的global_uninit_var的Value值为0000000000000004，表示对其长度为4字节，即为global_uninit_var类型（int）的字节长度
* 在可执行文件中，st_value表示符号的虚拟地址，动态链接需要使用到

```c
int a[1024];

int b[10] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

const int c[5] = {0xbb, 0xbc};
int d;
int e = 0x88;

// link with liblog.so
void say_hello(const  char* msg) {
    __android_log_print(ANDROID_LOG_DEBUG, "minidl", "%s", msg);
}

// link with libc.so
void printf_hello(const  char* msg, int i) {
    b[0] = c[i];
    printf("%s", msg);
}
```

```shell
[12] .text             PROGBITS        000007cc 0007cc 00190c 00  AX  0   0  4
[21] .data             PROGBITS        00004000 003000 000034 00  WA  0   0  4
[22] .bss              NOBITS          00004034 003034 001005 00  WA  0   0  4
```

```shell
 nchain=25:
[ LOCAL]0x00000000, size=00000000, STT_NOTYPE   STBLOCAL ndx=UNDEF name=
[IMPORT]0x00000000, size=00000000,   STT_FUNC STB_GLOBAL ndx=UNDEF name=__cxa_finalize
[IMPORT]0x00000000, size=00000000,   STT_FUNC STB_GLOBAL ndx=UNDEF name=__cxa_atexit
#say_hello定义在目标文件中，ndx值为12表示位于代码段.text中
[EXPORT]0x00000809, size=00000056,   STT_FUNC STB_GLOBAL ndx=   12 name=say_hello
[IMPORT]0x00000000, size=00000000,   STT_FUNC STB_GLOBAL ndx=UNDEF name=__android_log_print
#printf_hello定义在目标文件中，ndx值为12表示位于代码段.text中
[EXPORT]0x00000841, size=00000064,   STT_FUNC STB_GLOBAL ndx=   12 name=printf_hello
[IMPORT]0x00000000, size=00000000,   STT_FUNC STB_GLOBAL ndx=UNDEF name=printf
#b符号在.data区, 属于OBJECT类型
[EXPORT]0x00004004, size=00000040, STT_OBJECT STB_GLOBAL ndx=   21 name=b
#a符号在.bss区, 属于OBJECT类型
[EXPORT]0x00004034, size=00004096, STT_OBJECT STB_GLOBAL ndx=   22 name=a
#d符号在.bss区, 属于OBJECT类型
[EXPORT]0x00005034, size=00000004, STT_OBJECT STB_GLOBAL ndx=   22 name=d
#e符号在.data区, 属于OBJECT类型
[EXPORT]0x0000402c, size=00000004, STT_OBJECT STB_GLOBAL ndx=   21 name=e
[   ABS]0x00004034, size=00000000, STT_NOTYPE STB_GLOBAL ndx=  ABS name=_edata
[   ABS]0x00005039, size=00000000, STT_NOTYPE STB_GLOBAL ndx=  ABS name=_end
[   ABS]0x00004034, size=00000000, STT_NOTYPE STB_GLOBAL ndx=  ABS name=__bss_start
[IMPORT]0x00000000, size=00000000, STT_OBJECT STB_GLOBAL ndx=UNDEF name=__stack_chk_guard
[IMPORT]0x00000000, size=00000000,   STT_FUNC STB_GLOBAL ndx=UNDEF name=__stack_chk_fail
#printf符号在目标文件中没有定义，所以ndx值为UND
[IMPORT]0x00000000, size=00000000,   STT_FUNC STB_GLOBAL ndx=UNDEF name=fprintf
[IMPORT]0x00000000, size=00000000,   STT_FUNC STB_GLOBAL ndx=UNDEF name=fflush
[IMPORT]0x00000000, size=00000000,   STT_FUNC STB_GLOBAL ndx=UNDEF name=abort
[IMPORT]0x00000000, size=00000000, STT_OBJECT STB_GLOBAL ndx=UNDEF name=__sF
[IMPORT]0x00000000, size=00000000,   STT_FUNC STB_GLOBAL ndx=UNDEF name=__aeabi_memcpy
[IMPORT]0x00000000, size=00000000,   STT_FUNC STB_GLOBAL ndx=UNDEF name=__aeabi_memclr8
[IMPORT]0x00000000, size=00000000,   STT_FUNC STB_GLOBAL ndx=UNDEF name=snprintf
[IMPORT]0x00000000, size=00000000,   STT_FUNC STB_GLOBAL ndx=UNDEF name=dladdr
[IMPORT]0x00000000, size=00000000,   STT_FUNC STB_GLOBAL ndx=UNDEF name=__gnu_Unwind_Find_exidx
```
     STB_GLOBAL/STBLOCAL
     STB_GLOBAL，表示符号绑定信息为全局符号，外部可见
     STBLOCAL  , 局部符号, 对于目标文件外部, 不可见


## 使用 readelf -s 命令查看目标文件的所有符号详细信息
```shell
λ readelf20.bat -s built\armeabi-v7a\libdemo.so

Symbol table '.dynsym' contains 25 entries:
   Num:    Value  Size Type    Bind   Vis      Ndx Name
     0: 00000000     0 NOTYPE  LOCAL  DEFAULT  UND
     1: 00000000     0 FUNC    GLOBAL DEFAULT  UND __cxa_finalize@LIBC (2)
     2: 00000000     0 FUNC    GLOBAL DEFAULT  UND __cxa_atexit@LIBC (2)
     3: 00000809    56 FUNC    GLOBAL DEFAULT   12 say_hello
     4: 00000000     0 FUNC    GLOBAL DEFAULT  UND __android_log_print
     5: 00000841    64 FUNC    GLOBAL DEFAULT   12 printf_hello
     6: 00000000     0 FUNC    GLOBAL DEFAULT  UND printf@LIBC (2)
     7: 00004004    40 OBJECT  GLOBAL DEFAULT   21 b
     8: 00004034  4096 OBJECT  GLOBAL DEFAULT   22 a
     9: 00005034     4 OBJECT  GLOBAL DEFAULT   22 d
    10: 0000402c     4 OBJECT  GLOBAL DEFAULT   21 e
    11: 00004034     0 NOTYPE  GLOBAL DEFAULT  ABS _edata
    12: 00005039     0 NOTYPE  GLOBAL DEFAULT  ABS _end
    13: 00004034     0 NOTYPE  GLOBAL DEFAULT  ABS __bss_start
    14: 00000000     0 OBJECT  GLOBAL DEFAULT  UND __stack_chk_guard@LIBC (2)
    15: 00000000     0 FUNC    GLOBAL DEFAULT  UND __stack_chk_fail@LIBC (2)
    16: 00000000     0 FUNC    GLOBAL DEFAULT  UND fprintf@LIBC (2)
    17: 00000000     0 FUNC    GLOBAL DEFAULT  UND fflush@LIBC (2)
    18: 00000000     0 FUNC    GLOBAL DEFAULT  UND abort@LIBC (2)
    19: 00000000     0 OBJECT  GLOBAL DEFAULT  UND __sF@LIBC (2)
    20: 00000000     0 FUNC    GLOBAL DEFAULT  UND __aeabi_memcpy
    21: 00000000     0 FUNC    GLOBAL DEFAULT  UND __aeabi_memclr8
    22: 00000000     0 FUNC    GLOBAL DEFAULT  UND snprintf@LIBC (2)
    23: 00000000     0 FUNC    GLOBAL DEFAULT  UND dladdr@LIBC (3)
    24: 00000000     0 FUNC    GLOBAL DEFAULT  UND __gnu_Unwind_Find_exidx
```

## 使用 readelf -S 查看目标文件的所有段信息，后面分析符号信息的时候会作为参照使用到。
```shell
root@localhost simple-secion-linux-elf]# readelf -S libdemo.o
[12] .text             PROGBITS        000007cc 0007cc 00190c 00  AX  0   0  4
[21] .data             PROGBITS        00004000 003000 000034 00  WA  0   0  4
[22] .bss              NOBITS          00004034 003034 001005 00  WA  0   0  4
```

几个重要的符号解释：

```shell
1. printf_hello 和 say_hello定义在目标文件中，Ndx值为12表示位于代码段中（参照 readelf -S 结果中的 [Nr] 值 ）
     Bind值为GLOBAL，表示符号绑定信息为全局符号，外部可见
     Type值为FUNC，  表示符号类四个函数或其他可执行代码
     Size值表示符号大小
     Value符号所在的地址
```

```shell
printf符号在目标文件中没有定义，所以Ndx值为UND
```

```shell
(1)a符号是未初始化的全局变量，Ndx值为22表示位于.bss数据段，
     Bind值为OBJECT，表示符号类型为STT_OBJECT，变量数组的符号类型都为该值
     Size  为4096，  表示符号大小占用4096字节
     Value 为22，    表示该符号位于Ndx段的第22个位置

```

```shell
(1)有部分Name未定义的符号，符号名就是段名，比如Num为2、Ndx为1的的符号表示.text段，他的符号名就是段名
```

```shell
SimpleSection.c符号的Ndx为ABS、Type为FILE，表示文件名的符号
     DT_JMPREL: 重定位记录的开始地址, 指向.rela.plt节在内存中保存的地址
     DT_PLTREL: 重定位记录的类型 RELA或RE, 这里是RELAL
     DT_PLTRELSZ: 重定位记录的总大小, 这里是24 * 2 = 48
```

# 导入表和导出表都需要依赖符号表

![20211229193137](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211229193137.png)


# 讲解导入导出之前, 我们需要了解下重定位

```c
typedef struct elf32_rel {
  Elf32_Addr    r_offset; //r_offset表示重定位入口的偏移。对于可重定位文件来说，这个值是该重定位入口所要修正的位置的第一个字节相对于节起始的偏移；对于可执行文件或共享对象文件来说，这个值是该重定位入口所要修正的位置的第一个字节的虚拟地址
  Elf32_Word    r_info; //r_info表示重定位入口的类型和符号。这个成员的高8位表示重定位入口的类型，低24位表示重定位入口的符号在符号表中的下标
} Elf32_Rel;
```

![20211229193555](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211229193555.png)

# 欸外知识

我定义了一个const数组, 但是符号表解析并没有找到对应的数据, 原因是对应数据进入rodata区

![20211229201456](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211229201456.png)

# 帮助

[https://blog.csdn.net/weixin_33895475/article/details/91930494](https://blog.csdn.net/weixin_33895475/article/details/91930494)

[https://malware.news/t/executable-and-linkable-format-101-part-2-symbols/17960](https://malware.news/t/executable-and-linkable-format-101-part-2-symbols/17960)

[https://www.freebuf.com/articles/system/193646.html](https://www.freebuf.com/articles/system/193646.html)


[https://lwn.net/Articles/192624/](https://lwn.net/Articles/192624/)