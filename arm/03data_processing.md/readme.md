
前面我们已经知道了寄存器对应的作用, 通过下面的例子我们可以学习到以下指令, 这里重点关注数据处理相关指令

1. 数据处理指令: SUB/MOVS/CMP/ADD

2. 跳转指令: B/BLE/BX

3. 存取指令: STR/LDR


相关示例:

```c
long test2() {
    int a = 1, b =2;
    if(a > b) {
        return a;
    } 
    return b;
}
```

```asm
.text:0000060C ; int test2()
.text:0000060C                 EXPORT test2
.text:0000060C test2                                   ; CODE XREF: main+16↓p
.text:0000060C
.text:0000060C var_C           = -0xC
.text:0000060C var_8           = -8
.text:0000060C var_4           = -4
.text:0000060C
.text:0000060C                 SUB             SP, SP, #0xC
.text:0000060E                 MOVS            R0, #1  ; r0 = 1
.text:00000610                 STR             R0, [SP,#0xC+var_8] ; var_8 = r0 = 1
.text:00000612                 MOVS            R0, #2  ; r0 = 2
.text:00000614                 STR             R0, [SP,#0xC+var_C] ; var_c = r0 = 2
.text:00000616                 LDR             R0, [SP,#0xC+var_8] ; r0 = var_8 = 1
.text:00000618                 LDR             R1, [SP,#0xC+var_C] ; r1 = var_c = 2
.text:0000061A                 CMP             R0, R1  ; r0 != r1
.text:0000061C                 BLE             loc_626 ; nop
.text:0000061E                 B               loc_620 ; r0 = var_8 = 1
.text:00000620 ; ---------------------------------------------------------------------------
.text:00000620
.text:00000620 loc_620                                 ; CODE XREF: test2+12↑j
.text:00000620                 LDR             R0, [SP,#0xC+var_8] ; r0 = var_8 = 1
.text:00000622                 STR             R0, [SP,#0xC+var_4] ; var_4 = r0 = 1
.text:00000624                 B               loc_62C ; r0 = var_4 = 2
.text:00000626 ; ---------------------------------------------------------------------------
.text:00000626
.text:00000626 loc_626                                 ; CODE XREF: test2+10↑j
.text:00000626                 LDR             R0, [SP,#0xC+var_C] ; r0 = var_c = 2
.text:00000628                 STR             R0, [SP,#0xC+var_4] ; var_4 = r0 = 2
.text:0000062A                 B               loc_62C ; r0 = var_4 = 2
.text:0000062C ; ---------------------------------------------------------------------------
.text:0000062C ; 2:   return 2;
.text:0000062C
.text:0000062C loc_62C                                 ; CODE XREF: test2+18↑j
.text:0000062C                                         ; test2+1E↑j
.text:0000062C                 LDR             R0, [SP,#0xC+var_4] ; r0 = var_4 = 2
.text:0000062E                 ADD             SP, SP, #0xC ; destory local var
.text:00000630                 BX              LR      ; return
.text:00000630 ; End of function test2
```

关于数据操作指令有16个, 其中比较常用的指令已经标红处理:


![20211124153203](https://cdn.jsdelivr.net/gh/yhnu/PicBed/20211124153203.png)

4. MOV指令

```asm
## 16位立即数

MOV #0x1234

## 可循环移位立即数

MOV #0x80000000

## 32位立即数

MOV #0x5678
MOVT #0x1234
```

5. 编译器如何访问立即数

