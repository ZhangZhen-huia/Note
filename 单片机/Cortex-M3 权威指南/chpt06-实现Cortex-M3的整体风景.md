# 流水线

Cortex‐M3 处理器使用一个 3 级流水线。流水线的 3 级分别是：取指，解码和执行

![image-20250402090606775](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504020906804.png)

# CM3详细框图

![image-20250402090658398](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504020906457.png)

- MPU：存储器保护单元，可选，它可以把存储器分成一些 regions，并分别予以保护。例如，它可以让 某些 regions 在用户级下变成只读，从而阻止了一些用户程序破坏关键数据
- NVIC：嵌套向量中断控制器 ，在中断发生时，它会自动取出对应的服务例程入口地址， 并且直接调用，无需软件判定中断源

# 总线接口

## I-Code总线

> 基于 AHB‐Lite 总线协议的 32 位总线，负责在 0x0000_0000 – 0x1FFF_FFFF 之间的取指操作，取指以字的长度执行，即使是对于 16 位指令也如此。因此 CPU 内核可以一次取出两条 16 位 Thumb 指令

## D-Code总线

> 基于 AHB‐Lite 总线协议的 32 位总线，负责在 0x0000_0000 – 0x1FFF_FFFF 之间的数据访问操作，连接到 D‐Code 总线上的任何设备都只需支持 AHB‐Lite 的对齐访问，不需要支 持非对齐访问

## 系统总线

> 基于 AHB‐Lite 总线协议的 32 位总线，负责在 0x2000_0000 – 0xDFFF_FFFF 和 0xE010_0000 – 0xFFFF_FFFF 之间的所有数据传送，取指和数据访问都算上。 和 D‐Code 总线一样，所有的数据传送都是对齐的

## 外部私有外设总线

> 基于 APB 总线协议的 32 位总线，负责 0xE004_0000 – 0xE00F_FFFF 之间的私有外设访问，由于此 APB 存储空间的一部分已经被 TPIU、ETM 以及 ROM 表用 掉了，就只留下了 0xE004_2000‐E00F_F000 这个区间用于配接附加的（私有）外设

## 调试访问端口总线

> 基于“增强型 APB 规格”的 32 位总线，它专用于挂接调 试接口，例如 SWJ‐DP 和 SW‐DP

# CM3其他接口

> ![image-20250402091808208](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504020918260.png)

# 总线连接示例

![image-20250402092110994](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504020921042.png)

# 复位信号

![image-20250402092329158](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504020923191.png)

![image-20250402092335303](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504020923339.png)