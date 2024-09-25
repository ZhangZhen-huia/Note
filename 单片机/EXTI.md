# 中断应用总结

Stm32 非常强大，每一个外设都可以产生中断，没有特别说明，异常=中断

![image-20240813210108705](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408132101728.png)



**中断向量表**，在启动文件中也有保存(下图是一部分)

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408132102756.png" alt="image-20240813210214699" style="zoom:67%;" />

**<u>*NVIC:*</u>**

嵌套向量中断控制器, 属于内核外设, 支持256个中断(16个内核中断, 240个外部中断), 最多支持256个优先级(可以剪裁), ST只使用了16个Core_cm3.h   misc.h中有如下结构体

```c
typedef struct
{
  __IO uint32_t ISER[8];                      /*!< Offset: 0x000  Interrupt 使能寄存器           */
       uint32_t RESERVED0[24];                                   
  __IO uint32_t ICER[8];                      /*!< Offset: 0x080  Interrupt 清除         */
       uint32_t RSERVED1[24];                                    
  __IO uint32_t ISPR[8];                      /*!< Offset: 0x100  Interrupt 使能悬起          */
       uint32_t RESERVED2[24];                                   
  __IO uint32_t ICPR[8];                      /*!< Offset: 0x180  Interrupt 清除悬起        */
       uint32_t RESERVED3[24];                                   
  __IO uint32_t IABR[8];                      /*!< Offset: 0x200  Interrupt 有效位寄存器           */
       uint32_t RESERVED4[56];                                   
  __IO uint8_t  IP[240];                      /*!< Offset: 0x300  Interrupt 中断优先级寄存器,8位有效 */
       uint32_t RESERVED5[644];                                  
  __O  uint32_t STIR;                         /*!< Offset: 0xE00  软件触发中断寄存器     */
}  NVIC_Type;   

```



中断编程顺序

1. 使能中断请求----外设使能, 控制外设相关寄存器 + 总开关NVIC中断使能寄存器
2. 配置中断优先级分组---NVIC->IPRx寄存器来定义中断优先级，内核外设**SCB->AIRCR的PRIGROUP[10:8]**位决定优先级分组

**`void NVIC_PriorityGroupConfig(uint32_t NVIC_PriorityGroup)`**

![image-20240813210425709](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408132115160.png)

3.配置NVIC寄存器，初始化NVIC——InitTypeDef

```c
typedef struct
{
 uint8_t NVIC_IRQChannel;         

 uint8_t NVIC_IRQChannelPreemptionPriority;
 
 uint8_t NVIC_IRQChannelSubPriority;    
 
 FunctionalState NVIC_IRQChannelCmd;     
 
} NVIC_InitTypeDef;
```

4.编写中断服务函数---在中断向量表里面找到对应的函数名称，中断函数写错了会执行启动文件中的, 陷入循环, 函数全部在stm32F10x_it.h文件中重新定义为空函数，中断服务函数写在stm32f10x_it.c里面



# EXTI(0-15-GPIO中断->外部中断)

EXTI(External interupt/event Controller外部中断/事件控制器)控制**，在** **APB2** **总线上**

F1系列有20个EXTI线

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408132117692.png" alt="image-20240813211751636" style="zoom:67%;" />



**事件:不进入NVIC，只用于内部硬件的自动控制，比如ADC**

------

**中断:经过NVIC，需要中断服务函数**



<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408132118945.png" alt="image-20240813211854870" style="zoom:67%;" />

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408132119207.png" alt="image-20240813211905152" style="zoom:67%;" />

**红线是中断**--**属于软件级的**

1. 是GPIO的任意一个引脚，一般是存在电平变化的信号,使用AFIO的寄存器进行配置
2. 是一个边沿检测电路，通过上升/下降沿触发选择器寄存器，对应位给1开启边沿信号检测，检测到了就在或门处给个1，而两个寄存器可以配置上升沿/下降沿，或者上升沿和下降沿都触发
3. 是一个或门电路，软件中断事件寄存器由软件置位 1
4. 是一个与门电路，中断屏蔽寄存器设置为1表示开放中断，编号 4     电路输出的信号会被保存到挂起寄存器(EXTI_PR)内，如果确定编号 4 电路输出为 1 就会把 EXTI_PR 对应位置 1，那么EXTI_PR     对应位就会变为0
5. 发送到内核NVIC中请求产生中断，并需求中断服务函数

**绿线是事件**-**最终输出一个脉冲信号**--**属于硬件级的**

1. 是一个与门电路，那么可以简单的控制 EXTI_EMR     来实现是否产生事件的目的，EXTI_EMR事件屏蔽寄存器控制, 写1使能, 来到脉冲发生器
2. 脉冲发生器，输入1就 产生脉冲
3.  脉冲信号，就是产生事件的线路最终的产物，这个脉冲信号可以给其他外设电路使用，比如定时器     TIM、模拟数字转换器 ADC 等等，这样的脉冲信号一般用来触发 TIM 或者 ADC 开始转换

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408132120148.png" alt="image-20240813212021116" style="zoom:67%;" />







***编程要点***

- ***初始化GPIO 初始化EXTI*** 
- ***初始化NVIC***
- ***编写中断服务函数***
- ***main函数***



------



***实际编程***

***AFIO:复用功能IO和调试配置，主要用来外部中断和重映射的配置***

***1-调试IO的配置: AFIO_MAPR[26:24], 配置JTAG和SWD的开关状态***

***2-重映射配置: AFIO_MAPR, 部分外设IO的重映射配置***

***3-外部中断配置: AFIO_EXTICR1-4, EXTI的每根线具体对应的IO***

![image-20240813212321819](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408132123858.png)

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408132123186.png" alt="image-20240813212343140" style="zoom:50%;" />

***如上图，一个中断线是对应多条IO口，所以多个IO口在同一时间只能有一个能触发外部中断***



**若要进行PA0 PB0 PC0 的中断检测，则需要用GPIOx->IDR*寄存器进行检测***

**在中断服务函数里面判断是哪一个端口触发的中断**