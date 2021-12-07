# 栈的对齐

ARM64里面 对栈的操作是16字节对齐的, 也就是一次开辟栈空间至少是16字节, 或者是16的倍数, 如果不是这个值会报错

## SP一次最少拉伸16个字节

```asm
.text ;代码段
.global _A,_B ;定义两个全局函数 A和B

_A:
	mov x0 ,#0xaaaa ;arm汇编中数据用#开头
	str x30,[sp,#-0x10]! ;在调用下一个函数之前临时保存lr寄存器中的地址, lr保存bl _A的下一条指令地址
	bl _B
	mov x0,#0xcccc
	ldr x30,[sp],#0x10 ;lr恢复
	ret  ;返回到bl指令所对应的下一条指令
	
_B:
	add x0, x0,#0x10
	ret
```


[https://juejin.cn/post/7022413524034912269](https://juejin.cn/post/7022413524034912269)