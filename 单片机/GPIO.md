[TOC]

# GPIO

***GPIO：general purpose intput output 通用输入输出端，即软件可以控制的引脚***



## 推挽与开漏

![image-20240813213206415](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408132132497.png)

***1是可见的外部的引脚，保护二极管原理：当电压超过3.3v后，电流会流向保护二极管，以防止内部芯片被烧坏，但是如果烧穿了二极管，则芯片就会随之被烧坏***

------



### 推挽输出

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408132134777.png" alt="image-20240813213446724" style="zoom: 67%;" />

------

**VDD是3.3V**

**INT输入	1时，Ug<Us，NMOS截止，PMOS导通：VDD-S-G-D-OUT  推 VDD给外设灌电流**

**INT输入	0时， Ug>Us,  NMOS导通，PMOS截止:	OUT=0   挽  	拉电流**

------

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408132136504.png" alt="image-20240813213607390" style="zoom:67%;" />

### 开漏输出

只能输出低电平，除非外部加上拉电阻

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408132137244.png" alt="image-20240813213733179" style="zoom: 50%;" />

------

**1--0--Ug=Us---截止---OUT为高阻态---若想输出高电平，则需要加一个上拉电阻，使VDD=OUT且VDD可以根据需求来配置**

**0--1--Ug>Us---导通---OUT接地输出低电平(由内部电路提供)**

------

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408132138034.png" alt="image-20240813213815947" style="zoom:50%;" />



# GPIO寄存器

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408132139083.png" alt="image-20240813213915039" style="zoom: 50%;" />





### ***TTL肖特/施密特触发器***

施密特触发器, 是一种**整形电路**, 把一个非标准的方波变为一个标准的方波, 有**一个正向阈值电压**, **负向阈值电压**, 

只有在变化的时候向上超过正向阈值电压, 或者向下超过负向阈值电压的时候输入的信号才会改变

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408132140843.png" alt="image-20240813214007788" style="zoom:50%;" />

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408132142362.png" alt="image-20240813214201328" style="zoom:75%;" />

# GPIO几种使用方法

```c
方法一：
//配置RCC时钟
*(unsigned int * ) 0x40021018 |=(1<<3);
//配置IO为输出
*(unsigned int * ) 0x40010C00 |=（1<<（4*0））;
//控制ODR
*(unsigned int *) 0x40010C0C &=~(1<<0);



方法二：
#ifndef __STM32F10x_H__
#define __STM32F10x_H__

#define PERIRHRAL_BASE 					((unsigned int)0x40000000)       //外设基地址
#define APB1_BASE	 					(PERIRHRAL_BASE)
#define APB2_BASE						(APB1_BASE + 0x10000)
#define AHB_BASE						(APB1_BASE + 0x20000)

#define RCC_BASE						(AHB_BASE + 0x1000)
#define GPIOB_BASE						(APB2_BASE + 0x0c00)

#define RCCAPB2_ENR						*(unsigned int *)(RCC_BASE + 0x18)
#define GPIOB_ODR						*(unsigned int *)(GPIOB_BASE + 0xc)
#define GPIOB_CRL						*(unsigned int *)(GPIOB_BASE + 0x0)

#endif



方法三：
typedef struct {
	uint32_t	GPIO_CRL;
	uint32_t	GPIO_CRH;
	uint32_t	GPIO_IDR;
	uint32_t	GPIO_ODR;
	uint32_t	GPIO_BSRR;
	uint32_t	GPIO_BRR;
	uint32_t	GPIO_LCKR;

}GPIO_TypeDef;

#define GPIOB 							((GPIO_TypeDef *)GPIOB_BASE)




第四种：使用初始化函数，并且使用宏定义增加可移植性

typedef enum
{ 
  GPIO_Speed_10MHz = 1,         // 10MHZ        (01)b
  GPIO_Speed_2MHz,              // 2MHZ         (10)b
  GPIO_Speed_50MHz              // 50MHZ        (11)b
}GPIOSpeed_TypeDef;

typedef enum
{ GPIO_Mode_AIN = 0x0,           // 模拟输入     (0000 0000)b
  GPIO_Mode_IN_FLOATING = 0x04,  // 浮空输入     (0000 0100)b
  GPIO_Mode_IPD = 0x28,          // 下拉输入     (0010 1000)b
  GPIO_Mode_IPU = 0x48,          // 上拉输入     (0100 1000)b
  
  GPIO_Mode_Out_OD = 0x14,       // 开漏输出     (0001 0100)b
  GPIO_Mode_Out_PP = 0x10,       // 推挽输出     (0001 0000)b
  GPIO_Mode_AF_OD = 0x1C,        // 复用开漏输出 (0001 1100)b
  GPIO_Mode_AF_PP = 0x18         // 复用推挽输出 (0001 1000)b
}GPIOMode_TypeDef;

typedef struct
{
	uint16_t GPIO_Pin;
	uint16_t GPIO_Speed;
	uint16_t GPIO_Mode;
}GPIO_InitTypeDef;

voidGPIO_Init(GPIO_TypeDef*GPIOx, GPIO_InitTypeDef*GPIO_InitStruct)

void LED_GPIO_Config()
{
	//开启时钟
	RCC_APB2PeriphClockCmd(LED_G_GPIO_CLOCK, ENABLE);
	//初始化引脚
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = LED_G_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	
	GPIO_Init(LED_G_GPIO_PORT, &GPIO_InitStructure);
}

```



# GPIO模式

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408132144408.png" alt="image-20240813214409356" style="zoom:80%;" />



# 通用与复用IO

### *通用:*

IO端的输入或者输出是由GPIO外设控制的



### 复用

1.同一时间IO只能用作一种复用功能，否则会发送冲突，若冲突了，就考虑**重映射**

2.IO端的输入或者输出是由非GPIO外设控制的，如USART,ADC,DAC,TIM…

3.各IO支持的复用功能可查数据手册

 

### 重映射

通过AFIO寄存器

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408132144129.png" alt="image-20240813214449092" style="zoom:50%;" />





**比如IIC进行了重映射**

下面是***AFIO_MAPR***寄存器

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408132144848.png" alt="image-20240813214457812" style="zoom:50%;" />

****

------

***==注意：当使用了外部中断时，或者使用重映射时要打开AFIO时钟==***



# 位带操作

权威指南：CM3权威指南=>5.4

寄存器里面的每一个位都重新找一个地址, **每个位膨胀为4个字节(32位), 最低位有效**, 只有最前面的1MB会被映射

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408132149789.png" alt="image-20240813214905714" style="zoom:67%;" />

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408132150395.png" alt="image-20240813215028312" style="zoom:67%;" />

![image-20240813215127806](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408132151904.png)
