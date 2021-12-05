

![20211205233422](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211205233422.png)


# 一条奇怪的指令

前段时间在做Arm64结构体传参时，看到一个奇怪的指令MRS X23, #3, c13, c0, #2，这其实是AArch64状态下对于Canary机制的实现。本篇介绍Canary机制与其中的一些特征。

# 什么是Canary机制

Canary 的意思是金丝雀，来源于英国矿井工人用来探查井下气体是否有毒的金丝雀笼子。工人们每次下井都会带上一只金丝雀。如果井下的气体有毒，金丝雀由于对毒性敏感就会停止鸣叫甚至死亡，从而使工人们得到预警。

Canary机制是一种栈保护机制，**在函数开始执行的时候，会先在栈底插入cookie信息，在函数真正返回的时候，会验证cookie的值是否合法（栈帧销毁前测试该值是否被改变)，如果不合法就停止程序运行 (防止栈溢出发生)。这个cookie信息也被称为Canary**。由于Canary机制的实现简单高效，它被普遍用于Linux系统上预防栈溢出的第一道防线。而Android基于Linux微内核，因而也采用这一机制对栈溢出进行检验。本篇主要介绍Canary机制的特征码，其它知识点不再展开。


# AArch64状态下的Canary机制

因为我是在Arm64上面遇到了对应的问题， Arm32应该也有类似的机制，这里并不讨论，后期遇到了再进行添加。 

![20211205234657](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211205234657.png)

我通过./ghidra和ida进行查看，发现他们翻译成了不同的指令，显然ghida给我了我们搜索到线索，X8, #3, c13, c0, #2代表的意思是“**以读写权限访问非调试系统寄存器TPIDR_EL0，并将其中的值保存到X8寄存器中**”


# 什么是TPIDR_EL0

这回，弄明白了上面的指令的作用后，又有了新的问题，这个TPIDR_EL0寄存器是干什么的？为什么要取它保存的值？文章中对TPIDR_EL0的描述是，软件运行在EL0级别下，可被读写的线程ID寄存器。换句话说，软件在EL0运行级别下，这个寄存器可以存储线程身份信息以便于管理，其作用相当于TLS（Thread Local Storage）。

![20211205235004](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211205235004.png)

# 相关示例程序以及对应的流程信息

下面的一个示例程序会导致堆栈溢出，引发堆栈保护， 详细的跟踪记录，见最上面的事例图便可以了解对应的工作原理了。

```c
int test11() {
    int a = 0xa1;
    int b = 0xa2;
    //tigger stack overflow
    int *c = &a;
    c[0] = 0xb0;
    c[1] = 0xb1;
    c[2] = 0xb2;
    c[3] = 0xb3;
    c[4] = 0xb4;
    c[5] = 0xb5;
    c[6] = 0xb6;
    c[7] = 0xb7;
    return a;
}
```


