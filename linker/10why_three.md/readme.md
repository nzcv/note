## 为什么so里面有两个PT_LOAD段, 在maps文件里面却有三段映射

        其中一个疑惑便是为什么elf文件里面只有2个段, 但是到内存后却有3段映射.

这里尝试做一些解答.

### 对应现象

1. 通过ida观察elf具有两个PT_LOAD段

![linker](https://cdn.jsdelivr.net/gh/yhnu/PicBed/20211116095810.png)

2. 通过maps文件观察, 具有3个映射段

![linker](https://cdn.jsdelivr.net/gh/yhnu/PicBed/20211116095257.png)

3. 通过源码观察, 确实只加载2个段

![linker](https://cdn.jsdelivr.net/gh/yhnu/PicBed/20211115132401.png)


### 内核层mmap观察分配情况

```shell
130|OnePlus7T:/data/local/tmp # dmesg -w|grep kr|grep sotest
[ 3142.307841] [info]kr_mmap mmap file libsotest.so. vaddr=0x7b51953000, len=0x1d0 prot=1, off=0x0(ReadProgramHeaders)
[ 3142.307861] [info]kr_mmap mmap file libsotest.so. vaddr=0x7b51952000, len=0xae0 prot=1, off=0x35000(ReadSectionHeaders)
[ 3142.307891] [info]kr_mmap mmap file libsotest.so. vaddr=0x7b51950000, len=0x1ca0 prot=1, off=0x34000(ReadDynamicSection)
[ 3142.307906] [info]kr_mmap mmap file libsotest.so. vaddr=0x7b5194f000, len=0xaeb prot=1, off=0x0(ReadDynamicSection)
[ 3142.308172] [info]kr_mmap mmap file libsotest.so. vaddr=0x7abcd88000, len=0x34988 prot=5, off=0x0(LoadSegments)
[ 3142.308199] [info]kr_mmap mmap file libsotest.so. vaddr=0x7abcdcc000, len=0x11f8 prot=3, off=0x34000(LoadSegments)
```

通过上面的日志信息可以了解到, linker在Android10上面总计进行了6次mmap, 分别为

(1) ReadProgramHeaders

(2) ReadSectionHeaders

(3) ReadDynamicSection

(4) ReadDynamicSection

(5) LoadSegments(通过文件偏移, 可以知晓这里对应elf的第一个段)

(6) LoadSegments(通过文件偏移, 可以知道这里对应elf的第二个段)

其中最后2次映射由LoadSegments引发,也就是我们真正要关注的问题点. 通过mmap观察走两次mmap为正常行为,为什么maps有3段呢? 我们还需要进一步分析, 因此我又hook了mprotect的方法

(tips: 如果大家仔细观察, 这里又会有另外一个小疑问, 为什么mmap的偏移是0x34000, 但是elf的fileoffset是0x34D18. 这里后面再做详细解答)

### 详细堆栈信息如下

```shell
----------------------------------
kr_openat tgid=7635 path=/proc/7635/maps
kr_openat vaddr=0x7b51953000
        pc=0x7b52dace58 sp=0x7fe046bc00 c->sp=0x7fe046bc00 c->pc=0x7b52dace58 regs_len=320 stack_len=1024 path_len=1024
        #00 0x7b52dace58 /apex/com.android.runtime/bin/linker64+fce58(__dl_mmap64)
        #01 0x7b52cff5e8 /apex/com.android.runtime/bin/linker64+4f5e8(__dlMappedFileFragment::Map(int, long, unsigned long, unsigned long))
        #02 0x7b52cffeb0 /apex/com.android.runtime/bin/linker64+4feb0(__dlElfReader::ReadProgramHeaders())
        #03 0x7b52cff790 /apex/com.android.runtime/bin/linker64+4f790(__dlElfReader::Read(char const*, int, long, long))
        #04 0x7b52cf35ac /apex/com.android.runtime/bin/linker64+435ac(__dlload_library(android_namespace_t*, LoadTask*, std::__1::vector<LoadTask*, std::__1::allocator<LoadTask*> >*, int, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> > const&, bool))
        #05 0x7b52cf23f0 /apex/com.android.runtime/bin/linker64+423f0(__dlload_library(android_namespace_t*, LoadTask*, ZipArchiveCache*, std::__1::vector<LoadTask*, std::__1::allocator<LoadTask*> >*, int, bool))
----------------------------------
kr_openat tgid=7635 path=/proc/7635/maps
kr_openat vaddr=0x7b51952000
        pc=0x7b52dace58 sp=0x7fe046bc00 c->sp=0x7fe046bc00 c->pc=0x7b52dace58 regs_len=320 stack_len=1024 path_len=1024
        #00 0x7b52dace58 /apex/com.android.runtime/bin/linker64+fce58(__dl_mmap64)
        #01 0x7b52cff5e8 /apex/com.android.runtime/bin/linker64+4f5e8(__dlMappedFileFragment::Map(int, long, unsigned long, unsigned long))
        #02 0x7b52d000b4 /apex/com.android.runtime/bin/linker64+500b4(__dlElfReader::ReadSectionHeaders())
        #03 0x7b52cff79c /apex/com.android.runtime/bin/linker64+4f79c(__dlElfReader::Read(char const*, int, long, long))
        #04 0x7b52cf35ac /apex/com.android.runtime/bin/linker64+435ac(__dlload_library(android_namespace_t*, LoadTask*, std::__1::vector<LoadTask*, std::__1::allocator<LoadTask*> >*, int, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> > const&, bool))
        #05 0x7b52cf23f0 /apex/com.android.runtime/bin/linker64+423f0(__dlload_library(android_namespace_t*, LoadTask*, ZipArchiveCache*, std::__1::vector<LoadTask*, std::__1::allocator<LoadTask*> >*, int, bool)
----------------------------------
kr_openat tgid=7635 path=/proc/7635/maps
kr_openat vaddr=0x7b51950000
        pc=0x7b52dace58 sp=0x7fe046bbf0 c->sp=0x7fe046bbf0 c->pc=0x7b52dace58 regs_len=320 stack_len=1040 path_len=1024
        #00 0x7b52dace58 /apex/com.android.runtime/bin/linker64+fce58(__dl_mmap64)
        #01 0x7b52cff5e8 /apex/com.android.runtime/bin/linker64+4f5e8(__dlMappedFileFragment::Map(int, long, unsigned long, unsigned long))
        #02 0x7b52d00630 /apex/com.android.runtime/bin/linker64+50630(__dlElfReader::ReadDynamicSection())
        #03 0x7b52cff7a8 /apex/com.android.runtime/bin/linker64+4f7a8(__dlElfReader::Read(char const*, int, long, long))
        #04 0x7b52cf35ac /apex/com.android.runtime/bin/linker64+435ac(__dlload_library(android_namespace_t*, LoadTask*, std::__1::vector<LoadTask*, std::__1::allocator<LoadTask*> >*, int, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> > const&, bool))
        #05 0x7b52cf23f0 /apex/com.android.runtime/bin/linker64+423f0(__dlload_library(android_namespace_t*, LoadTask*, ZipArchiveCache*, std::__1::vector<LoadTask*, std::__1::allocator<LoadTask*> >*, int, bool)
----------------------------------
kr_openat tgid=7635 path=/proc/7635/maps
kr_openat vaddr=0x7b5194f000
        pc=0x7b52dace58 sp=0x7fe046bbf0 c->sp=0x7fe046bbf0 c->pc=0x7b52dace58 regs_len=320 stack_len=1040 path_len=1024
        #00 0x7b52dace58 /apex/com.android.runtime/bin/linker64+fce58(__dl_mmap64)
        #01 0x7b52cff5e8 /apex/com.android.runtime/bin/linker64+4f5e8(__dlMappedFileFragment::Map(int, long, unsigned long, unsigned long))
        #02 0x7b52d006a8 /apex/com.android.runtime/bin/linker64+506a8(__dlElfReader::ReadDynamicSection())
        #03 0x7b52cff7a8 /apex/com.android.runtime/bin/linker64+4f7a8(__dlElfReader::Read(char const*, int, long, long))
        #04 0x7b52cf35ac /apex/com.android.runtime/bin/linker64+435ac(__dlload_library(android_namespace_t*, LoadTask*, std::__1::vector<LoadTask*, std::__1::allocator<LoadTask*> >*, int, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> > const&, bool))
        #05 0x7b52cf23f0 /apex/com.android.runtime/bin/linker64+423f0(__dlload_library(android_namespace_t*, LoadTask*, ZipArchiveCache*, std::__1::vector<LoadTask*, std::__1::allocator<LoadTask*> >*, int, bool)
----------------------------------
kr_openat tgid=7635 path=/proc/7635/maps
kr_openat vaddr=0x7abcd88000
        pc=0x7b52dace58 sp=0x7fe046c050 c->sp=0x7fe046c050 c->pc=0x7b52dace58 regs_len=320 stack_len=16304 path_len=1024
        #00 0x7b52dace58 /apex/com.android.runtime/bin/linker64+fce58(__dl_mmap64)
        #01 0x7b52d00be4 /apex/com.android.runtime/bin/linker64+50be4(__dlElfReader::LoadSegments())
        #02 0x7b52d007fc /apex/com.android.runtime/bin/linker64+507fc(__dlElfReader::Load(address_space_params*))
        #03 0x7b52cea6e4 /apex/com.android.runtime/bin/linker64+3a6e4(__dlLoadTask::load(address_space_params*))
        #04 0x7b52ce9d04 /apex/com.android.runtime/bin/linker64+39d04(__dlfind_libraries(android_namespace_t*, soinfo*, char const* const*, unsigned long, soinfo**, std::__1::vector<soinfo*, std::__1::allocator<soinfo*> >*, unsigned long, int, android_dlextinfo const*, bool, bool, std::__1::vector<android_namespace_t*, std::__1::allocator<android_namespace_t*> >*))
        #05 0x7b52cecba0 /apex/com.android.runtime/bin/linker64+3cba0(__dldo_dlopen(char const*, int, android_dlextinfo const*, void const*))
        #06 0x7b52ce80dc /apex/com.android.runtime/bin/linker64+380dc(__dl___loader_android_dlopen_ext)
        #07 0x7b4df5b0b8 /apex/com.android.runtime/lib64/bionic/libdl.so+10b8
        #08 0x7b4e2dcfd8 /apex/com.android.runtime/lib64/libnativeloader.so+7fd8
        #09 0x7b4e2dcc5c /apex/com.android.runtime/lib64/libnativeloader.so+7c5c
        #10 0x7acb748938 /apex/com.android.runtime/lib64/libart.so+37b938
        #11 0x7ac16e10e0 /apex/com.android.runtime/lib64/libopenjdkjvm.so+50e0
        #12 0x7164aaf4 /system/framework/arm64/boot.oat+b9af4
----------------------------------
kr_openat tgid=7635 path=/proc/7635/maps
kr_openat vaddr=0x7abcdcc000
        pc=0x7b52dace58 sp=0x7fe046c050 c->sp=0x7fe046c050 c->pc=0x7b52dace58 regs_len=320 stack_len=16304 path_len=1024
        #00 0x7b52dace58 /apex/com.android.runtime/bin/linker64+fce58(__dl_mmap64)
        #01 0x7b52d00be4 /apex/com.android.runtime/bin/linker64+50be4(__dlElfReader::LoadSegments())
        #02 0x7b52d007fc /apex/com.android.runtime/bin/linker64+507fc(__dlElfReader::Load(address_space_params*))
        #03 0x7b52cea6e4 /apex/com.android.runtime/bin/linker64+3a6e4(__dlLoadTask::load(address_space_params*))
        #04 0x7b52ce9d04 /apex/com.android.runtime/bin/linker64+39d04(__dlfind_libraries(android_namespace_t*, soinfo*, char const* const*, unsigned long, soinfo**, std::__1::vector<soinfo*, std::__1::allocator<soinfo*> >*, unsigned long, int, android_dlextinfo const*, bool, bool, std::__1::vector<android_namespace_t*, std::__1::allocator<android_namespace_t*> >*))
        #05 0x7b52cecba0 /apex/com.android.runtime/bin/linker64+3cba0(__dldo_dlopen(char const*, int, android_dlextinfo const*, void const*))
        #06 0x7b52ce80dc /apex/com.android.runtime/bin/linker64+380dc(__dl___loader_android_dlopen_ext)
        #07 0x7b4df5b0b8 /apex/com.android.runtime/lib64/bionic/libdl.so+10b8
        #08 0x7b4e2dcfd8 /apex/com.android.runtime/lib64/libnativeloader.so+7fd8
        #09 0x7b4e2dcc5c /apex/com.android.runtime/lib64/libnativeloader.so+7c5c
        #10 0x7acb748938 /apex/com.android.runtime/lib64/libart.so+37b938
        #11 0x7ac16e10e0 /apex/com.android.runtime/lib64/libopenjdkjvm.so+50e0       
```

### 补全hook信息

```shell
[18391.525011] [info]kr_mmap mmap file libsotest.so. vaddr=0x7b5195a000, len=0x1d0 prot=1, off=0x0
[18391.525027] [info]kr_mmap mmap file libsotest.so. vaddr=0x7b51959000, len=0xae0 prot=1, off=0x35000
[18391.525055] [info]kr_mmap mmap file libsotest.so. vaddr=0x7b51957000, len=0x1ca0 prot=1, off=0x34000
[18391.525069] [info]kr_mmap mmap file libsotest.so. vaddr=0x7b51956000, len=0xaeb prot=1, off=0x0
[18391.525315] [info]kr_mmap mmap file libsotest.so. vaddr=0x7abcd16000, len=0x34988 prot=5, off=0x0
[18391.525343] [info]kr_mmap mmap file libsotest.so. vaddr=0x7abcd5a000, len=0x11f8 prot=3, off=0x34000 
[18391.525629] [20211115_17:03:46.098714]@6 [info]kr_mprotect start=0x7abcd5a000 len=0x1000 prot=1(关键信息)
[18391.525670] [20211115_17:03:46.098754]@6 [info]kr_mprotect start=0x7b51955000 len=0x1000 prot=1
[18391.525769] [20211115_17:03:46.098853]@6 [info]kr_mprotect start=0x7b51b82000 len=0x64000 prot=1
[18391.525851] [20211115_17:03:46.098936]@6 [info]kr_mprotect start=0x7b51c3a000 len=0x64000 prot=1
[18391.525933] [20211115_17:03:46.099017]@6 [info]kr_mprotect start=0x7b51aeb000 len=0x64000 prot=1
[18391.526019] [20211115_17:03:46.099104]@6 [info]kr_mprotect start=0x7b51a87000 len=0x64000 prot=1
[18391.533599] [20211115_17:03:46.106682]@6 [info]kr_mprotect start=0x7a63101000 len=0x2c000 prot=3
[18391.533950] [20211115_17:03:46.107034]@6 [info]kr_mprotect start=0x7a63101000 len=0x1a2b8 prot=5
[18391.534488] [20211115_17:03:46.107573]@6 [info]kr_mprotect start=0x7a63132000 len=0x12000 prot=3
[18391.534601] [20211115_17:03:46.107685]@6 [info]kr_mprotect start=0x7a63132000 len=0xa78 prot=5
[18391.534683] [20211115_17:03:46.107768]@6 [info]kr_mprotect start=0x7a63101000 len=0x2c000 prot=3
```

第二次采集的时候通过hook补全了mprotect权限, 发现为了一行关键信息. linker使用mprotect进行了权限修改, 因此内核层便进行了maps分割

这里简单画了下对应信息, 方便能够理解背后的行为.

![20211116191000](https://cdn.jsdelivr.net/gh/yhnu/PicBed/20211116191000.png)

通过上面的分析的,便解答了心中的疑惑. 那么我们进一步思考linker为什么需要进行上面的行为呢? 这个问题我们就需要去linker源码里面寻找答案了, 

堆栈信息已经给我们提供了阅读代码的线索, 我们也就可以去大海里遨游不怕迷路了.

### linker LoadSegments详细分析

ElfReader::ReadProgramHeaders 进行mmap, 在析构函数进行 munmap

ElfReader::ReadSectionHeaders 后续再分析

ElfReader::ReadDynamicSection 后续再分析

ElfReader::ReadDynamicSection 后续再分析

ElfReader::LoadSegments 这里进行一下详细阐述, linker其实是操作系统和elf的连接器, 既要满足elf的要求, 又要满足mmap的一些限制. 

比如:

mmap需要按页分配, elf需要对齐

下图详细描述了虚拟地址空间映射和so文件之间的关系

![linker](https://cdn.jsdelivr.net/gh/yhnu/PicBed/20211122090406.png)

因为mmap时使用但是PAGE_START(phdr->p_offset), elf的fileoffset是0x34D18, 向上页对齐便是0x34000. 心里的疑惑也就解开了.

### bss段在什么地方?

.bss 也叫做全局变量段

![20211117100643](https://cdn.jsdelivr.net/gh/yhnu/PicBed/20211117100643.png)

自己也做了对应的实验,  开辟一个非常大的全局变量. 其中memsize和

![20211122104726](https://cdn.jsdelivr.net/gh/yhnu/PicBed/20211122104726.png)

### mprotect由谁调用的?

```shell
[13208.302913] [20211117_15:08:06.310817]@5 [info]kr_mmap mmap file libsotest.so. vaddr=0x7e57126000, len=0x1d0 prot=1, off=0x0
[13208.302926] [20211117_15:08:06.310830]@5 [info]kr_mmap mmap file libsotest.so. vaddr=0x7e57125000, len=0xae0 prot=1, off=0x35000
[13208.302949] [20211117_15:08:06.310852]@5 [info]kr_mmap mmap file libsotest.so. vaddr=0x7e57123000, len=0x1ca0 prot=1, off=0x34000
[13208.302959] [20211117_15:08:06.310863]@5 [info]kr_mmap mmap file libsotest.so. vaddr=0x7e57122000, len=0xaeb prot=1, off=0x0
[13208.303170] [20211117_15:08:06.311073]@5 [info]kr_mmap mmap file libsotest.so. vaddr=0x7dc2382000, len=0x34988 prot=5, off=0x0
[13208.303185] [20211117_15:08:06.311088]@5 [info]kr_mmap mmap file libsotest.so. vaddr=0x7dc23c6000, len=0x11f8 prot=3, off=0x34000
[13208.303481] [20211117_15:08:06.311385]@5 [info]kr_mprotect start=0x7dc23c6000 len=0x1000 prot=1 tgid=13596(由谁触发?)
[13208.303536] [20211117_15:08:06.311440]@5 [info]kr_mprotect start=0x7e57121000 len=0x1000 prot=1 tgid=13596
```

```shell
kr_gid tgid=13596 path=/proc/13596/maps mtim=1637132869
kr_tag start=0x7dc23c6000
	pc=0x7e58578e78 sp=0x7ffd9dd960 c->sp=0x7ffd9dd960 c->pc=0x7e58578e78 regs_len=320 stack_len=9888 tag_len=1024
	#00 0x7e58578e78 /apex/com.android.runtime/bin/linker64+fce78(__dl_mprotect)
	#01 0x7e584cd160 /apex/com.android.runtime/bin/linker64+51160(__dlphdr_table_protect_gnu_relro(elf64_phdr const*, unsigned long, unsigned long long))
	#02 0x7e584bc7f8 /apex/com.android.runtime/bin/linker64+407f8(__dlsoinfo::protect_relro())
	#03 0x7e584baff0 /apex/com.android.runtime/bin/linker64+3eff0(__dlsoinfo::link_image(LinkedList<soinfo, SoinfoListAllocator> const&, LinkedList<soinfo, SoinfoListAllocator> const&, android_dlextinfo const*, unsigned long*))
	#04 0x7e584b6324 /apex/com.android.runtime/bin/linker64+3a324(__dlfind_libraries(android_namespace_t*, soinfo*, char const* const*, unsigned long, soinfo**, std::__1::vector<soinfo*, std::__1::allocator<soinfo*> >*, unsigned long, int, android_dlextinfo const*, bool, bool, std::__1::vector<android_namespace_t*, std::__1::allocator<android_namespace_t*> >*))
	#05 0x7e584b8ba0 /apex/com.android.runtime/bin/linker64+3cba0(__dldo_dlopen(char const*, int, android_dlextinfo const*, void const*))
	#06 0x7e584b40dc /apex/com.android.runtime/bin/linker64+380dc(__dl___loader_android_dlopen_ext)
	#07 0x7e550790b8 /apex/com.android.runtime/lib64/bionic/libdl.so+10b8
	#08 0x7e55024fd8 /apex/com.android.runtime/lib64/libnativeloader.so+7fd8
	#09 0x7e55024c5c /apex/com.android.runtime/lib64/libnativeloader.so+7c5c
	#10 0x7dd0e10938 /apex/com.android.runtime/lib64/libart.so+37b938
	#11 0x7dc6ea60e0 /apex/com.android.runtime/lib64/libopenjdkjvm.so+50e0
	#12 0x71e9eaf4 /system/framework/arm64/boot.oat+b9af4
	#13 0x71ebbcac /system/framework/arm64/boot.oat+d6cac
	#14 0x71ebcc94 /system/framework/arm64/boot.oat+d7c94
	#15 0x71ec1870 /system/framework/arm64/boot.oat+dc870
```

![linker](https://cdn.jsdelivr.net/gh/yhnu/PicBed/20211117151714.png)

### 为什么需要使用mprotect?

![20211122112623](https://cdn.jsdelivr.net/gh/yhnu/PicBed/20211122112623.png)

因为elf里面有个对应Read-only After Relocation段(R__) GNU Read-only After Relocation, 因此linker需要在进行relocation后进行只读权限设置, 因此使用mprotect


### 其他比较好的讲解

[liner底层原理](https://wooyun.js.org/drops/Android%20Linker%E5%AD%A6%E4%B9%A0%E7%AC%94%E8%AE%B0.html)
