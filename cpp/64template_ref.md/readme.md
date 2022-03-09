# 模板的本质

# 模板函数实现交换

```c
#include <stdio.h>

template<class T>
void myswap(T &l, T &r) {
    T tmp;
    tmp = r;
    r = l;
    l = tmp;
}

int main(int argc, char** argv) {
    int a = 10, b = 20;
    myswap(a, b);
    return 0;
}
```

```asm
void myswap<int>(int&, int&):                    // @void myswap<int>(int&, int&)
        sub     sp, sp, #32                     // =32
        str     x0, [sp, #24]
        str     x1, [sp, #16]
        ldr     x8, [sp, #16]
        ldr     w9, [x8]
        str     w9, [sp, #12]
        ldr     x8, [sp, #24]
        ldr     w9, [x8]
        ldr     x8, [sp, #16]
        str     w9, [x8]
        ldr     w9, [sp, #12]
        ldr     x8, [sp, #24]
        str     w9, [x8]
        add     sp, sp, #32                     // =32
        ret
```

# 模板函数实现折半查找
```c
#include <stdio.h>

template<class T>
int Search_Bin(T ST[], int len, T key)
{
    int low = 1;
    int high = len;
    int mid;
    while (low <= high) {
        mid = (low+high) / 2;
        if (ST[mid] == key)
        if(true)
        {
            return mid;
        } else if(ST[mid] > key)
        {
            high = mid-1;
        }
        else {
            low = mid + 1;
       }
    }

    return 0;
}

struct Person {
    const char* name;
    int age;
};

int main(int argc, char** argv) {
    Person p[] = {
        {"Name", 10},
    };
    Person k = {"Name", 10};

    Search_Bin(p, 2, k);
    return 0;
}
```
![20211222232108](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211222232108.png)


# 引用

C++为了规避指针的灵活性, 使用引用类型达到相同目的, 底层没区别. 但是更加安全

```c
#include <stdio.h>

int Test1(int *x) {
    *x = 1;
}

int Test2(int &x) {
    x = 1;
}
```

![20211222232257](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211222232257.png)

1. 引用是C++新增类型
2. 引用类型只能赋值一次, 不能重新赋值
3. 引用是变量的一个别名
4. 引用可以想指针那样去访问, 修改对象的内存, 但是更加安全


# 友元函数
```c
#include <stdio.h>
#include <iostream>

template<class T>
int Search_Bin(T ST[], int len, T key)
{
    int low = 1;
    int high = len;
    int mid;
    while (low <= high) {
        mid = (low+high) / 2;
        if (ST[mid] == key)
        {
            return mid;
        } else if(ST[mid] > key)
        {
            high = mid-1;
        }
        else {
            low = mid + 1;
       }
    }

    return 0;
}

class Person {
public:
    const char* name;
    int age;
    friend void Print(Person &p); 
};

void Print(Person &p) {
    std::cout << p.name << p.age;
}

int main(int argc, char** argv) {
    Person p[] = {
        {"Name", 10},
    };
    Person k = {"Name", 10};

    Search_Bin(p, 2, k);
    return 0;
}
```

用途:
![20211223001836](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211223001836.png)
1. 运行符重载在某些场合需要使用友元
2. 两个类需要共享数据

区别:

1. 成员函数有this指针, 友元函数没有this指针

2. 友元函数不能被继承

# 运算符重载

和普通函数没区别

