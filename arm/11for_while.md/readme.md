# For循环的实现没什么特别的

![20211207215744](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211207215744.png)

指令都学过了, 简单看看就好

```c
int test15() {
    int ret = 0;
    for(int i=0; i < 100; i ++) {
        ret += i;
    }
    return ret;
}
```

```asm
.text:0000000000000CC0 ; int __cdecl test15()
.text:0000000000000CC0                 EXPORT test15
.text:0000000000000CC0 test15                                  ; CODE XREF: main+12C↓p
.text:0000000000000CC0
.text:0000000000000CC0 var_8           = -8
.text:0000000000000CC0 var_4           = -4
.text:0000000000000CC0 i               =  8
.text:0000000000000CC0 ret             =  0xC
.text:0000000000000CC0
.text:0000000000000CC0 ; __unwind {
.text:0000000000000CC0                 SUB             SP, SP, #0x10
.text:0000000000000CC4                 STR             WZR, [SP,#0x10+var_4]
.text:0000000000000CC8                 STR             WZR, [SP,#0x10+var_8]
.text:0000000000000CCC
.text:0000000000000CCC loc_CCC                                 ; CODE XREF: test15+40↓j
.text:0000000000000CCC                 MOV             W8, #0x64
.text:0000000000000CD0                 LDR             W9, [SP,#0x10+var_8]
.text:0000000000000CD4                 CMP             W9, W8
.text:0000000000000CD8                 CSET            W8, LT
.text:0000000000000CDC                 TBNZ            W8, #0, loc_CE4
.text:0000000000000CE0                 B               loc_D04
.text:0000000000000CE4 ; ---------------------------------------------------------------------------
.text:0000000000000CE4
.text:0000000000000CE4 loc_CE4                                 ; CODE XREF: test15+1C↑j
.text:0000000000000CE4                 LDR             W8, [SP,#0x10+var_8]
.text:0000000000000CE8                 LDR             W9, [SP,#0x10+var_4]
.text:0000000000000CEC                 ADD             W8, W9, W8
.text:0000000000000CF0                 STR             W8, [SP,#0x10+var_4]
.text:0000000000000CF4                 LDR             W8, [SP,#0x10+var_8]
.text:0000000000000CF8                 ADD             W8, W8, #1
.text:0000000000000CFC                 STR             W8, [SP,#0x10+var_8]
.text:0000000000000D00                 B               loc_CCC
.text:0000000000000D04 ; ---------------------------------------------------------------------------
.text:0000000000000D04
.text:0000000000000D04 loc_D04                                 ; CODE XREF: test15+20↑j
.text:0000000000000D04                 LDR             W0, [SP,#0x10+var_4]
.text:0000000000000D08                 ADD             SP, SP, #0x10
.text:0000000000000D0C                 RET
.text:0000000000000D0C ; } // starts at CC0
```

# 关于do While循环


```c
int test16() {
    int ret = 0;
    int i = 0;
    while (i < 100)
    {
        ret += i;
        i++;
    }    
    return ret;
}
```

