
# 结构体字节对齐

```c
typedef struct {
    int a;
    char b; //字节对齐, 占用4个字节
} klass;


int test20() {
    klass k = {0};
}
```

![struct01](https://cdn.jsdelivr.net/gh/nzcv/picgo/struct01.png)


## 关于数组当参数对应sizeof

![struct03_sizeof](https://cdn.jsdelivr.net/gh/nzcv/picgo/struct03_sizeof.png)