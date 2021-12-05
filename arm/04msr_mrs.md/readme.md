
# 程序状态寄存器访问指令


![20211125094433](https://cdn.jsdelivr.net/gh/yhnu/PicBed/20211125094433.png)

## 关于MRS

1. 用户模式读取状态寄存器T flag is awalys 0

对应原因:

    ·T标志位：该位反映处理器的运行状态。当该位为1时，程序运行于THUMB状态，否则运行于ARM状态。该信号反映在外部引脚TBIT上。在程序中不得修改CPSR中的TBIT位，否则处理器工作状态不能确定。

2. 特权模式CPSR的低8位（包括I、F、T和M[4：0]）称为控制位，当发生异常时这些位可以被改变。如果处理器运行于特权模式时，这些位也可以由程序修改。

```c
void test3() {
    asm volatile(
        "MRS r0, CPSR\n"
        // "MRS r15, CPSR\n"   // rx 不允许是r15
        // "MRS r0, SPSR\n"    // 用户模式会触发异常
        "MRS CPSR_n, #1\n"

        "MRS R0,CPSR\n"          // Read CPSR, T Flag is awalys 0

                                 // >>> CPSR bin(0x80070030)
                                 //     '0b10000000000001110000000000110000'
                                 // >>> R0 bin(0x80070010)           t
                                 //     '0b10000000000001110000000000010000'

        "BIC R0,R0,#0x1F\n"      // modify by removing current mode
                                 // >>> R0 bin(0x80070000)
                                 //     '0b10000000000001110000000000000000'
        
        "MOV R0,#0x20000000\n"   // and substituting Supervisor mode
        "MSR CPSR_f,R0\n"        // Write the result back to CPSR        
        "nop\n");
}
```

## 关于MSR

1. Fields

![20211125161358](https://cdn.jsdelivr.net/gh/yhnu/PicBed/20211125161358.png)

对应赋值对位域有影响.

2. Immediate

8bit: range 0x00-0xff

### 不同模式权限不一样

![20211125162615](https://cdn.jsdelivr.net/gh/yhnu/PicBed/20211125162615.png)

用户模式读取SPSR, 会造成异常

![20211124170010](https://cdn.jsdelivr.net/gh/yhnu/PicBed/20211124170010.png)