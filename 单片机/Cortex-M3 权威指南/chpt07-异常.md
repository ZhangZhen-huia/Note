# 异常类型

- 1－15 的对应系统异常
- 大于等于 16 的则全是外部中断
- 个别异常的优先级被定死，其余都可编程
- 编号为0表示没有异常

> 系统异常
>
> ![image-20250402092723686](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504020927740.png)
>
> - NMI：由外部引脚的电平状态引发

> 外部中断
>
> ![image-20250402093023885](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504020930917.png)

在 NVIC 的中断控制及状态寄存器中，有一个 VECTACTIVE 位段；另外，还有一个特殊功 能寄存器 IPSR。在它们二者的里面，都记录了当前正服务异常的编号

![image-20250402093416823](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504020934296.png)

![image-20250402093406724](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504020934755.png)

![image-20250402093452478](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504020934524.png)

## 悬起

如果一个发生的异常不能被即刻响应，就称它被“悬起”(pending)

一个异常被悬起的原因，可能是系统当前正在执行一个更高优先级异常的服务例程，或者因相关掩蔽位的设置导致该异常被除能。对于每个异常源，在被悬起的情况下，都会有一个对应的“**悬起状态寄存器**”保存其异常请求，直到该异常能够执行为止

# 优先级

- 优先级的数值越小，则优先级越高
- 支持中断嵌套，使得高优先级异常会抢占低优先级异常
- 复位，NMI，硬Fault优先级是固定的，且都是负数
- 支持抢占优先级和亚优先级
- 抢占优先级决定了抢占行为：当系统正在响应某异常 L 时，如果来了抢占优先级更高的 异常 H，则 H 可以抢占 L。亚优先级则处理“内务”：当抢占优先级相同的异常有不止一个 悬起时，就优先响应亚优先级最高的异常

# 中断输入及悬起行为 

- 当中断输入脚被 assert 后，该中断就被悬起。即使后来中断源取消了中断请求，已经被 标记成悬起的中断也被记录下来。到了系统中它的优先级最高的时候，就会得到响应，但是，如果在某个中断得到响应之前，其悬起状态被清除了（例如，在 PRIMASK 或 FAULTMASK 置位的时候软件清除了悬起状态标志），则中断被取消

- > ![image-20250402121843347](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504021218389.png)

- 当某中断的服务例程开始执行时，就称此中断进入了“活跃”状态，并且其悬起位会被 硬件自动清除

- > ![image-20250402121923814](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504021219863.png)

- 如果中断源咬住请求信号不放，该中断就会在其上次服务例程返回后再次被置为悬起状态

- > ![image-20250402121944397](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504021219448.png)

- 如果某个中断在得到响应之前，其请求信号以若干的脉冲的方式呈现，则被 视为只有一次中断请求，多出的请求脉冲全部错失——这是中断请求太快，以致于超出处理 器反应限度的情况

- > ![image-20250402122006393](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504021220442.png)

- 如果在服务例程执行时，中断请求释放了，但是在服务例程返回前又重新被置为有效， 则 CM3 会记住此动作，重新悬起该中断

- > ![image-20250402122026760](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504021220800.png)

# Fault类异常

- 总线Faults
- 存储器管理Faults
- 用法Faults
- 硬Faults

## 总线Faults

当 AHB 接口上正在传送数据时，如果回复了一个错误信号(error response)，则会产生总 线 faults

> ![image-20250402122838477](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504021228525.png)

正常情况下总线fault发生后会执行总线fault的服务历程，如果总线 fault 被除能，或者总线 fault 发生时正在处理同级或更高优先级异常，则总线 fault 被迫成为 “硬伤”——上访成硬 fault，使得最后执行的是硬 fault 的服务例程

> 如果在硬 fault 服务例程的执行中又 产生了总线 fault（太钻牛角尖了），内核将进入锁定状态

使能总线fault，在使能之前，总线 fault 服务例程的入口地址必须已经在向 量表中配置好，否则程序可能跑飞

![image-20250402123107084](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504021231111.png)