```asm
.text:0000000000000D84                 EXPORT test16
.text:0000000000000D84 test16                                  ; CODE XREF: main+B8↓p
.text:0000000000000D84
.text:0000000000000D84 var_8           = -8
.text:0000000000000D84 var_4           = -4
.text:0000000000000D84
.text:0000000000000D84 ; __unwind {
.text:0000000000000D84                 SUB             SP, SP, #0x10
.text:0000000000000D88                 STR             WZR, [SP,#0x10+var_4]                                    ;var4 = 0
.text:0000000000000D8C                 STR             WZR, [SP,#0x10+var_8]                                    ;var8 = 0
.text:0000000000000D90
.text:0000000000000D90 loc_D90                                 ; CODE XREF: test16+34↓j
.text:0000000000000D90                 LDR             W8, [SP,#0x10+var_8]                                     ;w8 = var8 = 0
.text:0000000000000D94                 SUBS            W8, W8, #0x64                                            ;w8 = w8 - 100 = -100
.text:0000000000000D98                 B.GE            loc_DBC                                                  ;if w8 >= 0 break
.text:0000000000000D9C                 LDR             W9, [SP,#0x10+var_8]                                     ;w9 = var8 = 0
.text:0000000000000DA0                 LDR             W8, [SP,#0x10+var_4]                                     ;w8 = var4 = 0
.text:0000000000000DA4                 ADD             W8, W8, W9                                               ;w8 = w8 + w9 = 0
.text:0000000000000DA8                 STR             W8, [SP,#0x10+var_4]                                     ;var4 = 0
.text:0000000000000DAC                 LDR             W8, [SP,#0x10+var_8]                                     ;w8 = var8 = 0
.text:0000000000000DB0                 ADD             W8, W8, #1                                               ;w8 += 1
.text:0000000000000DB4                 STR             W8, [SP,#0x10+var_8]                                     ;var8 = w8 = 1
.text:0000000000000DB8                 B               loc_D90                                                  ;loop
.text:0000000000000DBC ; ---------------------------------------------------------------------------
.text:0000000000000DBC
.text:0000000000000DBC loc_DBC                                 ; CODE XREF: test16+14↑j
.text:0000000000000DBC                 LDR             W0, [SP,#0x10+var_4]
.text:0000000000000DC0                 ADD             SP, SP, #0x10
.text:0000000000000DC4                 RET
.text:0000000000000DC4 ; } // starts at D84
```

```c
int test17() {
    int ret = 0;
    int i = 0;
    do
    {
        ret += i;
        i++;
    } while(i < 100);
    return ret;
}
```

```asm
.text:0000000000000D60 ; int __cdecl test17()
.text:0000000000000D60                 EXPORT test17
.text:0000000000000D60 test17                                  ; CODE XREF: main+134↓p
.text:0000000000000D60
.text:0000000000000D60 var_8           = -8
.text:0000000000000D60 var_4           = -4
.text:0000000000000D60 i               =  8
.text:0000000000000D60 ret             =  0xC
.text:0000000000000D60
.text:0000000000000D60 ; __unwind {
.text:0000000000000D60                 SUB             SP, SP, #0x10
.text:0000000000000D64                 STR             WZR, [SP,#0x10+var_4]        ;var4 = 0
.text:0000000000000D68                 STR             WZR, [SP,#0x10+var_8]        ;var8 = 0
.text:0000000000000D6C
.text:0000000000000D6C loc_D6C                                 ; CODE XREF: test17+38↓j 
.text:0000000000000D6C                 LDR             W8, [SP,#0x10+var_8]         ;w8 = var8 = 0
.text:0000000000000D70                 LDR             W9, [SP,#0x10+var_4]         ;w9 = var4 = 0
.text:0000000000000D74                 ADD             W8, W9, W8                   ;w8 = w8 + w9 = 0
.text:0000000000000D78                 STR             W8, [SP,#0x10+var_4]         ;var4 = w8 = 0
.text:0000000000000D7C                 LDR             W8, [SP,#0x10+var_8]         ;w8 = var8 = 0
.text:0000000000000D80                 ADD             W8, W8, #1                   ;w8 = w8 + 1 = 1
.text:0000000000000D84                 STR             W8, [SP,#0x10+var_8]         ;var8 = w8 = 1
.text:0000000000000D88                 MOV             W8, #0x64                    ;w8 = 100
.text:0000000000000D8C                 LDR             W9, [SP,#0x10+var_8]         ;w9 = var8 = 1
.text:0000000000000D90                 CMP             W9, W8                       ;w9 < w8
.text:0000000000000D94                 CSET            W8, LT                       ;w8 = 1
.text:0000000000000D98                 TBNZ            W8, #0, loc_D6C              ;jump if w8 == 9
.text:0000000000000D9C                 LDR             W0, [SP,#0x10+var_4]         ;
.text:0000000000000DA0                 ADD             SP, SP, #0x10
.text:0000000000000DA4                 RET
.text:0000000000000DA4 ; } // starts at D60
```