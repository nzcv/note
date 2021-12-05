今天有小伙伴遇到了关于有符号数除的2的N次幂使用移位运算加速的理解问题,这里做一下对应记录.

# 遇到的问题

![20211129173426](https://cdn.jsdelivr.net/gh/yhnu/PicBed/20211129173426.png)

# 除法的实现

早期计算机并没有设计专门的除法电路, 也就没有专门的指令. 数学家们总是可以找到高效的除法计算方法

于是就有了借助移位运算进行高效除法计算的理论基础以及对应的实现.

![20211129172021](https://cdn.jsdelivr.net/gh/yhnu/PicBed/20211129172021.png)

## 无符号除2的幂示例代码

```c
unsigned int test5(unsigned int a1)
{
  // 无符号int除以2的倍数
  // LSRS            R1, R0, #12
  return a1 / 4096;
}
```

```asm
hello32:0D8AD4BE test5
hello32:0D8AD4BE
hello32:0D8AD4BE var_4= -4
hello32:0D8AD4BE
hello32:0D8AD4BE SUB             SP, SP, #4
hello32:0D8AD4C0 STR             R0, [SP,#4+var_4]       ; var_4 = r0 = 0x55
hello32:0D8AD4C2 LDR             R0, [SP,#4+var_4]       ; r0 = var_4 = 0x55
hello32:0D8AD4C4 LSRS            R0, R0, #0xC            ; lsrs is thumb provides the unsigned value of a register divided by a variable power of two. r0 = 0x55 >> 0xc = 0
hello32:0D8AD4C6 ADD             SP, SP, #4
hello32:0D8AD4C8 BX              LR
hello32:0D8AD4C8 ; End of function test5
```

### 重要研究LSR指令


![20211129100432](https://cdn.jsdelivr.net/gh/yhnu/PicBed/20211129100432.png)

    实例中的lsrs采用的是上面这种情况. r0 = r0(0x55) >> 0xc = 0

![20211129100451](https://cdn.jsdelivr.net/gh/yhnu/PicBed/20211129100451.png)

### ARM64位对应指令UDIV

```asm
; arm64 code
.text:00000000000007A4 ; unsigned int __cdecl test5(unsigned int a1)
.text:00000000000007A4                 EXPORT test5
.text:00000000000007A4 test5                                   ; CODE XREF: main+54↓p
.text:00000000000007A4
.text:00000000000007A4 var_4           = -4
.text:00000000000007A4 a1              =  0xC
.text:00000000000007A4
.text:00000000000007A4 ; __unwind {
.text:00000000000007A4                 SUB             SP, SP, #0x10
.text:00000000000007A8                 MOV             W8, #0x1000 ; w8 is tmp var
.text:00000000000007AC                 STR             W0, [SP,#0x10+var_4] ; var_4 = 0x55
.text:00000000000007B0                 LDR             W0, [SP,#0x10+var_4] ; w8 = var_4 = 0x55
.text:00000000000007B4                 UDIV            W0, W0, W8 ; Unsigned Divide; https://developer.arm.com/documentation/dui0801/g/A32-and-T32-Instructions/UDIV
.text:00000000000007B8                 ADD             SP, SP, #0x10
.text:00000000000007BC                 RET
.text:00000000000007BC ; } // starts at 7A4
.text:00000000000007BC ; End of function test5
```


## 有符号除2的N次幂示例代码2
```c
int test6(signed int a1)
{
  // 有符号int除以2的倍数
  // ASRS            R1, R0, #31       //R0为要操作的数，如果R0为正，R1恒为0；如果R0为负，R1恒为-1；
  // ADD.W           R0, R0, R1,LSR#20 //R0为要操作的数，R0+=R1逻辑右移一定位数，具体位数等于32-幂
  // ASRS            R0, R0, #12       //R0为要操作的数，R0算数右移一定位数，具体位数等于幂
  return a1 / 4096;
}
```
```asm
hello32:01DCD4BE test6
hello32:01DCD4BE
hello32:01DCD4BE var_4= -4
hello32:01DCD4BE
hello32:01DCD4BE SUB             SP, SP, #4
hello32:01DCD4C0 STR             R0, [SP,#4+var_4]       ; var_4 = r0 = 0x55
hello32:01DCD4C2 LDR             R0, [SP,#4+var_4]       ; r0 = var_4 = 0x55
hello32:01DCD4C4 ASRS            R1, R0, #0x1F           ; r1 = r0(0x55) >> 0x1f = 0 ; 提取符号位
hello32:01DCD4C6 ADD.W           R0, R0, R1,LSR#20       ; r0 = r0 + r1 >> 20; r0 = 0x55
hello32:01DCD4CA ASRS            R0, R0, #0xC            ; r0 = r0 >> 0xc = 0
hello32:01DCD4CC ADD             SP, SP, #4
hello32:01DCD4CE BX              LR
hello32:01DCD4CE ; End of function test6
```

![20211129103915](https://cdn.jsdelivr.net/gh/yhnu/PicBed/20211129103915.png)

    示例程序使用上面的指令, 采用算术右移进行除法运算

![20211129104019](https://cdn.jsdelivr.net/gh/yhnu/PicBed/20211129104019.png)

    Q: 为什么会有R1 = R0 >> 0x1f(31)呢?
    A: usigned 32 is 32bit, the 32bit is flag. 

### 背后的原理

通过脑图我们已经知道有符号除2^N运算的公式是, 至于为什么数学家已经论证了见help link

    (x + (1<<N) - 1) >> N


    示例代码中N = 12 (0xc)


    ASRS R1, R0, #0x1F;

    ADD.W R0, R0, R1, LSR#20; 1 << N - 1 等价于 -1 >> (32 - N)

                            ; 1 << 12 -1 等价于 -1 >> 20


下面是两种指令得到的结果可以看到r0和r1是相等, 至于为什么使用 -1 >> (32-N)呢? 我们看一下指令的条数就知道了, 编译器并不想浪费任何一条指令.

![20211129171348](https://cdn.jsdelivr.net/gh/yhnu/PicBed/20211129171348.png)

![20211129171520](https://cdn.jsdelivr.net/gh/yhnu/PicBed/20211129171520.png)


## help link

参考书籍 <<深入理解计算机系统>>