NVIC 提供了若干个 fault 状态寄存器，其中一个名为“总线 fault 状态寄存器”(BFSR)的。通过它，总 线 fault 服务例程可以确定产生 fault 的场合：是在数据访问时，在取指时，还是在中断的堆栈操作时

![image-20250402123321214](C:\Users\a1874\AppData\Roaming\Typora\typora-user-images\image-20250402123321214.png)

![image-20250402123710997](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504021237045.png)

### 精确的总线Faults和不精确的总线Faults

> ![image-20250402123619509](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504021236564.png)
>
> 对于精确的总线 fault，肇事的指令的地址被压在堆栈中。如果 BFSR 中 的 BFARVALID 位为 1，还可以找出是在访问哪块存储器时产生该总线 fault 的——该存储器 的地址被放到“总线 fault 地址寄存器（BFAR）”中

## 存储器管理 Faults 

- 访问了 MPU 设置区域覆盖范围之外的地址
- 往只读的区域写入数据
- 用户级下访问了只允许在特权级下访问的地址
- 使能之前应优先建立好 MemManage fault 服务例程的入口地址

![image-20250402124002397](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504021240427.png)

> 在 MemManage fault 发生后，如果其服务例程是使能的，则执行服务例程。如果同时还 发生了其它高优先级异常，则优先处理这些高优先级的异常，MemManage 异常被悬起。如 果此时处理器已经在处理同级或高优先级异常，或者 MemManage fault 服务例程被除能，则 和总线 fault 一样：上访成硬 fault，最终执行的是硬 fault 的服务例程。如果硬 fault 服务例 程或 NMI 服务例程的执行也导致了 MemManage fault，那内核将被锁定
>
> 这一过程和总线Fault一样

### 存储器管理 fault 状态寄存器(MFSR)

![image-20250402124141100](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504021241146.png)

## 用法Fault

- 执行了未定义的指令
- 执行了协处理器指令
- 尝试进入 ARM 状态
- 无效的中断返回（LR 中包含了无效/错误的值）
- 使用多重加载/存储指令时，地址没有对齐
- 通过设置 NVIC 的对应控制位，可以在下列场合下也产生用法 fault：
  - 除数为零
  - 任何未对齐的访问

![image-20250402124509984](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504021245013.png)

- 和前两个Fault执行流程一样

![image-20250402124800585](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504021248624.png)

## 硬Fault

在 NVIC 中有一个硬 fault 状态寄存器（HFSR），它指出产生硬 fault 的原因。如果不是由于取向 量造成的，则硬 fault 服务例程必须检查其它的 fault 状态寄存器，以最终决定是谁上访的

![image-20250402182522930](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504021825973.png)

## 应对Fault

![image-20250402182814857](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504021828886.png)

两个都只复位处理器内核而不复位其它片上设施

![image-20250402182909557](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504021829588.png)

***各个 fault 状态寄存器(FSRs)都保持住它们的状态，直到手工清除，所以在fault中断服务例程中处理完相应的fault后记得清除状态寄存器，FSRs 采用一个写时清除机制（写 1 时清除）***

# SVC和PendSV

## SVC

系统服务调用，亦简称系统调用

当用户程序想要控制特定的硬件时，它就会产生一个 SVC 异常，然后操作系统提供的 SVC 异常服务例程得到执行，它再 调用相关的操作系统函数，后者完成用户程序请求的服务

SVC 异常是必须立即得到响应的（若因优先级不比当前正处理的高，或是其它原因使之无法立即 响应，将上访成硬 fault——译者注），应用程序执行 SVC 时都是希望所需的请求立即得到响 应

![image-20250402220019291](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504022200334.png)

## PendSV

可悬起的系统调用

悬起 PendSV 的方法是：手工往 NVIC 的 PendSV 悬起寄存器中写 1。悬起后，如果优先级不够 高，则将缓期等待执行

PendSV典型应用于上下文切换

- 执行了一次系统调用
- systick中断中任务流转的需要

PendSV 异常会自动延迟上下文切换的请求， 直到其它的 ISR 都完成了处理后才放行，所以他的优先级为最低优先级

![image-20250402221438843](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504022214901.png)