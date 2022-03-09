# 位运算


ARM的位运算是如此的重要,以至于对应的执行单元,其中一个操作数带有移位操作功能. 

![20211213113955](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211213113955.png)

ARM 微处理器内嵌的桶型移位器（Barrel Shifter），支持数据的各种移位操作，移位操作在 ARM 指令集中不作为单独的指令使用，它只能作为指令格式中是一个字段，在汇编语言中表示为指令中的选项。

## Barrel Shift包含5种运算

![20211213131806](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211213131806.png)


![20211213154747](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211213154747.png)

逻辑左移和算术左移计算结果是一模一样的.

```c
void test25() {
    asm volatile(
        "mov r1, #0xffffffff\n"
        "lsls r0, r1, #1\n"         //逻辑左移, 影响标志位
        
        "mov r1, #0xffffffff\n"
        "movs r0, r1, LSL#1\n"      //shift op 逻辑左移, 影响标志位

        "mov r1, #0xffffffff\n"
        //"asl r0, r1, #1\n"         //算术左移, 指令不存在

        "mov r1, #0xffffffff\n"
        "lsl r0, r1, #1\n");       //逻辑/算术左移动, 不影响标志位
}
```

```asm
hello:05694448 test25
hello:05694448 MOV.W           R1, #0xFFFFFFFF
hello:0569444C LSLS            R0, R1, #1
hello:0569444E MOV.W           R1, #0xFFFFFFFF
hello:05694452 LSLS            R0, R1, #1
hello:05694454 MOV.W           R1, #0xFFFFFFFF
hello:05694458 MOV.W           R1, #0xFFFFFFFF
hello:0569445C MOV.W           R0, R1,LSL#1
hello:05694460 BX              LR
hello:05694460 ; End of function test25
```


