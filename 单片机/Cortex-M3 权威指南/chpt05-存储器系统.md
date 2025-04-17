# 存储器功能概览

> CM3 的存储器系统与从传统 ARM 架构的相比，已经脱胎换骨了： 
>
> - 第一， 它的存储器映射是预定义的，并且还规定好了哪个位置使用哪条总线。 
> - 第二， CM3 的存储器系统支持所谓的“位带”（bit‐band）操作。通过它，实现了对单 一比特的原子操作。位带操作仅适用于一些特殊的存储器区域中，见本章论述。 
> - 第三， CM3 的存储器系统支持非对齐访问和互斥访问。这两个特性是直到了 v7M 时才 出来的。 最后，CM3 的存储器系统支持 both 小端配置和大端配置

# 存储器映射

存储空间的一些位置用于调试组件等私有外设，这个地址段称之为“私有外设区”，组件包括：

- FPB： 闪存地址重载及断点单元
- DWT：数据观察点单元
- ITM：指令跟踪宏单元
- ETM：嵌入式跟踪宏单元
- TPIU：跟踪端口接口单元
- ROM表

***CM3的地址空间是4GB(32位地址 = 2^32,2^32/1024/1024/1024 = 4GB(KB,MB,GB))***

程序可以在代码区，内部 SRAM 区以及外部 RAM 区中执行

- 位带操作只适 用于数据访问，不适用于取指

![image-20250318095329129](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503180953183.png)

![image-20250318095656956](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503180956991.png)

0.5GB的隐密地带

包括了系统级组件，内部私有外设总线 s，外部私有外设总线 s，以及由提供者定义的系统外设

两条私有外设总线

- APB：既用于 CM3 内部的 APB 设备，也用于外部设备（这里的“外部”是对内核而言）。CM3 允许器件制造商再添加一些片上 APB外设到 APB 私有总 线上，它们通过 ABP 接口来访问
- AHB:只用于 CM3 内部的 AHB 外设，它们是：NVIC, FPB, DWT 和 ITM

NVIC所在的区域叫做“系统控制空间（SCS）”，在SCS中还存在Systick，MPU，以及代码调试控制所用的寄存器

![image-20250318100117270](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503181001302.png)

> CM3 中的 MPU 是选配的，由芯片制造商决定是否配上

# 存储器访问属性s

- 可否缓冲(Bufferable) 
- 可否缓存(Cacheable) 
- 可否执行(Executable) 
- 可否共享(Sharable)

MPU可以配置不同的存储器并覆盖缺省的访问属性

地址空间可以通过另一种方式分为 8 个 512MB 等份

1. 代码区（0x00000000 - 0x1FFFFFFF）:可执行指令，缓存属性为WT（写通），即不可缓存，数据操作是通过数据总线接口的（读数据使用 D‐Code，写数据使用 System），且在此区上的写操作是缓冲的
2. SRAM区（0x20000000 - 0x3FFFFFFF）:片内RAM，写操作是缓冲的，常用于固件升级等维护工作
3. 片上外设区（0x40000000 – 0x5FFFFFFF）：不可缓存，不可以在此区执行指令
4. 外部 RAM 区的前半段（0x60000000 ‐ 0x7FFFFFFF）：片外 RAM，可缓存（缓存属性为 WB‐WA），并且可以执行指令
5. 外部 RAM 区的后半段（0x80000000 - 0x9FFFFFFF）:不可缓存(WT),其他同前半段
6. 外部外设区的前半段(0xA0000000 – 0xBFFFFFFF)：片外外设的寄存器，也用于多核系统中的共享内存，不可缓冲，不可执行指令
7. 外部外设区的后半段(0xC0000000 – 0xDFFFFFFF):与前半段的功能完全一致
8. 系统区(0xE0000000 – 0xFFFFFFFF):不可执行代码,不可缓存，不可缓冲

> - 写回（Write Back)：写入的数据先存入缓存中，等到有需要的时候再写入最终目的地，这也是 cache 的最基本职能，用于改善数据传送的效率
> - 写通(Write Through)：写入的数据直接写入最终目的地，所以架空了cache，这常用于和片上外设或其它处理器共享的内 存中，如显卡的显存，片上外设寄存器，以及双核系统中的共享内存，写通操作和 C 中的“volatile” 有密切的联系

# 存储器缺省访问许可

当没有配备MPU或者MPU被失能的时候缺省条件才会生效

缺省访问许可如下

![image-20250318102400753](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503181024802.png)

如果MPU有效，则MPU可以在地址空间中划分出若干个regions，并为不同的 region规定不同的访问许可权限

# 位带操作

![image-20250325011335505](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503250113555.png)

在CM3中存在两个位带区

- 片内SRAM最低1MB的范围，0x2000_0000‐0x200F_FFFF
- 片内外设区最低1MB的范围，0x4000_0000‐0x400F_FFFF

位带别名区：片内SRAM和片内外设的1MB的范围把每一个位都膨胀成32位的字

> ![image-20250325010407611](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503250104779.png)

0xFFFFF = 2^20 / 1024 /1024 = 1MB

一个字节的八位扩展到28个字节

> ![image-20250325011209483](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503250112538.png)

## 别名地址转换

×4表示一个字为 4 个字节，×8表示一个字节中有 8 个比特。

### SRAM

![image-20250325011537412](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503250115439.png)

### 片内外设

![image-20250325011547511](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503250115539.png)

## 位带操作优势

CM3 把这个“读－改－写” 做成一个硬件级别支持的原子操作，不能被中断，也就实现了互斥访问

## 位带操作使用

```c
//把“位带地址＋位序号”转换成别名地址的宏
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr & 0xFFFFF)<<5)+(bitnum<<2))
//把该地址转换成指针
#define MEM_ADDR(addr) *((volatile unsigned long *) (adr))
```

> ![image-20250402085929017](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504020859734.png)

- 当你使用位带功能时，要访问的变量必须用 volatile 来定义

- > ![](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504020905582.png)

