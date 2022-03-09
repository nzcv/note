

前段时间通篇概览一遍[汇编语言程序设计——基于ARM体系结构(第4版)](https://item.jd.com/10027570912314.html), 总感觉纸上得来终觉浅, 并不能够让我产生一种读汇编就跟读C代码一样那种流畅的感觉. 如果我们越熟悉, 越发觉得他们是有规律可循的, 这里做一下对应的记录, 互相共勉.

## 通用寄存器并不通用

表面上arm为我们提供了31个通用寄存器, 但其实他们并不通用, 或者说编译在编译代码时, 并不是随意的给寄存器赋值的, 他们都被划分为了不同的用途和含义, 我们需要记住这些用途,看汇编看起来就不会无从着手了.

ARM64提供了31个通用寄存器，其用途如下表：

x0~x7：传递子程序的参数和返回值，使用时不需要保存，多余的参数用堆栈传递，64位的返回结果保存在x0中。

**x8：用于保存子程序的返回地址，使用时不需要保存。**

x9~x15：临时寄存器，也叫可变寄存器，子程序使用时不需要保存。

x16~x17：子程序内部调用寄存器（IPx），使用时不需要保存，尽量不要使用。

x18：平台寄存器，它的使用与平台相关，尽量不要使用。

x19~x28：临时寄存器，子程序使用时必须保存。

x29：帧指针寄存器（FP），用于连接栈帧，使用时必须保存。

x30：链接寄存器（LR），用于保存子程序的返回地址。

x31：堆栈指针寄存器（SP），用于指向每个函数的栈顶。

![20211123091457](https://cdn.jsdelivr.net/gh/yhnu/PicBed/20211123091457.png)

![20211122173258](https://cdn.jsdelivr.net/gh/yhnu/PicBed/20211122173258.png)


### 比较重要的寄存器 FP, LR, SP, PC, PSR

![20211122145005](https://cdn.jsdelivr.net/gh/yhnu/PicBed/20211122145005.png)

1. 通过PC寄存器状态信息可以了解到目前PC寄存器在等待SVC调用, 处于read函数+4的偏移位置

2. 通过LR寄存器了解到上一层调用来自__sread, 调用链为__sread()->read()

3. PSR寄存器有好几种模式, 目前显示的是用户模式下面的CPSR寄存器的状态信息, 不同的位,具有不同的含义, 图片内已经做了详细的注释信息

### 次重要寄存器

```asm
hello:00000056C73836C4 SUB             SP, SP, #0x10           ; CODE XREF: main:loc_56C7383710↓p
hello:00000056C73836C8 MOV             W8, #1                  ; w8 = 1         ;
hello:00000056C73836CC MOV             W9, #2                  ; w9 = 2         ;
hello:00000056C73836D0 STR             W8, [SP,#0x10+var_4]    ; var_4 = w8 = 1 ;临时变量初始化为1
hello:00000056C73836D4 STR             W9, [SP,#0x10+var_8]    ; var_8 = w9 = 2 ;临时变量初始化2
hello:00000056C73836D8 STR             WZR, [SP,#0x10+var_C]   ; var_c = 0      ;临时变量初始化0     使用wzr寄存器
hello:00000056C73836DC LDR             W8, [SP,#0x10+var_4]    ; w8 = var_4 = 1 ;临时变量到寄存器w8
hello:00000056C73836E0 LDR             W9, [SP,#0x10+var_8]    ; w9 = var_8 = 2 ;临时变量到寄存器w9
hello:00000056C73836E4 ADD             W8, W8, W9              ; w8 = w8 + w9 = 3;执行加法
hello:00000056C73836E8 STR             W8, [SP,#0x10+var_C]    ; var_c = w8 = 3  ;存储结果
hello:00000056C73836EC LDR             W0, [SP,#0x10+var_C]    ; w0 = var_c = 3  ;返回结果 因为返回值是int, 因此使用w. w0便存储了对应的返回值
hello:00000056C73836F0 ADD             SP, SP, #0x10           ; ;pop stack      ;回收临时变量
hello:00000056C73836F4 RET
```

1. X0寄存器: 保存返回值

2. X8, X9寄存器: tmp

3. WZR/XZR寄存器: zero寄存器, 一般用来进行零时变量初始化

### PC寄存器

因为PC寄存器比较重要, 因此单独列出来

[https://github.com/yhnu/note/tree/master/arm/02register_pc.md](https://github.com/yhnu/note/tree/master/arm/02register_pc.md)

### 指令的设计

1. 没有隐式内存操作指令

所有的内存操作都有对应的指令(LDR/STR)

2. 不同的指令都有对应的操作数{内存/立即数/寄存器}(对应数量1-3个)

(1) 内存操作数和立即数操作数不能同时存在

(2) 内存操作数至多出现一次

(3) 寄存器操作数总在最前面

### 常用的指令

1. SUB 

将某一寄存器的值和另一寄存器的值 相减 并将结果保存在另一寄存器中

```asm
SUB             x0, x1, x2   ; x0 = x1 - x2
SUB             SP, SP, #0x30; 更新栈顶寄存器的值,(可以看出:申请 0x30 字节占空间为临时变量)
```

2. STP(stp：store pair)

入栈指令（str 的变种指令，可以同时操作两个寄存器）, var_xxx(xxx代表偏移,一般为负数)

```asm
STP             X29, X30, [SP,#0x20+var_s0] ; 入栈指令（str 的变种指令，可以同时操作两个寄存器）, 保存X29, X30, X29为FP寄存器, X30为LR寄存器
```

3. ADD

将某一寄存器的值和另一寄存器的值 相加 并将结果保存在另一寄存器中

```asm
add x0, x0, #1    ; x0 = x0 + 1
add x0, x1, x2    ; x0 = x1 + x2
add x0, x1, [x2]  ; x0 = x1 + mem(x2)

ADD             X29, SP, #0x20; X29 = SP + 0x20, 前面已经保存了寄存器,这里开始使用寄存器
```

4. ADR

    这是一条小范围的地址读取指令，它将基于PC的相对偏移的地址读到目标寄存器中；
    使用格式：ADR register exper

5. ADRP

用来定位数据段中的数据用, 因为 aslr 会导致代码及数据的地址随机化, 用 adrp 来根据 pc 做辅助定位

```asm
ADRP            X8, #aGoing@PAGE        ; "going ...... \n"
ADD             X8, X8, #aGoing@PAGEOFF ; "going ...... \n"
```

* 得到一个大小为4KB的页的基址，而且在该页中有全局变量aGoing的地址；ADRP就是讲该页的基址存到寄存器X8中；
* ADD指令会算出g的地址，X8+#aGoing@PAGEOFF，#aGoing@PAGEOFF是一个偏移量；这样就得到了g的地址X8;

![20211122213837](https://cdn.jsdelivr.net/gh/yhnu/PicBed/20211122213837.png)

对应原理：

* 编译时，首先会计算出当前PC到exper的偏移量#offset_to_exper
* pc的低12位清零，然后加上偏移量，给register, 得到的地址，是含有label的4KB对齐内存区域的base地址；

6. STUR/STR

ST开头的为存数据,比如说STR、STP、STUR

U: 表示负数
P: PAIR

```asm
STUR            WZR, [X29,#var_4]       ; var_4 = 0
STUR            W0, [X29,#var_8]        ; var_8 = w0; w0 is argc
STR             X1, [SP,#0x20+var_10]   ; var_10 = x1; x1 is argv
STR             X8, [SP,#0x20+var_18]   ; var_18 = x8;
```

7. LDR

```asm
LDR             X0, [SP,#0x20+var_18]   ; x0 = var_18 = x8 = "going" is printf arg0
```

8. BL/B/BR

BL: Branch with Link            (−16MB to +16MB)

B : Branch                      (−2KB to +2KB)

BR: Branch to register          (Any value in register)

[B指令ARM官方文档](https://developer.arm.com/documentation/dui0497/a/the-cortex-m0-instruction-set/branch-and-control-instructions/b--bl--bx--and-blx?lang=en)

```asm
BL              unk_639712B640          ; jump to printf with save lr
BL              unk_639712B650          ; jump to getchar with save lr
B               loc_639712B738          ; while(1)
BR              X17                     ; printf
```
### hello 添加对应注释

![20211122190626](https://cdn.jsdelivr.net/gh/yhnu/PicBed/20211122190626.png)

### PLT 调用特征

```asm
loc_639712B640                            ; CODE XREF: main+28↓p
ADRP            X16, #off_639713BFB0@PAGE ; X16 = PAGE(0x110000)
LDR             X17, [X16,#off_639713BFB0@PAGEOFF] ;  X17 = mem(X16 + offset) ; offset=00112000
ADD             X16, X16, #off_639713BFB0@PAGEOFF  ; X16 = X16 + offset
BR              X17                                ; printf
```

![20211122193628](https://cdn.jsdelivr.net/gh/yhnu/PicBed/20211122193628.png)

通过printf的plt调用过程,他们使用的策略都是一样的:

* 使用x16, x17寄存器
* 首先使用ADRP找到页基址, 然后加载对应页偏移, BR进行寄存器跳转

## help link

[https://github.com/yhnu/note/tree/master/arm/01register](https://github.com/yhnu/note/tree/master/arm/01register)

[https://github.com/yhnu/op7t](https://github.com/yhnu/op7t)

[https://github.com/yhnu/note/files/7585646/DDI01001.pdf](https://github.com/yhnu/note/files/7585646/DDI01001.pdf)