# NVIC

向量中断控制器，简称NVIC

- 支持1 ~ 240个外部中断输入，通常外部中断写作 IRQs
- 支持一个不可屏蔽中断NMI
-  访问地址为0xE000_E000
- 所有 NVIC 的中断控制/状态寄存器都只能在特权级下访问，软件触发中断寄存器可以在用户级下访问以产生软件中断
- 有几个中断屏蔽寄存器 也与中断控制密切相关，他们是“特殊功能寄存器”，只能通过 MRS/MSR 及 CPS 来访问

# 中断配置基础

- NVIC_ISERx——使能与失能寄存器

- NVIC_ISPRx——悬起与解悬寄存器

- NVIC_IPRx——优先级寄存器

- NVIC_IABRx——活动状态寄存器

- > ![image-20250403120922450](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504031209500.png)

# 中断的使能与除能

中断的使能与除能分别使用各自的寄存器来控制

CM3 中可以有 240 对使能位／除能位，每个中断拥有一 对。这 240 个对子分布在 8 对 32 位寄存器中

- 使能一个中断就往对应的SETENA位中写入1，写0没有效果
- 失能一个中断就往对应的CLRENA位中写入1，写0没有效果
- 能按字／半字／字节访问

![image-20250403123352207](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504031233253.png)

# 中断的悬起与解悬

中断的悬起状态可以通过寄存器来读取，还可以写它们来手工悬起中断

- 中断设置悬起寄存器(SETPEND)
- 中断悬起清除寄存器(CLRPEND)
- 能按字／半字／字节访问

![image-20250403123822320](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504031238363.png)

# 优先级

每个外部中断都有一个对应的优先级寄存器，每个寄存器占用 8 位，但是允许最少只使用最高 3 位

能按字／半字／字节访问

![image-20250403124838103](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504031248134.png)

![image-20250403124848292](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504031248335.png)

# 活动控制

- 每个外部中断都有一个活动状态位。在处理器执行了其 ISR 的第一条指令后，它的活动 位就被置 1，并且直到 **ISR 返回（所以被高优先级抢占了也不会置0）**时才硬件清零
- 能按字／半字／字节访问
- 只读寄存器

![image-20250403125219969](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504031252006.png)

# 特殊功能寄存器

## PRIMASK

 用于除能在 NMI 和硬 fault 之外的所有异常，它有效地把当前优先级改为 0

![image-20250403130041181](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504031300208.png)

## FAULTMASK

更绝，它把当前优先级改为‐1。这么一来，连硬fault都被掩蔽了，但是掩蔽不了NMI，使用方案与PRIMASK的相似。但要注意的是，**FAULTMASK会在异常退出时自动清零**

## BASEPRI

- 只掩蔽优先级低于某一阈值的中断——它们的优先级在数字上大于等于某个数
- 如果往BASEPRI中写0，BASEPRI将停止掩蔽任何中断

> ![image-20250403130238895](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504031302926.png)

> 还可以使用***BASEPRI_MAX***这个名字来访问BASEPRI寄存器，不过使用BASEPRI_MAX时只能一次次地扩大掩蔽范围，即写入的值必须小于里面原来的值
>
> ![image-20250403130351117](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504031303152.png)

# 其它异常的配置寄存器

用法Fault，总线Fault，存储器管理Fault的使能都是通过***系统Handler控制及状态寄存器(SHCSR)***来设置的

![image-20250403145530614](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504031455661.png)

# ICSR寄存器

***对于NMI、SysTick定时器以及PendSV，可以通过此寄存器手工悬起它们***

![image-20250403145657277](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504031456329.png)

# 软件中断

![image-20250403150439329](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504031504367.png)

> 注意：系统异常（NMI，faults，PendSV等），不能用此法悬起。而且缺省时就不允许用 户程序改动NVIC寄存器的值。如果确实需要，必须先在NVIC的配置和控制寄存器 (0xE000_ED14)中，把比特1（USERSETMPEND）置位，才能允许用户级下访问NVIC的STIR

# SysTick定时器 

操作系统“心跳”的产生者，SysTick定时器被捆绑在NVIC中，用于产生SYSTICK异常

该定时器的时钟源可以是内部时钟（FCLK， CM3上的自由运行时钟），或者是外部时钟（ CM3处理器上的STCLK信号）

## 寄存器

### SysTick控制及状态寄存器0xE000_E010

![image-20250403150823181](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504031508222.png)

### SysTick重装载数值寄存器0xE000_E014

![image-20250403150845998](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504031508027.png)

### SysTick当前数值寄存器0xE000_E018

![image-20250403150906994](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504031509029.png)

### SysTick校准数值寄存器0xE000_E01C

![image-20250403150918914](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504031509958.png)

> 校准值寄存器提供了这样一个解决方案：它使系统即使在不同的CM3产品上运行，也能 产生恒定的SysTick中断频率。最简单的作法就是：直接把TENMS的值写入重装载寄存器，这 样一来，只要没突破系统极限，就能做到每10ms来一次 SysTick异常。如果需要其它的SysTick 异常周期，则可以根据TENMS的值加以比例计算。只不过，在少数情况下，CM3芯片可能无 法准确地提供TENMS的值（如，CM3的校准输入信号被拉低），所以为保险起见，最好在使 用TENMS前检查器件的参考手册
