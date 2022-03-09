# 虚函数

```c
#include <stdio.h>

class Base {
    public:
    void Fun1() {
        printf("Fun\n");
    }
    virtual void Fun2() {
        printf("Fun\n");
    }
};

int main() {
    Base a;
    Base* pb = &a;
    a.Fun1();
    a.Fun2();

    pb->Fun1();
    pb->Fun2();
    return 0;
}
```


## 虚函数表


    当类声明中有虚函数时, 编译器会自动生成虚函数表, 并在构造函数中进行初始化.

![20211220083147](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211220083147.png)



## 虚函数调用

![20211220083701](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211220083701.png)


1. 直接调用和普通函数没有任何区别

![20211220083408](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211220083408.png)

2. 间接调用

通过x10寄存器来确认具体应该调用哪一个函数

![20211220083543](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211220083543.png)


# 单继承无函数覆盖

![20211220214019](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211220214019.png)

# 单继承有函数覆盖

![20211220214130](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211220214130.png)

# 多继承无函数覆盖

几个父类几个虚函数表

# 多继承有函数覆盖

覆盖了哪个函数在哪个虚函数表

 