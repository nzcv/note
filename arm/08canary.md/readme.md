s
```asm
.text:0000000000000860                 EXPORT my_argIsStruct
.text:0000000000000860 my_argIsStruct                          ; CODE XREF: main+98↓p
.text:0000000000000860
.text:0000000000000860 var_48          = -0x48
.text:0000000000000860 var_44          = -0x44
.text:0000000000000860 var_40          = -0x40
.text:0000000000000860 var_3C          = -0x3C
.text:0000000000000860 var_38          = -0x38
.text:0000000000000860 var_30          = -0x30
.text:0000000000000860 var_2C          = -0x2C
.text:0000000000000860 var_28          = -0x28
.text:0000000000000860 var_20          = -0x20
.text:0000000000000860 var_18          = -0x18
.text:0000000000000860 var_10          = -0x10
.text:0000000000000860 var_8           = -8
.text:0000000000000860 var_s0          =  0
.text:0000000000000860
.text:0000000000000860 ; __unwind {
.text:0000000000000860                 SUB             SP, SP, #0x60
.text:0000000000000864                 STP             X29, X30, [SP,#0x50+var_s0] ; save fp lr
.text:0000000000000868                 ADD             X29, SP, #0x50 ; x29 for addring
.text:000000000000086C                 MRS             X8, #3, c13, c0, #2 ; canary
.text:0000000000000870                 LDR             X9, [X8,#0x28] ; x9 for canary
.text:0000000000000874                 STUR            X9, [X29,#var_8] ; var8 = x9 = canary
.text:0000000000000878                 STUR            X1, [X29,#var_10] ; var10 = x1
.text:000000000000087C                 STUR            X0, [X29,#var_18] ; var18 = x0
.text:0000000000000880                 LDUR            X9, [X29,#var_18] ; x9 = var18 = x0
.text:0000000000000884                 LDUR            W10, [X29,#var_10] ; w10 = var10 = x1
.text:0000000000000888                 STR             W10, [SP,#0x50+var_20] ; var20 = w10 = x1
.text:000000000000088C                 STR             X9, [SP,#0x50+var_28] ; var28 = x9 = x0
.text:0000000000000890                 STR             W2, [SP,#0x50+var_2C] ; var_2c = w2 = _z
.text:0000000000000894                 STR             W3, [SP,#0x50+var_30] ; var_30 = w3 = a1
.text:0000000000000898                 MOV             W10, WZR ; w10 = 0
.text:000000000000089C                 STR             W10, [SP,#0x50+var_28] ; var28 = 0
.text:00000000000008A0                 MOV             W10, #4
.text:00000000000008A4                 STR             W10, [SP,#0x50+var_28+4] ; var24 = 4
.text:00000000000008A8                 MOV             W10, #8
.text:00000000000008AC                 STR             W10, [SP,#0x50+var_20] ; var20 = 8
.text:00000000000008B0                 LDR             W1, [SP,#0x50+var_28] ; w1 = var28 = 0
.text:00000000000008B4                 ADRP            X0, #unk_A08@PAGE
.text:00000000000008B8                 ADD             X0, X0, #unk_A08@PAGEOFF ; format
.text:00000000000008BC                 STR             X8, [SP,#0x50+var_38] ; var38 = x8, will use x8 later
.text:00000000000008C0                 BL              .printf
.text:00000000000008C4                 LDR             W1, [SP,#0x50+var_28+4]
.text:00000000000008C8                 ADRP            X8, #unk_A2A@PAGE
.text:00000000000008CC                 ADD             X8, X8, #unk_A2A@PAGEOFF
.text:00000000000008D0                 STR             W0, [SP,#0x50+var_3C]
.text:00000000000008D4                 MOV             X0, X8  ; format
.text:00000000000008D8                 BL              .printf
.text:00000000000008DC                 LDR             W1, [SP,#0x50+var_20]
.text:00000000000008E0                 ADRP            X8, #unk_A4C@PAGE
.text:00000000000008E4                 ADD             X8, X8, #unk_A4C@PAGEOFF
.text:00000000000008E8                 STR             W0, [SP,#0x50+var_40]
.text:00000000000008EC                 MOV             X0, X8  ; format
.text:00000000000008F0                 BL              .printf
.text:00000000000008F4                 LDR             W1, [SP,#0x50+var_2C]
.text:00000000000008F8                 ADRP            X8, #unk_A6E@PAGE
.text:00000000000008FC                 ADD             X8, X8, #unk_A6E@PAGEOFF
.text:0000000000000900                 STR             W0, [SP,#0x50+var_44]
.text:0000000000000904                 MOV             X0, X8  ; format
.text:0000000000000908                 BL              .printf
.text:000000000000090C                 LDR             W1, [SP,#0x50+var_30]
.text:0000000000000910                 ADRP            X8, #unk_A9C@PAGE
.text:0000000000000914                 ADD             X8, X8, #unk_A9C@PAGEOFF
.text:0000000000000918                 STR             W0, [SP,#0x50+var_48]
.text:000000000000091C                 MOV             X0, X8  ; format
.text:0000000000000920                 BL              .printf
.text:0000000000000924                 LDR             X8, [SP,#0x50+var_38]
.text:0000000000000928                 LDR             X9, [X8,#0x28]
.text:000000000000092C                 LDUR            X11, [X29,#var_8]
.text:0000000000000930                 SUBS            X9, X9, X11
.text:0000000000000934                 B.NE            loc_948 ; stack guard check
.text:0000000000000938                 B               loc_93C
.text:000000000000093C ; ---------------------------------------------------------------------------
.text:000000000000093C
.text:000000000000093C loc_93C                                 ; CODE XREF: my_argIsStruct+D8↑j
.text:000000000000093C                 LDP             X29, X30, [SP,#0x50+var_s0]
.text:0000000000000940                 ADD             SP, SP, #0x60 ; '`'
.text:0000000000000944                 RET
.text:0000000000000948 ; ---------------------------------------------------------------------------
.text:0000000000000948
.text:0000000000000948 loc_948                                 ; CODE XREF: my_argIsStruct+D4↑j
.text:0000000000000948                 BL              .__stack_chk_fail
.text:0000000000000948 ; } // starts at 860
.text:0000000000000948 ; End of function my_argIsStruct
```