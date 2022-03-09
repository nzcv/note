# 指针的本质

对编译器而言, 指针仅仅就是一种类型

![20211212192549](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211212192549.png)


## 声明/赋值/Sizeof

![20211209163004](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211209163004.png)

Arm64 带*号就是8个字节

## 自增++/--

![20211209163657](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211209163657.png)

    ++/-- 等价于 +=1/-=1

    对应数值为去掉一个*,对应的宽度

## 加减运算+/-

![pointer01](https://cdn.jsdelivr.net/gh/nzcv/picgo/pointer01.png)


总结：

1、带*类型的变量可以加、减一个整数，但不能乘或者除；

2、带*类型变量与其他整数相加或者相减时：

    带*类型变量 + N = 带*类型变量 + N*(去掉一个*后类型的宽度)；
    
    带*类型变量 - N = 带*类型变量 - N*(去掉一个*后类型的宽度)。

## 指针相减

    带*类型变量1 - 带*类型变量2  = (v(带*类型变量1) - v(带*类型变量2))/去掉一个*后类型的宽度

![20211210104915](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211210104915.png)


## &符号的使用

    & 可以获取任何一个变量的地址
    &a 的类型 就是a的类型+*

```c
int a;
void test01() {
    int b;
    int* pa = &a;
}

void test02() {
    int p = 10;

    int *******p7;
    int ******p6;
    int *****p5;
    int ****p4;
    int ***p3;
    int **p2;
    int *p1;

    p1 = &p;
    p2 = &p1;
    p3 = &p2;
    p4 = &p3;
    p5 = &p4;
    p6 = &p5;
    p7 = &p6;
}

void test03() {
    char a = 0xa1;
    short b = 0xa2;
    int c = 0xa3;

    char *pa= &a;
    short *pb = &b;
    int *pc = &c;

    char **ppa = &pa;
    short **ppb = &pb;
    int **ppc = &pc;
}
```
![20211210083548](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211210083548.png)


![20211210084340](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211210084340.png)

## *符号的使用

```c
int x = 10;

int *px = &x;
int &&ppx = &px;
int *px1 = *ppx;
```

![20211210140053](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211210140053.png)


## 指针与数组

![pointer05](https://cdn.jsdelivr.net/gh/nzcv/picgo/pointer05.png)


## 字符串与数组和指针

```c
char  *x = "china";
char y[] = "china";

void testb() {
    *(x+1) = "A"; // 非法修改
    y[1] = "A";
}
```

```c
char *a = "if"
char *b = "for"
char *c = "while"

char keyword[] = {
    "if",
    "for",
    "while".
}
```

## 指针与结构体

```c
struct Arg
{
    int a;
    int b;
    int c;
};

void testc() {
    //宽度 ++ -- +N -N SUB CMP
    Arg a;
    Arg* pArg = &a;
    //pArg = 
}
```

## 指针分类(指针/结构体指针/数组指针/函数指针)

1. 指针

2. 结构体指针

3. 数组指针
 
```c
int *a[5];          //指针数组
int a[5];           //数组
int (*pa)[5];       //数组指针
```

```c
#include <stdio.h>

void test() {
    int a[5] = {1, 2, 3, 4, 5};
    int (*pa)[2];
    
    pa = (int (*)[2])a;

    printf("%lx, %lx\n", pa , *pa);

    printf("%lx\n", *(*(pa + 0) + 0)); //pa[0][0] 0*2 + 0
    printf("%lx\n", *(*(pa + 1) + 1)); //pa[1][1] 1*2 + 1
    printf("%lx\n", *(*(pa + 3) + 3)); //pa[1][1] 3*2 + 3

    char (*pb)[2][3];
    pb = (char (*)[2][3])a;
    printf("%lx\n", *(*(*(pb + 0) + 0) + 0)); // (0*2 + 0)*3 + 0
    printf("%lx\n", *(*(*(pb + 2) + 3) + 4)); // (2*2 + 3)*3 + 4 = 25
    printf("%lx\n", *(*(*(pb + 3) + 2) + 5)); // (3*2 + 2)*3 + 5 = 29
    printf("%lx\n", *(*(*(pb + 2) + 2) + 2)); // (2*2 + 2)*3 + 2 = 20


}
```

![20211212192259](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211212192259.png)

4. 函数指针

![20211212232514](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211212232514.png)

```c
int fun(int a, int b); //函数
int (*pfun)(int a, int b); //函数指针
```