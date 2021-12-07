
# For循环的实现没什么特别的

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