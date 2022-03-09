
![20211205185052](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211205185052.png)

参数传递的本质: 将上层函数变量复制一份, 传给下层函数.

# 过程调用标准AAPC(Arm Architecture Procedure Call)

![20220118084646](https://cdn.jsdelivr.net/gh/nzcv/picgo/20220118084646.png)
有了标准, 才能够进行C调用汇编或者汇编调用C


# ARM32调用约定

![20220118084824](https://cdn.jsdelivr.net/gh/nzcv/picgo/20220118084824.png)

参数是不同位数传参情况, 额外的参数被caller进行入栈, callee进行出栈

## 寄存器传参

![20220118085114](https://cdn.jsdelivr.net/gh/nzcv/picgo/20220118085114.png)

## 寄存器返回

![20220118085234](https://cdn.jsdelivr.net/gh/nzcv/picgo/20220118085234.png)

## 汇编调用C程序/C调用汇编程序

![20220118085619](https://cdn.jsdelivr.net/gh/nzcv/picgo/20220118085619.png)

## 32位实现64位加法运算

![20220118085751](https://cdn.jsdelivr.net/gh/nzcv/picgo/20220118085751.png)

## Arm的栈结构

![20220118085916](https://cdn.jsdelivr.net/gh/nzcv/picgo/20220118085916.png)

## 堆栈传参数与栈平衡

![20220118090516](https://cdn.jsdelivr.net/gh/nzcv/picgo/20220118090516.png)

## 子程序需要保存的寄存器

![20220118091115](https://cdn.jsdelivr.net/gh/nzcv/picgo/20220118091115.png)

![20220118091308](https://cdn.jsdelivr.net/gh/nzcv/picgo/20220118091308.png)

## 子程序返回PUSH {LR}/ POP {PC}

![20220118091739](https://cdn.jsdelivr.net/gh/nzcv/picgo/20220118091739.png)

## 结构体传参

```c
void my_argIsStruct(TPoint3D a1, int a2, int a3) {    
    //结构体作为参数，会把其字段按顺序依次放入寄存器或者栈，相当于把结构体拆解为多个参数，按照ARM传递参数规则来传递
    a1.dx_0 = 0;
    a1.dy_4 = 4;
    a1.dz_8 = 8;
    printf("结构体参数，其字段1：%d", a1.dx_0);
    printf("结构体参数，其字段2：%d", a1.dy_4);
    printf("结构体参数，其字段3：%d", a1.dz_8);
    printf("结构体参数之后的第2个int参数：%d", a2);
    printf("结构体参数之后的第3个int参数：%d", a3);
}
```

    结构体作为参数，会把其字段按顺序依次放入寄存器或者栈，相当于把结构体拆解为多个参数，按照ARM传递参数规则来传递

```asm
hello32:0286B5BA STR             R2, [SP,#0x18+var_C]    ; varc = r2
hello32:0286B5BC STR             R1, [SP,#0x18+var_10]   ; var10 = r1
hello32:0286B5BE STR             R0, [SP,#0x18+var_14]   ; var14 = r0
hello32:0286B5C0 STR             R3, [SP,#0x18+var_18]   ; var18 = r3
hello32:0286B5C2 MOVS            R0, #0
hello32:0286B5C4 STR             R0, [SP,#0x18+var_14]   ; var14 = 0
hello32:0286B5C6 MOVS            R0, #4
hello32:0286B5C8 STR             R0, [SP,#0x18+var_10]   ; var10 = 4
hello32:0286B5CA MOVS            R0, #8
hello32:0286B5CC STR             R0, [SP,#0x18+var_C]    ; varc = 8
```
    R0-3会作为参数进行传递, 其他参数会压栈处理, 并且R7会赋值为SP,后面基于R7(SP)进行寻址操作

```asm
hello32:0286B5B0 my_argIsStruct
hello32:0286B5B0
hello32:0286B5B0 var_18= -0x18
hello32:0286B5B0 var_14= -0x14
hello32:0286B5B0 var_10= -0x10
hello32:0286B5B0 var_C= -0xC
hello32:0286B5B0
hello32:0286B5B0 PUSH            {R7,LR}                 ; save r7, lr. r7 will as sp
hello32:0286B5B2 MOV             R7, SP                  ; r7 = sp
hello32:0286B5B4 SUB             SP, SP, #0x10           ; local var
hello32:0286B5B6 LDR.W           R12, [R7,#8]            ; ????????????????????????????????????
hello32:0286B5BA STR             R2, [SP,#0x18+var_C]    ; varc = r2
hello32:0286B5BC STR             R1, [SP,#0x18+var_10]   ; var10 = r1
hello32:0286B5BE STR             R0, [SP,#0x18+var_14]   ; var14 = r0
hello32:0286B5C0 STR             R3, [SP,#0x18+var_18]   ; var18 = r3
hello32:0286B5C2 MOVS            R0, #0
hello32:0286B5C4 STR             R0, [SP,#0x18+var_14]   ; var14 = 0
hello32:0286B5C6 MOVS            R0, #4
hello32:0286B5C8 STR             R0, [SP,#0x18+var_10]   ; var10 = 4
hello32:0286B5CA MOVS            R0, #8
hello32:0286B5CC STR             R0, [SP,#0x18+var_C]    ; varc = 8
hello32:0286B5CE LDR             R1, [SP,#0x18+var_14]   ; r1 is arg1 0
hello32:0286B5D0 LDR             R0, =(unk_286B485 - 0x286B5D6)
hello32:0286B5D2 ADD             R0, PC                  ; unk_286B485 ; r0 is arg0 is str
hello32:0286B5D4 BLX             unk_286B6D0             ; printf(r0, r1)
hello32:0286B5D8 LDR             R1, [SP,#0x18+var_10]
hello32:0286B5DA LDR             R0, =(unk_286B463 - 0x286B5E0)
hello32:0286B5DC ADD             R0, PC                  ; unk_286B463
hello32:0286B5DE BLX             unk_286B6D0
hello32:0286B5E2 LDR             R1, [SP,#0x18+var_C]
hello32:0286B5E4 LDR             R0, =(unk_286B3E5 - 0x286B5EA)
hello32:0286B5E6 ADD             R0, PC                  ; unk_286B3E5
hello32:0286B5E8 BLX             unk_286B6D0
hello32:0286B5EC LDR             R1, [SP,#0x18+var_18]   ; var18 = r3
hello32:0286B5EE LDR             R0, =(unk_286B435 - 0x286B5F4)
hello32:0286B5F0 ADD             R0, PC                  ; unk_286B435
hello32:0286B5F2 BLX             unk_286B6D0             ; printf(r0, r3)
hello32:0286B5F6 LDR             R1, [R7,#8]             ; stack var [r7+8]
hello32:0286B5F8 LDR             R0, =(unk_286B407 - 0x286B5FE)
hello32:0286B5FA ADD             R0, PC                  ; unk_286B407
hello32:0286B5FC BLX             unk_286B6D0             ; printf(r0, stack var[r7+8])
hello32:0286B600 ADD             SP, SP, #0x10
hello32:0286B602 POP             {R7,PC}
hello32:0286B602 ; End of function my_argIsStruct
```

## 相关重点


    Q: 为什么会将SP赋值给R7 ?

    A: R7会赋值为SP,后面基于R7(SP)进行寻址操作, 所以为什么local var都是负数偏移

    Q: LDR.W R12, [R7,#8] 又有什么意义?

    A: 目前并不清楚, 和友人一起确认就是废物代码

# ARM64调用约定

参数1~参数8 分别保存到 X0~X7 寄存器中 ，剩下的参数从右往左依次入栈，被调用者实现栈平衡，返回值存放在 X0 中。


[https://docs.microsoft.com/zh-cn/cpp/build/arm64-windows-abi-conventions?view=msvc-170](https://docs.microsoft.com/zh-cn/cpp/build/arm64-windows-abi-conventions?view=msvc-170)

[AArch64状态下的Canary机制](https://cataloc.gitee.io/blog/2021/04/24/Android%E9%80%86%E5%90%91%E4%B8%AD%E7%9A%84Canary%E6%9C%BA%E5%88%B6/#AArch32%E7%8A%B6%E6%80%81%E4%B8%8B%E7%9A%84Canary%E6%9C%BA%E5%88%B6)

```c
int test9(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int stack1) {
    return a1 + a2 + a3 + a4 + a5 + a7 + a8 + stack1;
}

// ## arm learn 01
int main(int argc, char const *argv[]) {
    while (1) {
        //arm 64 args
        test9(1, 2, 3, 4, 5, 6, 7, 8, 9);
        sleep(5);
    }
    return 0;
}
```

```asm
hello:0000005C45037910 SUB             SP, SP, #0x30
hello:0000005C45037914 LDR             W8, [SP,#0x30+arg_0]    ; stack_var1 to r8
hello:0000005C45037918 STR             W0, [SP,#0x30+var_4]    ; r0 to var4
hello:0000005C4503791C STR             W1, [SP,#0x30+var_8]    ; r1 to var8
hello:0000005C45037920 STR             W2, [SP,#0x30+var_C]    ; r2 to varc
hello:0000005C45037924 STR             W3, [SP,#0x30+var_10]   ; r3 to var16
hello:0000005C45037928 STR             W4, [SP,#0x30+var_14]   ; r4 to var20
hello:0000005C4503792C STR             W5, [SP,#0x30+var_18]   ; r5 to var24
hello:0000005C45037930 STR             W6, [SP,#0x30+var_1C]   ; r6 to var28
hello:0000005C45037934 STR             W7, [SP,#0x30+var_20]   ; r7 to var32
hello:0000005C45037938 STR             W8, [SP,#0x30+var_24]   ; r8 = stack_var1 to var36
hello:0000005C4503793C LDR             W8, [SP,#0x30+var_4]    ; r8 = 1
hello:0000005C45037940 LDR             W9, [SP,#0x30+var_8]    ; r9 = 2
hello:0000005C45037944 ADD             W8, W8, W9
hello:0000005C45037948 LDR             W9, [SP,#0x30+var_C]
hello:0000005C4503794C ADD             W8, W8, W9
hello:0000005C45037950 LDR             W9, [SP,#0x30+var_10]
hello:0000005C45037954 ADD             W8, W8, W9
hello:0000005C45037958 LDR             W9, [SP,#0x30+var_14]
hello:0000005C4503795C ADD             W8, W8, W9
hello:0000005C45037960 LDR             W9, [SP,#0x30+var_1C]
hello:0000005C45037964 ADD             W8, W8, W9
hello:0000005C45037968 LDR             W9, [SP,#0x30+var_20]
hello:0000005C4503796C ADD             W8, W8, W9
hello:0000005C45037970 LDR             W9, [SP,#0x30+var_24]
hello:0000005C45037974 ADD             W0, W8, W9              ; ret var use w0
hello:0000005C45037978 ADD             SP, SP, #0x30
hello:0000005C4503797C RET
```

![20211202220612](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211202220612.png)

![20211202221224](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211202221224.png)

![20211202222442](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211202222442.png)

**我们通过堆栈数据的展示可以了解到堆栈是8字节对齐模式存储的** 

![20211202223118](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211202223118.png)

```c
void my_argIsStruct(TPoint3D a1, int a2, int a3) {    
    //结构体作为参数，会把其字段按顺序依次放入寄存器或者栈，相当于把结构体拆解为多个参数，按照ARM传递参数规则来传递
    a1.dx_0 = 0xb1;
    a1.dy_4 = 0xb2;
    a1.dz_8 = 0xb3;
    printf("filed1:%d\n", a1.dx_0);
    printf("filed2:%d\n", a1.dy_4);
    printf("filed2:%d\n", a1.dz_8);
    printf("a2:%d\n", a2);
    printf("a3:%d\n", a3);
}

int main() {
    ...
    TPoint3D a1={0xa1, 0xa2, 0xa3};
    my_argIsStruct(a1, 0xa4, 0xa5);
    ...
}
```

```asm
hello:00000056BBEAF810 ; =============== S U B R O U T I N E =======================================
hello:00000056BBEAF810
hello:00000056BBEAF810 ; Attributes: bp-based frame
hello:00000056BBEAF810
hello:00000056BBEAF810 my_argIsStruct
hello:00000056BBEAF810
hello:00000056BBEAF810 var_60= -0x60
hello:00000056BBEAF810 var_58= -0x58
hello:00000056BBEAF810 var_50= -0x50
hello:00000056BBEAF810 var_48= -0x48
hello:00000056BBEAF810 var_3C= -0x3C
hello:00000056BBEAF810 var_38= -0x38
hello:00000056BBEAF810 var_34= -0x34
hello:00000056BBEAF810 var_30= -0x30
hello:00000056BBEAF810 var_2C= -0x2C
hello:00000056BBEAF810 var_28= -0x28
hello:00000056BBEAF810 var_24= -0x24
hello:00000056BBEAF810 var_20= -0x20
hello:00000056BBEAF810 var_18= -0x18
hello:00000056BBEAF810 var_C= -0xC
hello:00000056BBEAF810 var_8= -8
hello:00000056BBEAF810 var_4= -4
hello:00000056BBEAF810 var_s0=  0
hello:00000056BBEAF810
hello:00000056BBEAF810 SUB             SP, SP, #0x70
hello:00000056BBEAF814 STP             X29, X30, [SP,#0x60+var_s0]                      ;save fp, lr
hello:00000056BBEAF818 ADD             X29, SP, #0x60                                   ;这句
hello:00000056BBEAF81C MOV             X8, #0xC                                         ;sizeof(TPoint3D)
hello:00000056BBEAF820 MOV             W9, #0xB1                                        ;to reg9
hello:00000056BBEAF824 MOV             W10, #0xB2                                       ;to reg10
hello:00000056BBEAF828 MOV             W11, #0xB3                                       ;to reg11
hello:00000056BBEAF82C ADRP            X12, #aFiled1D@PAGE     ; "filed1:%d\n"          ;to reg12 with pc rel
hello:00000056BBEAF830 ADD             X12, X12, #aFiled1D@PAGEOFF ; "filed1:%d\n"       
hello:00000056BBEAF834 ADRP            X13, #aFiled2D@PAGE     ; "filed2:%d\n"          ;to reg13 with pc rel
hello:00000056BBEAF838 ADD             X13, X13, #aFiled2D@PAGEOFF ; "filed2:%d\n"
hello:00000056BBEAF83C ADRP            X14, #aA2D@PAGE         ; "a2:%d\n"              ;to reg14 with pc rel
hello:00000056BBEAF840 ADD             X14, X14, #aA2D@PAGEOFF ; "a2:%d\n"
hello:00000056BBEAF844 ADRP            X15, #aA3D@PAGE         ; "a3:%d\n"              ;to reg15 with pc rel
hello:00000056BBEAF848 ADD             X15, X15, #aA3D@PAGEOFF ; "a3:%d\n"              
hello:00000056BBEAF84C SUB             X16, X29, #-var_C                                ;
hello:00000056BBEAF850 SUB             X17, X29, #-var_20
hello:00000056BBEAF854 STUR            X0, [X29,#var_20]                                ;                          stack_var_20 = x0 = 0XA10XA2
hello:00000056BBEAF858 STUR            X1, [X29,#var_18]                                ;                          stack_var_18 = x1 = 0XA3
hello:00000056BBEAF85C MOV             X0, X16                                          ;                          x0 = stack_var_C
hello:00000056BBEAF860 MOV             X1, X17                                          ;                          x1 = stack_var_20
hello:00000056BBEAF864 STUR            W2, [X29,#var_2C]                                ;                          stack_var_2c = w2 = 0XA4
hello:00000056BBEAF868 MOV             X2, X8                                           ;                          x2 = x8 = 12 = sizeof(TPoint3D) 
hello:00000056BBEAF86C STR             W3, [SP,#0x60+var_30]                            ;[r->s] w3 -> var30
hello:00000056BBEAF870 STR             W9, [SP,#0x60+var_34]                            ;[r->s] w9 -> var34
hello:00000056BBEAF874 STR             W10, [SP,#0x60+var_38]                           ;[r->s] w10-> var38
hello:00000056BBEAF878 STR             W11, [SP,#0x60+var_3C]                           ;[r->s] w11-> var3c
hello:00000056BBEAF87C STR             X12, [SP,#0x60+var_48]                           ;[r->s] w12-> var48
hello:00000056BBEAF880 STR             X13, [SP,#0x60+var_50]                           ;[r->s] w13-> var50
hello:00000056BBEAF884 STR             X14, [SP,#0x60+var_58]                           ;[r->s] w14-> var58
hello:00000056BBEAF888 STR             X15, [SP,#0x60+var_60]                           ;[r->s] w15-> var60
hello:00000056BBEAF88C BL              loc_56BBEAF6A0                                   ;                           memcpy
hello:00000056BBEAF890 LDUR            W9, [X29,#var_2C]
hello:00000056BBEAF894 STUR            W9, [X29,#var_24]
hello:00000056BBEAF898 LDR             W10, [SP,#0x60+var_30]
hello:00000056BBEAF89C STUR            W10, [X29,#var_28]
hello:00000056BBEAF8A0 LDR             W11, [SP,#0x60+var_34]
hello:00000056BBEAF8A4 STUR            W11, [X29,#var_C]
hello:00000056BBEAF8A8 LDR             W3, [SP,#0x60+var_38]
hello:00000056BBEAF8AC STUR            W3, [X29,#var_8]
hello:00000056BBEAF8B0 LDR             W4, [SP,#0x60+var_3C]
hello:00000056BBEAF8B4 STUR            W4, [X29,#var_4]
hello:00000056BBEAF8B8 LDUR            W1, [X29,#var_C]
hello:00000056BBEAF8BC LDR             X0, [SP,#0x60+var_48]
hello:00000056BBEAF8C0 BL              loc_56BBEAF680
hello:00000056BBEAF8C4 LDUR            W1, [X29,#var_8]
hello:00000056BBEAF8C8 LDR             X8, [SP,#0x60+var_50]
hello:00000056BBEAF8CC MOV             X0, X8
hello:00000056BBEAF8D0 BL              loc_56BBEAF680
hello:00000056BBEAF8D4 LDUR            W1, [X29,#var_4]
hello:00000056BBEAF8D8 LDR             X8, [SP,#0x60+var_50]
hello:00000056BBEAF8DC MOV             X0, X8
hello:00000056BBEAF8E0 BL              loc_56BBEAF680
hello:00000056BBEAF8E4 LDUR            W1, [X29,#var_24]
hello:00000056BBEAF8E8 LDR             X8, [SP,#0x60+var_58]
hello:00000056BBEAF8EC MOV             X0, X8
hello:00000056BBEAF8F0 BL              loc_56BBEAF680
hello:00000056BBEAF8F4 LDUR            W1, [X29,#var_28]
hello:00000056BBEAF8F8 LDR             X8, [SP,#0x60+var_60]
hello:00000056BBEAF8FC MOV             X0, X8
hello:00000056BBEAF900 BL              loc_56BBEAF680
hello:00000056BBEAF904 LDP             X29, X30, [SP,#0x60+var_s0]
hello:00000056BBEAF908 ADD             SP, SP, #0x70
hello:00000056BBEAF90C RET
hello:00000056BBEAF90C ; End of function my_argIsStruct
```

## 通过上面的汇编代码我们知道，loc_56BBEAF6A0是memcpy， 但是为什么会产生memcpy呢？

![20211205134956](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211205134956.png)

我最开始一直没搞明白，如果编译器只是把寄存器存储到栈，那么挨个赋值不就完事儿了么？类似于下面的2句，也就完成了结构体到堆栈的传输

```asm
hello:00000056BBEAF854 STUR            X0, [X29,#var_20]                                ;                          stack_var_20 = x0 = 0XA10XA2
hello:00000056BBEAF858 STUR            X1, [X29,#var_18]                                ;                          stack_var_18 = x1 = 0XA3
```


[https://www.cnblogs.com/sinferwu/p/12810595.html](https://www.cnblogs.com/sinferwu/p/12810595.html)