```asm
hello:0000005FA6FDFB54 test11
hello:0000005FA6FDFB54
hello:0000005FA6FDFB54 var_90= -0x90
hello:0000005FA6FDFB54 var_88= -0x88
hello:0000005FA6FDFB54 var_80= -0x80
hello:0000005FA6FDFB54 var_78= -0x78
hello:0000005FA6FDFB54 var_70= -0x70
hello:0000005FA6FDFB54 var_68= -0x68
hello:0000005FA6FDFB54 var_60= -0x60
hello:0000005FA6FDFB54 var_30= -0x30
hello:0000005FA6FDFB54 var_28= -0x28
hello:0000005FA6FDFB54 var_1C= -0x1C
hello:0000005FA6FDFB54 var_18= -0x18
hello:0000005FA6FDFB54 var_10= -0x10
hello:0000005FA6FDFB54 var_8= -8
hello:0000005FA6FDFB54 var_s0=  0
hello:0000005FA6FDFB54
hello:0000005FA6FDFB54 SUB             SP, SP, #0x30           ; CODE XREF: test11+1B0↓p
hello:0000005FA6FDFB58 STP             X29, X30, [SP,#0x20+var_s0]
hello:0000005FA6FDFB5C ADD             X29, SP, #0x20
hello:0000005FA6FDFB60 MRS             X8, #3, c13, c0, #2
hello:0000005FA6FDFB64 LDR             X9, [X8,#0x28]
hello:0000005FA6FDFB68 STUR            X9, [X29,#var_8]
hello:0000005FA6FDFB6C MOV             W10, #0xA1
hello:0000005FA6FDFB70 STUR            W10, [X29,#var_10+4]
hello:0000005FA6FDFB74 MOV             W10, #0xA2
hello:0000005FA6FDFB78 STR             W10, [SP,#0x20+var_10]
hello:0000005FA6FDFB7C SUB             X9, X29, #-(var_10+4)
hello:0000005FA6FDFB80 STR             X9, [SP,#0x20+var_18]
hello:0000005FA6FDFB84 LDR             X9, [SP,#0x20+var_18]
hello:0000005FA6FDFB88 MOV             W10, #0xB0
hello:0000005FA6FDFB8C STR             W10, [X9]
hello:0000005FA6FDFB90 LDR             X9, [SP,#0x20+var_18]
hello:0000005FA6FDFB94 MOV             W10, #0xB1
hello:0000005FA6FDFB98 STR             W10, [X9,#4]
hello:0000005FA6FDFB9C LDR             X9, [SP,#0x20+var_18]
hello:0000005FA6FDFBA0 MOV             W10, #0xB2
hello:0000005FA6FDFBA4 STR             W10, [X9,#8]
hello:0000005FA6FDFBA8 LDR             X9, [SP,#0x20+var_18]
hello:0000005FA6FDFBAC MOV             W10, #0xB3
hello:0000005FA6FDFBB0 STR             W10, [X9,#0xC]
hello:0000005FA6FDFBB4 LDR             X9, [SP,#0x20+var_18]
hello:0000005FA6FDFBB8 MOV             W10, #0xB4
hello:0000005FA6FDFBBC STR             W10, [X9,#0x10]
hello:0000005FA6FDFBC0 LDR             X9, [SP,#0x20+var_18]
hello:0000005FA6FDFBC4 MOV             W10, #0xB5
hello:0000005FA6FDFBC8 STR             W10, [X9,#0x14]
hello:0000005FA6FDFBCC LDR             X9, [SP,#0x20+var_18]
hello:0000005FA6FDFBD0 MOV             W10, #0xB6
hello:0000005FA6FDFBD4 STR             W10, [X9,#0x18]
hello:0000005FA6FDFBD8 LDR             X9, [SP,#0x20+var_18]
hello:0000005FA6FDFBDC MOV             W10, #0xB7
hello:0000005FA6FDFBE0 STR             W10, [X9,#0x1C]
hello:0000005FA6FDFBE4 LDUR            W10, [X29,#var_10+4]
hello:0000005FA6FDFBE8 LDR             X8, [X8,#0x28]
hello:0000005FA6FDFBEC LDUR            X9, [X29,#var_8]
hello:0000005FA6FDFBF0 SUBS            X8, X8, X9
hello:0000005FA6FDFBF4 STR             W10, [SP,#0x20+var_1C]
hello:0000005FA6FDFBF8 B.NE            loc_5FA6FDFC10
hello:0000005FA6FDFBFC B               loc_5FA6FDFC00
hello:0000005FA6FDFC00 ; ---------------------------------------------------------------------------
hello:0000005FA6FDFC00
hello:0000005FA6FDFC00 loc_5FA6FDFC00                          ; CODE XREF: test11+A8↑j
hello:0000005FA6FDFC00 LDR             W0, [SP,#0x20+var_1C]
hello:0000005FA6FDFC04 LDP             X29, X30, [SP,#0x20+var_s0]
hello:0000005FA6FDFC08 ADD             SP, SP, #0x30
hello:0000005FA6FDFC0C RET
hello:0000005FA6FDFC10 ; ---------------------------------------------------------------------------
hello:0000005FA6FDFC10
hello:0000005FA6FDFC10 loc_5FA6FDFC10                          ; CODE XREF: test11+A4↑j
hello:0000005FA6FDFC10 BL              unk_5FA6FDF6E0
```


```asm
0000007FE6B805D0  00000005000000B3  
0000007FE6B805D8  000000A2000000A1  
0000007FE6B805E0  00000000000000A3  
0000007FE6B805E8  95E90E95548E982B  
0000007FE6B805F0  0000007FE6B80660  [stack]:0000007FE6B80660
0000007FE6B805F8  0000005CB3ACAD08  test11+1B4
0000007FE6B80600  0000005C00000009  
```

```asm
0000007FE6B805D0  00000005000000B3  
0000007FE6B805D8  0000007FE6B805E4  [stack]:0000007FE6B805E4  == &a
0000007FE6B805E0  000000A1000000A2  
0000007FE6B805E8  95E90E95548E982B  
0000007FE6B805F0  0000007FE6B80660  [stack]:0000007FE6B80660
0000007FE6B805F8  0000005CB3ACAD08  test11+1B4
0000007FE6B80600  0000005C00000009  
```


# help link

[https://blog.iret.xyz/article.aspx/thread_pointer_aarch64](https://blog.iret.xyz/article.aspx/thread_pointer_aarch64)