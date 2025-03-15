# 寄存器组

#### 通用目的寄存器 R0-R7

低组寄存器，所有指令都可以访问他们，字长位32位，复位后的初始值是不可预料的

#### 通用目的寄存器 R8-R12

高组寄存器，32位字长，复位后的初始值是不可预料的

![image-20250305194842262](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503051948343.png)

#### 堆栈指针 R13

当引用 R13（或写作 SP）时，你引用到的是当前正在使用的那一个，另一个必须用特殊的指 令来访问（MRS,MSR 指令）

- 主堆栈指针MSP，或写作 SP_main，它由 OS 内核、异常服务 例程以及所有需要特权访问的应用程序代码来使用。

- 进程堆栈指针PSP，或写作SP_process 用于常规的应用程序代码（不处于异常服用例程中时）

堆栈指针用于访问堆栈，并且 PUSH 指令和 POP 指令默认使用 SP

> 堆栈是一种存储器的使用模型。它由一块连续的内存，以及一个栈顶指针组成，用 于实现“**先进后出（FILO）**”的缓冲区。其最典型的应用，就是在数据处理前先保存寄存器的值， 再在处理任务完成后从中恢复先前保护的这些值。 
>
> 在执行 PUSH 和 POP 操作时，那个通常被称为 SP 的地址寄存器，会自动被调整， 以避免后续的操作破坏先前的数据
>
> ![image-20250305195730996](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503051957041.png)

PUSH和POP的汇编和C语言表达

![image-20250305195913895](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503051959925.png)

**

> [!IMPORTANT]
>
> ***向下生长的满栈***
>
> 在 PUSH 新 数据时，堆栈指针先减一个单元。通常在进入一个子程序后，第一件事就是把寄存器的值先 PUSH 入堆栈中，在子程序退出前再 POP 曾经 PUSH 的那些寄存器。另外，PUSH 和 POP 还 能一次操作多个寄存器

![image-20250305200157634](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503052001666.png)

#### 连接存储器 R14（LR）

> LR用于再调用子程序时存储返回地址，当你在使用 BL(分支并连接，Branch and Link)指令时， 就自动填充 LR 的值
>
> 先让pc等于function1函数的地址，然后再把function1退出后下一步要执行的地址存入LR寄存器中，用于function1函数的返回
>
> ![image-20250305200554775](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503052005815.png)

BX指令用于跳转，BX LR 表示跳转到LR寄存器内保存的地址，即跳转到function1函数的下一句代码

#### 程序计数器 R15

> 在汇编代码中可以用 ”PC“ 来访问他，CM3 内部 使用了指令流水线，读 PC 时返回的值是当前指令的地址+4，PC寄存器指向的地址的值就是下一步要执行的地址
>
> ![image-20250305201323035](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503052013065.png)

#### 特殊功能寄存器组

- 程序状态寄存器PSRs或xPSR
- 中断屏蔽寄存器组 PRIMASK，FAULTMASK，BASEPRI
- 控制寄存器 CONTROL

> 他们只能被专用的MSR（向寄存器中写值）和MRS（从寄存器中读取值）指令访问，而且他们也没有存储器地址
>
> ![image-20250308220728513](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503082207607.png)

###### 程序状态寄存器

> 程序状态寄存器在其内部又被分为三个子状态寄存器： 
>
> -  应用程序 PSR（APSR） 
> -  中断号 PSR（IPSR） 
> - 执行 PSR（EPSR）
> - ![image-20250308221024062](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503082210113.png)

###### 中断屏蔽寄存器组

- PRIMASK：置1，只剩下NMI 和 硬fault可以响应
-  FAULTMASK ：置1，只有NMI才可以响应
- BASEPRI：定义被屏蔽优先级的阈值

![image-20250308221203610](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503082212656.png)

要读取这些寄存器同样要使用MSR/MRS指令，***且只有在特权级下，才允许访问这 3 个寄存器***

为了快速的开关中断，cm3还专门设置了一条CPS指令（再FreeRtos的临界段里面有应用）

![image-20250308222849797](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503082228826.png)

###### 控制寄存器

控制寄存器用于定义特权级别，还用于选择当前使用哪个堆栈指针

> ![image-20250308223030375](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503082230416.png)

# 操作模式

