![20211206175821](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211206175821.png)

# if/switch汇编实现原理

研究if/switch语句必然少不了对应的条件判断, 以及对应的条件跳转.

# 再次熟悉一下CMP指令

在Arm64位下, 硬件并没有实现对应的CMP指令, Compare, like SUBS but without destination register. 

![20211206171643](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211206171643.png)

CMP指令被分为了3个大类:

![20211206143310](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211206143310.png)

# 再次熟悉一下NZCV标志位

![20211206144733](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211206144733.png)

请注意:

    ALU subtraction no borrow为1, borrow为0
      
    ALU addition Carry out为1, no carried out为0

# 熟悉CSET(C is Conditional not Carry)

![cset2](https://cdn.jsdelivr.net/gh/nzcv/picgo/cset2.png)


# 熟悉TBNZ(Test bit and Branch if Nonzero)

![tbnz](https://cdn.jsdelivr.net/gh/nzcv/picgo/tbnz.png)

# 示例程序

```c
int test12(int n) {    
    int ret = 0xa0;
    if (n == 1) {
        ret = 0xa1;
    } else if (n == 2) {
        ret = 0xa2;
    } else if (n == 3) {
        ret = 0xa3;
    } else if (n == 4) {
        ret = 0xa4;
    } else if (n == 5) {
        ret = 0xa5;
    }
    return ret;
}

int test13(int n) {
    int ret = 0xa0;
    switch (n) {
        case 1:
            ret = 0xa1;
            break;
        case 2:
            ret = 0xa2;
            break;
        case 3:
            ret = 0xa3;
            break;
        case 4:
            ret = 0xa4;
            break;
        case 5:
            ret = 0xa5;
            break;
        default:
            ret = 0xa6;
            break;
    }
    return ret;
}
```

有了上面的基础,我们就能够理解下面的核心代码

```asm
.text:0000000000000968                 SUB             SP, SP, #0x10
.text:000000000000096C                 MOV             W8, #1                   ;w8 临时变量
.text:0000000000000970                 STR             W0, [SP,#0x10+var_8]     ;var8 = w0
.text:0000000000000974                 LDR             W0, [SP,#0x10+var_8]     ;
.text:0000000000000978                 CMP             W0, W8                   ;NZCV0010
.text:000000000000097C                 CSET            W8, EQ                   ;W8 = Z = 0
.text:0000000000000980                 TBNZ            W8, #0, loc_988          ;NZ
.text:0000000000000984                 B               loc_994
.text:0000000000000988 ; ---------------------------------------------------------------------------
.text:0000000000000988
.text:0000000000000988 loc_988                                 ; CODE XREF: test12+18↑j
.text:0000000000000988                 MOV             W8, #0xA1
.text:000000000000098C                 STR             W8, [SP,#0x10+var_4]
.text:0000000000000990                 B               loc_A2C
```
![20211206225053](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211206225053.png)


# 关于Switch

1. 假设 switch 内分支比较多的时候，在编译的时候会生成一个表（跳转表每个地址四个字节）；

2. 假设 switch 内分支比较少的时候（例如3，少于4的时候没有意义），就没有必要使用此结构，相当于if...else语句的结构；

3. 假设 switch 内各个分支常量的差值较大的时候，编译器会在效率还是内存之间进行取舍，如果效率优先的情况下，这个时候编译器还是会编译成类似于if..else的结构。


```shell
注意: 这里只在godbolt进行了复现, 在ndk-23上面并没有得到相同结果
```

[https://godbolt.org/z/M3YTjxMr9](https://godbolt.org/z/M3YTjxMr9)

[https://www.jianshu.com/p/7e1eb1d72c83](https://www.jianshu.com/p/7e1eb1d72c83)
