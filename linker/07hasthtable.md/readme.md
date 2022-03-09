# 导出函数与变量

符号表

```shell
[ LOCAL]address=0x00000000, size=00000000, type=STT_NOTYPE bind=  STBLOCAL ndx=   0 name=
[IMPORT]address=0x00000000, size=00000000, type=  STT_FUNC bind=STB_GLOBAL ndx=   0 name=__cxa_finalize
[IMPORT]address=0x00000000, size=00000000, type=  STT_FUNC bind=STB_GLOBAL ndx=   0 name=__cxa_atexit
# 导出函数
[EXPORT]address=0x000007b9, size=00000056, type=  STT_FUNC bind=STB_GLOBAL ndx=   c name=say_hello
[IMPORT]address=0x00000000, size=00000000, type=  STT_FUNC bind=STB_GLOBAL ndx=   0 name=__android_log_print
# 导出函数
[EXPORT]address=0x000007f1, size=00000040, type=  STT_FUNC bind=STB_GLOBAL ndx=   c name=printf_hello
[IMPORT]address=0x00000000, size=00000000, type=  STT_FUNC bind=STB_GLOBAL ndx=   0 name=printf
# 导出全局变量
[EXPORT]address=0x00004008, size=00004096, type=STT_OBJECT bind=STB_GLOBAL ndx=  16 name=a
[   ABS]address=0x00004008, size=00000000, type=STT_NOTYPE bind=STB_GLOBAL ndx=fff1 name=_edata
[   ABS]address=0x00005009, size=00000000, type=STT_NOTYPE bind=STB_GLOBAL ndx=fff1 name=_end
[   ABS]address=0x00004008, size=00000000, type=STT_NOTYPE bind=STB_GLOBAL ndx=fff1 name=__bss_start
[IMPORT]address=0x00000000, size=00000000, type=STT_OBJECT bind=STB_GLOBAL ndx=   0 name=__stack_chk_guard
[IMPORT]address=0x00000000, size=00000000, type=  STT_FUNC bind=STB_GLOBAL ndx=   0 name=__stack_chk_fail
[IMPORT]address=0x00000000, size=00000000, type=  STT_FUNC bind=STB_GLOBAL ndx=   0 name=fprintf
[IMPORT]address=0x00000000, size=00000000, type=  STT_FUNC bind=STB_GLOBAL ndx=   0 name=fflush
[IMPORT]address=0x00000000, size=00000000, type=  STT_FUNC bind=STB_GLOBAL ndx=   0 name=abort
[IMPORT]address=0x00000000, size=00000000, type=STT_OBJECT bind=STB_GLOBAL ndx=   0 name=__sF
[IMPORT]address=0x00000000, size=00000000, type=  STT_FUNC bind=STB_GLOBAL ndx=   0 name=__aeabi_memcpy
[IMPORT]address=0x00000000, size=00000000, type=  STT_FUNC bind=STB_GLOBAL ndx=   0 name=__aeabi_memclr8
[IMPORT]address=0x00000000, size=00000000, type=  STT_FUNC bind=STB_GLOBAL ndx=   0 name=dladdr
[IMPORT]address=0x00000000, size=00000000, type=  STT_FUNC bind=STB_GLOBAL ndx=   0 name=snprintf
[IMPORT]address=0x00000000, size=00000000, type=  STT_FUNC bind=STB_GLOBAL ndx=   0 name=__gnu_Unwind_Find_exidx
```

## HASH表

![20211229150103](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211229150103.png)

### DT_HASH

https://flapenguin.me/elf-dt-hash

```c
// 查找导出函数
static Elf32_Sym* soinfo_elf_lookup(soinfo* si, unsigned hash, const char* name) {
    Elf32_Sym* symtab = si->symtab;
    const char* strtab = si->strtab;

    TRACE_TYPE(LOOKUP, "SEARCH %s in %s@0x%08x %08x %d",
               name, si->name, si->base, hash, hash % si->nbucket);

    for (unsigned n = si->bucket[hash % si->nbucket]; n != 0; n = si->chain[n]) {
        Elf32_Sym* s = symtab + n;
        if (strcmp(strtab + s->st_name, name)) continue;

            /* only concern ourselves with global and weak symbol definitions */
        switch(ELF32_ST_BIND(s->st_info)){
        case STB_GLOBAL:
        case STB_WEAK:
            if (s->st_shndx == SHN_UNDEF) {
                continue;
            }

            TRACE_TYPE(LOOKUP, "FOUND %s in %s (%08x) %d",
                       name, si->name, s->st_value, s->st_size);
            return s;
        }
    }

    return NULL;
}
```

```c
LOGD("================export================");
    for (int i = 0; i < soinfo.nbucket; i++) {
        for (int n = soinfo.bucket[i]; n != 0; n = soinfo.chain[n]) {
            Elf32_Sym* s = soinfo.symtab_ + n;
            auto bind    = ELF32_ST_BIND(s->st_info);
            auto type    = ELF32_ST_TYPE(s->st_info);
            if (s->st_shndx == SHN_UNDEF) {
                continue;
            }
            switch (ELF32_ST_BIND(s->st_info)) {
                case STB_GLOBAL: //export
                case STB_WEAK: {
                    if (s->st_shndx == SHN_UNDEF) {
                        continue;
                    }
                    auto bind_type = bind == STB_GLOBAL? "STB_GLOBAL" : "STB_WEAK";
                    auto export_name = soinfo.strtab_ + s->st_name;
                    auto addr        = soinfo.load_bias_ + s->st_value;
                    LOGD("[%s]%25s,addr=%p", bind_type, export_name, addr);
                } break;
                case STB_LOCAL: {
                    LOGD("[STB_LOCAL]");
                } break;
            }
        }
    }
```

其实符号表给了我们种种迹象, 让我们识别是什么是export函数

### DT_GNU_HASH

![20211228013603](https://cdn.jsdelivr.net/gh/nzcv/picgo/20211228013603.png)

![gnu_hash](https://cdn.jsdelivr.net/gh/nzcv/picgo/gnu_hash.png)


https://flapenguin.me/elf-dt-gnu-hash

文章里面已经讲解的很详细了,这里就不过多的废话了