2 个模式和两个特权等级

![image-20250308223151398](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503082231430.png)

# 异常与中断

> Cortex‐M3 支持大量异常，包括 16‐4‐1=11 个系统异常，和最多 240 个外部中断——简称 IRQ。具体使用了这 240 个中断源中的多少个，则由芯片制造商决定。由外设产生的中断信号，除了 SysTick 的之外，全都连接到 NVIC 的中断输入信号线。典型情况下，处理器一般支持 16 到 32 个中断，当然也有在此之外的。

> 在多数情况下，NMI 会被连接到一个看门狗定时器，有时也会是电压 监视功能块，以便在电压掉至危险级别后警告处理器。NMI 可以在任何时间被激活，甚至是 在处理器刚刚复位之后

![image-20250308223507645](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503082235708.png)

# 向量表S

> 当一个发生的异常被 CM3 内核接受，对应的异常 handler 就会执行。为了决定 handler 的入口地址，CM3 使用了“向量表查表机制”。

> 向量表其实是一个 WORD（32 位整数）数组，每个下标对应一种异常，该下标元素的值则是该异常 handler 的入口地址。向量表的存储位置是可以设置的，通过 NVIC 中的一个重定位寄存器来指出向量表的地址。在复位后，该寄存器的值为 0。因此，在地址 0 处必须包含一张向量表，用于初始时的
> 异常分配![image-20250309144221432](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503091442480.png)
> 
> ![image-20250309144231170](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503091442211.png)

# 栈内存操作

> 在 Cortex‐M3 中，除了可以使用 PUSH 和 POP 指令来处理堆栈外，内核还会在异常处理 的始末自动地执行 PUSH 与 POP 操作
>
> - 堆栈操作就是对内存的读写操作
> - 把寄存器的数据放入内存，以便将来能恢复之——当一个任务或一段子 程序执行完毕后恢复
> - 正常情况下，PUSH 与 POP 必须成对使用，而且参与的寄存器，不论 是身份还是先后顺序都必须完全一致
> - 当 PUSH/POP 指令执行时，SP 指针的值也根着自减/ 自增。
> - PUSH/POP 指令支持一次操作多个寄存器
> - ![image-20250309145033675](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503091450708.png)
> - 不管在寄存器列表中，寄存器的序号是以什么顺序给出的，汇编器都将把它们升序排序。然后 PUSH 指令按照从大到小的顺序依次入栈，POP 则按从小到大的顺序依次出栈。 如果不按升序写寄存器，有些汇编器可能会给出一个语法错误
> - PUSH 指令等效于与使用 R13 作为地址指针的 STMDB 指令，而 POP 指令则等效于使用 R13 作为地址指针的 LDMIA 指令——STMDB/LDMIA 还可以使用其它寄存器作为地址指针

# Cortex-M3 堆栈的实现

Cortex‐M3 使用的是“向下生长的满栈”模型。堆栈指针 SP 指向最后一个被压入堆栈的 32 位数值。在下一次压栈时，SP 先自减 4，再存入新的数值（虽然 POP 后被压入的数值还保存在栈中，但它已经无效了，因为为下次的 PUSH 将覆盖它的值）

> ![image-20250309145317605](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503091453644.png)
>
> ![image-20250309145337223](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503091453260.png)

# Cortex-M3 的双堆栈机制

由CONTROL[1]决定如何选择使用 	***主堆栈或者进程堆栈***

- CONTROL[1] = 0，只用MSP,此时用户程序和异常 handler 共享同一个堆栈。这 也是复位后的缺省使用方式
- CONTROL[1] = 1，线程模式用PSP，Handler模式用MSP
- 在特权级下，可以指定具体的堆栈指针，而不受当前使用堆栈的限制

# 复位序列

芯片在复位后，CM3会读取下列两个32位整数的值

- 从地址 0x0000,0000 处取出 MSP 的初始值

- 从地址 0x0000,0004 处取出 PC 的初始值——这个值是复位向量，LSB 必须是 1。然 后从这个值所对应的地址处取指

- > ![image-20250309150557745](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503091505783.png)

- 因为CM3 使用的是向下生长的满栈，所以MSP的初始值必须是堆栈内存的末地址加 1

![image-20250309150352902](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503091503960.png)
