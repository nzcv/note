
![20211208170055](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211208170055.png)

# memset进行初始化, 然后进行单个初始化

```c
int test18() {
    // a[0] = 0x55 , other is 0, use memset
    char a[100] = {0x55};
}
```

```asm
.text:0000000000000E40 ; int __cdecl test18()
.text:0000000000000E40                 EXPORT test18
.text:0000000000000E40 test18                                  ; CODE XREF: main+190↓p
.text:0000000000000E40
.text:0000000000000E40 var_78          = -0x78
.text:0000000000000E40 var_74          = -0x74
.text:0000000000000E40 var_10          = -0x10
.text:0000000000000E40 a               =  0xC
.text:0000000000000E40
.text:0000000000000E40 ; __unwind {
.text:0000000000000E40                 SUB             SP, SP, #0x80
.text:0000000000000E44                 STR             X30, [SP,#0x80+var_10]
.text:0000000000000E48                 MOV             X2, #0x64
.text:0000000000000E4C                 MOV             W8, #0x55
.text:0000000000000E50                 ADD             X0, SP, #0x80+var_74
.text:0000000000000E54                 MOV             W1, #0
.text:0000000000000E58                 STR             W8, [SP,#0x80+var_78]
.text:0000000000000E5C                 BL              .memset
.text:0000000000000E60                 LDR             W8, [SP,#0x80+var_78]
.text:0000000000000E64                 STRB            W8, [SP,#0x80+var_74]
.text:0000000000000E68                 BRK             #1
.text:0000000000000E68 ; } // starts at E40
```

# memcpy直接进行拷贝初始化

```c
//## array
int test18() {
    // 一直都是使用memcpy
    char a[16] = {0x55};
    char a[10] = {0xa1, 0xa2, 0xa3, 0xa4};
}
```

```asm
hello:000000583B9EDE40 SUB             SP, SP, #0x20           ; CODE XREF: test18+1B0↓p
hello:000000583B9EDE44 STR             X30, [SP,#0x20+var_10]
hello:000000583B9EDE48 ADRP            X1, #unk_583B9EE090@PAGE
hello:000000583B9EDE4C ADD             X1, X1, #unk_583B9EE090@PAGEOFF
hello:000000583B9EDE50 MOV             X2, #0x10
hello:000000583B9EDE54 MOV             X0, SP
hello:000000583B9EDE58 BL              unk_583B9ED6E0
hello:000000583B9EDE5C BRK             #1
```

![20211208165742](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211208165742.png)


# 汇编特征

等宽连续