# SRAM

就好比电脑上的内存条，可以进行扩展

## IS62WV51216ALL芯片

![image-20250303132850268](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503031328317.png)

> SRAM内部框图
>
> 1：存储器矩阵：用来存储数据的内存，SRAM掉电易丢失（EEPROM和FLASH不丢失），读写速度快
>
> 2：从地址译码器输入一个地址，然后通过列IO来找到对应的内存并返回内存中的值
>
> 3：控制电路，UB#和LB#就是用来控制要访问的是低字节还是高字节，如下图，一个存储单元是2个字节，UB#和LB#就是用来控制访问的是第一列还是第二列
>
> ![image-20250303134105008](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503031341032.png)

地址译码器有0-18 一共19根地址线，所以能寻址2的19次方 = 512*1024 = 512K个存储单元，另外本例子的芯片的数据宽度为16位，即一个存储单元为2个字节，所以SRAM的总内存是512K * 2 = 1M字节（对应芯片名称51216）

## SRAM信号线

![image-20250303133147409](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503031331460.png)

- IO 0-18：就类似于SPI的MISO和MOSI，用来传输数据
- CS1#和CS2：片选引脚，CS1#低电平有效，CS2高电平有效
- 带#的都是低电平有效

## SRAM存储器矩阵

![image-20250303133441250](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503031334283.png)

每一格代表一个SRAM的存储单元，通过行和列来寻址

但是本例芯片比较小，不需要列地址，只需要行地址

## SRAM读时序

如下图，他有时钟线，所以为异步通信

- 异步：UART，CAN
- 同步：IIC，SPI
- 可同可异：USART

![image-20250303134230052](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503031342095.png)

在发送地址的同时或者在发送地址之前就要把片选拉低（CS1#）或者拉高（CS2），一段时间后（图上并未准确说明，则认为不重要）进行读使能，然后在 tDOE 时间之后，SRAM的数据线上就出现数据了，存在时间为 （tRC -  tAA + tOHA ）的时间

![image-20250303134957148](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503031349201.png)

- 这些时间需要严格来遵循，因为他们没有时钟线

## SRAM写时序

![image-20250303135054732](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503031350779.png)

从图中可以看出来，单片机会在 tSD 之后进行采样,tHD是采样所需时间

> ![image-20250303135403161](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503031354203.png)

# FSMC

**——灵活的静态存储控制器**

所以他只能控制静态存储器，不能控制动态存储器比如SDRAM（动态在需要定时刷新，F429可以，因为他叫FMC，没有静态）

可以驱动SRAM，NOR FLASH，NAND FLASH类型的存储器

## 框图

![image-20250303140212636](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503031402712.png)

由上图可知FSMC直接连在了AHB总线上，很多地址线和数据线都是直接从内核上面连出来的，所以FSMC有些引脚通过直接访问内存的地址就可以访问SRAM和NOR，NANDFLASH

### 通信引脚

![image-20250303143032248](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503031430295.png)

> FSMC_NE具有四个引脚，用来控制四个不同的内存
>
> 四个NE引脚可以同时控制四个不同的SRAM芯片

## 存储器控制器

**NOR/PSRAM/SRAM设备使用相同的控制器, NAND/PC卡使用相同的控制器, 不同的控制器有不同的寄存器控制**

### SRAM

每一格寄存器都有四个，因为FSMC可以控制四个不同的内存

#### FSMC_BCR1/2/3/4 控制寄存器 —— 控制存储器类型, 数据宽度, 信号有效极性

![image-20250304103513969](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503041035027.png)

![image-20250304103536392](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503041035427.png)

![image-20250304103810541](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503041038568.png)

#### FSMC_BTR1/2/3/4 片选时序寄存器（读时序寄存器）—— 配置时间延迟, 比如数据保持时间，地址保存时间

![image-20250304103723154](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503041037185.png)

![image-20250304103749100](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503041037150.png)

#### FSMC_BWTR1/2/3/4 写时序寄存器 

和片选时序寄存器基本一样, 主要用来扩展模式（由BCR寄存器控制）的时候读时序, 写时序使用不同的参数，用来更加细致的区分读时序和写时序的各种时间延迟

## 时钟控制逻辑

FSMC挂在在AHB总线上，HCLK为72Mhz,控制器的**同步**时钟输出CLK就是由HCLK分频得来，只有在控制同步类型的存储器时才有效，可以配置1/2或者1/3分频（BCR寄存器控制），所以最大为36Mhz

控制异步类寄存器的时候，即使配置了同步时钟输出，也不会有时钟输出，本实例中的SRAM为异步的

## FSMC地址映射

![image-20250304105903461](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503041059509.png)

stm32把外部RAM的地址映射到了FSMC控制器，实现了一种内存访问模式：

我们只需要定义一个指针指向要访问的地址，stm32默认我们要使用FSMC来访问内存，那他就会控制FSMC来自动产生所需要的时序，我们只需要配置工作模式和时间什么的就好了，就不需要想IIC和SPI一样我们自己还要检测各种标志位

可以理解为FSMC把他所连接的外部RAM的地址映射到了STM32的External RAM（stm32预留了1G的空间）的区域了

![image-20250304115025931](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503041150977.png)

本例的SRAM是连接到了bank1的第三个区域，当我们访问SRAM的0地址时，FSMC会自动给我们映射到0x6800 0000的地址上面，访问1地址时，会自动映射到0x6800 0001的地址上面

## FSMC控制SRAM的时序

FSMC外设支持输出不同的时序来控制不同的存储器，它具有ABCD四种模式

下面是控制SRAM使用的模式A

### 读时序

![image-20250304115329317](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503041153365.png)

> T = 1/72Mhz = 13.8ns
>
> (ADDSET +1 +DATAST +1 + 2)T > 55ns 
>
> (DATAST +1)T > 25ns
>
> (ADDSET + 1)T > 0ns
>
> 所以理论值，ADDSET = 0，DATAST = 1，但是经测试，DATAST = 1并不能完全成功，而DATAST = 2时将可以胜任

![image-20250304120201065](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503041202111.png)

![image-20250304120211764](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503041202800.png)

![image-20250304120219285](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503041202324.png)

### 写时序

![image-20250304120255871](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503041202923.png)

> (ADDSET +1 +DATAST +1 )T > 55ns 
>
> (DATAST +1 )T>  40ns
>
> (ADDSET +1) T > 0ns
>
> 所以理论值是ADDSET  = 0，DATAST  = 2

## FSMC库函数

```c
typedef struct
{
  uint32_t FSMC_AddressSetupTime;       /*地址建立时间, 设置ADDSET, 设置之后会加一, 从发出地址后到读（写）使能信号跳变的时刻* */

  uint32_t FSMC_AddressHoldTime;        /*设置地址保持时间（ADDHLD）是复用模式, 不关注*/

  uint32_t FSMC_DataSetupTime;          /*设置数据建立时间，（DATAST） */

  uint32_t FSMC_BusTurnAroundDuration;  /*总线转换周期@note: It is only used for multiplexed NOR Flash memories. */

  uint32_t FSMC_CLKDivision;            /*同步时钟输出的分频因子，用于同步而非异步*/

  uint32_t FSMC_DataLatency;            /*数据的保持时间, SRAM也没有使用*/

  uint32_t FSMC_AccessMode;             /*选择访问模式这里使用模式A*/
}FSMC_NORSRAMTimingInitTypeDef;//配置模式和时序的结构体
```

```c
typedef struct
{
  uint32_t FSMC_Bank;                /*设置要控制的bank区域，通过寄存器组来选择（BCR1/2/3/4）*/

  uint32_t FSMC_DataAddressMux;      /*地址线数据线是否复用，只有NOR FALSH可以使用*/

  uint32_t FSMC_MemoryType;          /*存储器的类型*/

  uint32_t FSMC_MemoryDataWidth;     /*配置数据位的大小*/

  uint32_t FSMC_BurstAccessMode;     /* 配置是否使用突发模式, 突发模式：发送一个地址后连续访问多个数据（地址会自动加1），非突发模式下每访问一个数据都需要输入一个地址，只有在控制同步类型的存储器才能使用突发模式*/
                                       
  uint32_t FSMC_AsynchronousWait;     /*是否使能同步传输的时候的等待信号，异步不需要*/

  uint32_t FSMC_WaitSignalPolarity;  /*设置等待信号的极性，异步不需要*/

  uint32_t FSMC_WrapMode;            /*设置是否支持对齐的突发模式*/

  uint32_t FSMC_WaitSignalActive;    /*设置的等待信号在等待前的一个数据周期有效还是等待期间有效*/

  uint32_t FSMC_WriteOperation;      /*是否写使能，禁止的话就只能读取不能写入*/

  uint32_t FSMC_WaitSignal;          /*是否能使能等待状态插入*/

  uint32_t FSMC_ExtendedMode;        /*是否使能扩展模式(使用两个时序)，也即是模式A和模式1*/

  uint32_t FSMC_WriteBurst;          /*是否使能写的突发操作*/ 

  FSMC_NORSRAMTimingInitTypeDef* FSMC_ReadWriteTimingStruct; /*读寄存器的时序, 或者在没有分开使用的时候使用的时序*/  

  FSMC_NORSRAMTimingInitTypeDef* FSMC_WriteTimingStruct;     /*写寄存器的时序，要使用这个，就必须使能扩展模式*/
}FSMC_NORSRAMInitTypeDef;//初始化结构体
```

f103vet6上面没有外部SRAM，实验就没有现象

下面是按照野火来写的程序

```c
#ifndef __SRAM_H
#define __SRAM_H															    



#include "stm32f10x.h"

//使用NOR/SRAM的 Bank1.sector3,地址位HADDR[27,26]=10 
//对IS61LV25616/IS62WV25616,地址线范围为A0~A17 
//对IS61LV51216/IS62WV51216,地址线范围为A0~A18
#define Bank1_SRAM3_ADDR    ((uint32_t)(0x68000000))		

#define IS62WV51216_SIZE 0x100000  //512*16/2bits = 0x100000  ，1M字节


  
/*A地址信号线*/    
#define FSMC_A0_GPIO_PORT        GPIOF
#define FSMC_A0_GPIO_CLK         RCC_APB2Periph_GPIOF
#define FSMC_A0_GPIO_PIN         GPIO_Pin_0

#define FSMC_A1_GPIO_PORT        GPIOF
#define FSMC_A1_GPIO_CLK         RCC_APB2Periph_GPIOF
#define FSMC_A1_GPIO_PIN         GPIO_Pin_1

#define FSMC_A2_GPIO_PORT        GPIOF
#define FSMC_A2_GPIO_CLK         RCC_APB2Periph_GPIOF
#define FSMC_A2_GPIO_PIN         GPIO_Pin_2

#define FSMC_A3_GPIO_PORT        GPIOF
#define FSMC_A3_GPIO_CLK         RCC_APB2Periph_GPIOF
#define FSMC_A3_GPIO_PIN         GPIO_Pin_3

#define FSMC_A4_GPIO_PORT        GPIOF
#define FSMC_A4_GPIO_CLK         RCC_APB2Periph_GPIOF
#define FSMC_A4_GPIO_PIN         GPIO_Pin_4

#define FSMC_A5_GPIO_PORT        GPIOF
#define FSMC_A5_GPIO_CLK         RCC_APB2Periph_GPIOF
#define FSMC_A5_GPIO_PIN         GPIO_Pin_5

#define FSMC_A6_GPIO_PORT        GPIOF
#define FSMC_A6_GPIO_CLK         RCC_APB2Periph_GPIOF
#define FSMC_A6_GPIO_PIN         GPIO_Pin_12

#define FSMC_A7_GPIO_PORT        GPIOF
#define FSMC_A7_GPIO_CLK         RCC_APB2Periph_GPIOF
#define FSMC_A7_GPIO_PIN         GPIO_Pin_13

#define FSMC_A8_GPIO_PORT        GPIOF
#define FSMC_A8_GPIO_CLK         RCC_APB2Periph_GPIOF
#define FSMC_A8_GPIO_PIN         GPIO_Pin_14

#define FSMC_A9_GPIO_PORT        GPIOF
#define FSMC_A9_GPIO_CLK         RCC_APB2Periph_GPIOF
#define FSMC_A9_GPIO_PIN         GPIO_Pin_15

#define FSMC_A10_GPIO_PORT        GPIOG
#define FSMC_A10_GPIO_CLK         RCC_APB2Periph_GPIOG
#define FSMC_A10_GPIO_PIN         GPIO_Pin_0

#define FSMC_A11_GPIO_PORT        GPIOG
#define FSMC_A11_GPIO_CLK         RCC_APB2Periph_GPIOG
#define FSMC_A11_GPIO_PIN         GPIO_Pin_1

#define FSMC_A12_GPIO_PORT        GPIOG
#define FSMC_A12_GPIO_CLK         RCC_APB2Periph_GPIOG
#define FSMC_A12_GPIO_PIN         GPIO_Pin_2

#define FSMC_A13_GPIO_PORT        GPIOG
#define FSMC_A13_GPIO_CLK         RCC_APB2Periph_GPIOG
#define FSMC_A13_GPIO_PIN         GPIO_Pin_3

#define FSMC_A14_GPIO_PORT        GPIOG
#define FSMC_A14_GPIO_CLK         RCC_APB2Periph_GPIOG
#define FSMC_A14_GPIO_PIN         GPIO_Pin_4

#define FSMC_A15_GPIO_PORT        GPIOG
#define FSMC_A15_GPIO_CLK         RCC_APB2Periph_GPIOG
#define FSMC_A15_GPIO_PIN         GPIO_Pin_5

#define FSMC_A16_GPIO_PORT        GPIOD
#define FSMC_A16_GPIO_CLK         RCC_APB2Periph_GPIOD
#define FSMC_A16_GPIO_PIN         GPIO_Pin_11

#define FSMC_A17_GPIO_PORT        GPIOD
#define FSMC_A17_GPIO_CLK         RCC_APB2Periph_GPIOD
#define FSMC_A17_GPIO_PIN         GPIO_Pin_12

#define FSMC_A18_GPIO_PORT        GPIOD
#define FSMC_A18_GPIO_CLK         RCC_APB2Periph_GPIOD
#define FSMC_A18_GPIO_PIN         GPIO_Pin_13

/*D 数据信号线*/
#define FSMC_D0_GPIO_PORT        GPIOD
#define FSMC_D0_GPIO_CLK         RCC_APB2Periph_GPIOD
#define FSMC_D0_GPIO_PIN         GPIO_Pin_14

#define FSMC_D1_GPIO_PORT        GPIOD
#define FSMC_D1_GPIO_CLK         RCC_APB2Periph_GPIOD
#define FSMC_D1_GPIO_PIN         GPIO_Pin_15

#define FSMC_D2_GPIO_PORT        GPIOD
#define FSMC_D2_GPIO_CLK         RCC_APB2Periph_GPIOD
#define FSMC_D2_GPIO_PIN         GPIO_Pin_0

#define FSMC_D3_GPIO_PORT        GPIOD
#define FSMC_D3_GPIO_CLK         RCC_APB2Periph_GPIOD
#define FSMC_D3_GPIO_PIN         GPIO_Pin_1

#define FSMC_D4_GPIO_PORT        GPIOE
#define FSMC_D4_GPIO_CLK         RCC_APB2Periph_GPIOE
#define FSMC_D4_GPIO_PIN         GPIO_Pin_7

#define FSMC_D5_GPIO_PORT        GPIOE
#define FSMC_D5_GPIO_CLK         RCC_APB2Periph_GPIOE
#define FSMC_D5_GPIO_PIN         GPIO_Pin_8

#define FSMC_D6_GPIO_PORT        GPIOE
#define FSMC_D6_GPIO_CLK         RCC_APB2Periph_GPIOE
#define FSMC_D6_GPIO_PIN         GPIO_Pin_9

#define FSMC_D7_GPIO_PORT        GPIOE
#define FSMC_D7_GPIO_CLK         RCC_APB2Periph_GPIOE
#define FSMC_D7_GPIO_PIN         GPIO_Pin_10

#define FSMC_D8_GPIO_PORT        GPIOE
#define FSMC_D8_GPIO_CLK         RCC_APB2Periph_GPIOE
#define FSMC_D8_GPIO_PIN         GPIO_Pin_11

#define FSMC_D9_GPIO_PORT        GPIOE
#define FSMC_D9_GPIO_CLK         RCC_APB2Periph_GPIOE
#define FSMC_D9_GPIO_PIN         GPIO_Pin_12

#define FSMC_D10_GPIO_PORT        GPIOE
#define FSMC_D10_GPIO_CLK         RCC_APB2Periph_GPIOE
#define FSMC_D10_GPIO_PIN         GPIO_Pin_13

#define FSMC_D11_GPIO_PORT        GPIOE
#define FSMC_D11_GPIO_CLK         RCC_APB2Periph_GPIOE
#define FSMC_D11_GPIO_PIN         GPIO_Pin_14

#define FSMC_D12_GPIO_PORT        GPIOE
#define FSMC_D12_GPIO_CLK         RCC_APB2Periph_GPIOE
#define FSMC_D12_GPIO_PIN         GPIO_Pin_15

#define FSMC_D13_GPIO_PORT        GPIOD
#define FSMC_D13_GPIO_CLK         RCC_APB2Periph_GPIOD
#define FSMC_D13_GPIO_PIN         GPIO_Pin_8

#define FSMC_D14_GPIO_PORT        GPIOD
#define FSMC_D14_GPIO_CLK         RCC_APB2Periph_GPIOD
#define FSMC_D14_GPIO_PIN         GPIO_Pin_9

#define FSMC_D15_GPIO_PORT        GPIOD
#define FSMC_D15_GPIO_CLK         RCC_APB2Periph_GPIOD
#define FSMC_D15_GPIO_PIN         GPIO_Pin_10


/*控制信号线*/  
/*CS片选*/
/*NE3 ,对应的基地址0x68000000*/
#define FSMC_CS_GPIO_PORT        GPIOG
#define FSMC_CS_GPIO_CLK         RCC_APB2Periph_GPIOG
#define FSMC_CS_GPIO_PIN         GPIO_Pin_10

/*WE写使能*/
#define FSMC_WE_GPIO_PORT        GPIOD
#define FSMC_WE_GPIO_CLK         RCC_APB2Periph_GPIOD
#define FSMC_WE_GPIO_PIN         GPIO_Pin_5

/*OE读使能*/
#define FSMC_OE_GPIO_PORT        GPIOD
#define FSMC_OE_GPIO_CLK         RCC_APB2Periph_GPIOD
#define FSMC_OE_GPIO_PIN         GPIO_Pin_4


/*LB数据掩码*/
#define FSMC_UDQM_GPIO_PORT        GPIOE
#define FSMC_UDQM_GPIO_CLK         RCC_APB2Periph_GPIOE
#define FSMC_UDQM_GPIO_PIN         GPIO_Pin_1

/*UB数据掩码*/
#define FSMC_LDQM_GPIO_PORT        GPIOE
#define FSMC_LDQM_GPIO_CLK         RCC_APB2Periph_GPIOE
#define FSMC_LDQM_GPIO_PIN         GPIO_Pin_0



/*信息输出*/
#define SRAM_DEBUG_ON         1

#define SRAM_INFO(fmt,arg...)           printf("<<-SRAM-INFO->> "fmt"\n",##arg)
#define SRAM_ERROR(fmt,arg...)          printf("<<-SRAM-ERROR->> "fmt"\n",##arg)
#define SRAM_DEBUG(fmt,arg...)          do{\
                                          if(SRAM_DEBUG_ON)\
                                          printf("<<-SRAM-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)




											  
void FSMC_SRAM_Init(void);
void FSMC_SRAM_WriteBuffer(uint8_t* pBuffer,uint32_t WriteAddr,uint32_t NumHalfwordToWrite);
void FSMC_SRAM_ReadBuffer(uint8_t* pBuffer,uint32_t ReadAddr,uint32_t NumHalfwordToRead);

uint8_t SRAM_Test(void);

#endif
```



```c
#include "sram.h"	  

/**
  * @brief  初始化控制SRAM的IO
  * @param  无
  * @retval 无
  */
static void SRAM_GPIO_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 
  /* 使能SRAM相关的GPIO时钟 */

                         /*地址信号线*/
  RCC_APB2PeriphClockCmd(FSMC_A0_GPIO_CLK | FSMC_A1_GPIO_CLK | FSMC_A2_GPIO_CLK | 
                         FSMC_A3_GPIO_CLK | FSMC_A4_GPIO_CLK | FSMC_A5_GPIO_CLK |
                         FSMC_A6_GPIO_CLK | FSMC_A7_GPIO_CLK | FSMC_A8_GPIO_CLK |
                         FSMC_A9_GPIO_CLK | FSMC_A10_GPIO_CLK| FSMC_A11_GPIO_CLK| 
												 FSMC_A12_GPIO_CLK| FSMC_A13_GPIO_CLK|FSMC_A14_GPIO_CLK|
												 FSMC_A15_GPIO_CLK|FSMC_A16_GPIO_CLK|FSMC_A17_GPIO_CLK|FSMC_A18_GPIO_CLK|
                         /*数据信号线*/
                         FSMC_D0_GPIO_CLK | FSMC_D1_GPIO_CLK | FSMC_D2_GPIO_CLK | 
                         FSMC_D3_GPIO_CLK | FSMC_D4_GPIO_CLK | FSMC_D5_GPIO_CLK |
                         FSMC_D6_GPIO_CLK | FSMC_D7_GPIO_CLK | FSMC_D8_GPIO_CLK |
                         FSMC_D9_GPIO_CLK | FSMC_D10_GPIO_CLK| FSMC_D11_GPIO_CLK|
                         FSMC_D12_GPIO_CLK| FSMC_D13_GPIO_CLK| FSMC_D14_GPIO_CLK|
                         FSMC_D15_GPIO_CLK|  
                         /*控制信号线*/
                         FSMC_CS_GPIO_CLK  | FSMC_WE_GPIO_CLK | FSMC_OE_GPIO_CLK |
                         FSMC_UDQM_GPIO_CLK|FSMC_LDQM_GPIO_CLK, ENABLE);
												 


	 /*-- GPIO 配置 -----------------------------------------------------*/

  /* 通用 GPIO 配置 */
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;       //配置为复用功能
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     
  
  /*A地址信号线 针对引脚配置*/
  GPIO_InitStructure.GPIO_Pin = FSMC_A0_GPIO_PIN; 
  GPIO_Init(FSMC_A0_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = FSMC_A1_GPIO_PIN; 
  GPIO_Init(FSMC_A1_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = FSMC_A2_GPIO_PIN; 
  GPIO_Init(FSMC_A2_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = FSMC_A3_GPIO_PIN; 
  GPIO_Init(FSMC_A3_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = FSMC_A4_GPIO_PIN; 
  GPIO_Init(FSMC_A4_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = FSMC_A5_GPIO_PIN; 
  GPIO_Init(FSMC_A5_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = FSMC_A6_GPIO_PIN; 
  GPIO_Init(FSMC_A6_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = FSMC_A7_GPIO_PIN; 
  GPIO_Init(FSMC_A7_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = FSMC_A8_GPIO_PIN; 
  GPIO_Init(FSMC_A8_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = FSMC_A9_GPIO_PIN; 
  GPIO_Init(FSMC_A9_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = FSMC_A10_GPIO_PIN; 
  GPIO_Init(FSMC_A10_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = FSMC_A11_GPIO_PIN; 
  GPIO_Init(FSMC_A11_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FSMC_A12_GPIO_PIN; 
  GPIO_Init(FSMC_A12_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = FSMC_A13_GPIO_PIN; 
  GPIO_Init(FSMC_A13_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = FSMC_A14_GPIO_PIN; 
  GPIO_Init(FSMC_A14_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = FSMC_A15_GPIO_PIN; 
  GPIO_Init(FSMC_A15_GPIO_PORT, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = FSMC_A16_GPIO_PIN; 
  GPIO_Init(FSMC_A16_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = FSMC_A17_GPIO_PIN; 
  GPIO_Init(FSMC_A17_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = FSMC_A18_GPIO_PIN; 
  GPIO_Init(FSMC_A18_GPIO_PORT, &GPIO_InitStructure);
    
  /*DQ数据信号线 针对引脚配置*/
  GPIO_InitStructure.GPIO_Pin = FSMC_D0_GPIO_PIN; 
  GPIO_Init(FSMC_D0_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = FSMC_D1_GPIO_PIN; 
  GPIO_Init(FSMC_D1_GPIO_PORT, &GPIO_InitStructure);
    
  GPIO_InitStructure.GPIO_Pin = FSMC_D2_GPIO_PIN; 
  GPIO_Init(FSMC_D2_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = FSMC_D3_GPIO_PIN; 
  GPIO_Init(FSMC_D3_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = FSMC_D4_GPIO_PIN; 
  GPIO_Init(FSMC_D4_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = FSMC_D5_GPIO_PIN; 
  GPIO_Init(FSMC_D5_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = FSMC_D6_GPIO_PIN; 
  GPIO_Init(FSMC_D6_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = FSMC_D7_GPIO_PIN; 
  GPIO_Init(FSMC_D7_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = FSMC_D8_GPIO_PIN; 
  GPIO_Init(FSMC_D8_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = FSMC_D9_GPIO_PIN; 
  GPIO_Init(FSMC_D9_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = FSMC_D10_GPIO_PIN; 
  GPIO_Init(FSMC_D10_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = FSMC_D11_GPIO_PIN; 
  GPIO_Init(FSMC_D11_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = FSMC_D12_GPIO_PIN; 
  GPIO_Init(FSMC_D12_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = FSMC_D13_GPIO_PIN; 
  GPIO_Init(FSMC_D13_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = FSMC_D14_GPIO_PIN; 
  GPIO_Init(FSMC_D14_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = FSMC_D15_GPIO_PIN; 
  GPIO_Init(FSMC_D15_GPIO_PORT, &GPIO_InitStructure);
  
  /*控制信号线*/
  GPIO_InitStructure.GPIO_Pin = FSMC_CS_GPIO_PIN; 
  GPIO_Init(FSMC_CS_GPIO_PORT, &GPIO_InitStructure);
    
  GPIO_InitStructure.GPIO_Pin = FSMC_WE_GPIO_PIN; 
  GPIO_Init(FSMC_WE_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = FSMC_OE_GPIO_PIN; 
  GPIO_Init(FSMC_OE_GPIO_PORT, &GPIO_InitStructure);    
  
  GPIO_InitStructure.GPIO_Pin = FSMC_UDQM_GPIO_PIN; 
  GPIO_Init(FSMC_UDQM_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = FSMC_LDQM_GPIO_PIN; 
  GPIO_Init(FSMC_LDQM_GPIO_PORT, &GPIO_InitStructure);	
}				  			


/**
  * @brief  初始化FSMC外设
  * @param  None. 
  * @retval None.
  */
void FSMC_SRAM_Init(void)
{	
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  readWriteTiming;

	/*初始化SRAM相关的GPIO*/
	SRAM_GPIO_Config();
		
	/*使能FSMC外设时钟*/
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);

	//地址建立时间（ADDSET）为1个HCLK 1/72M=14ns，理论值
	readWriteTiming.FSMC_AddressSetupTime = 0x00;	

	//地址保持时间（ADDHLD）模式A未用到
	readWriteTiming.FSMC_AddressHoldTime = 0x00;	 

	//数据保持时间（DATAST）+ 1个HCLK = 3/72M=42ns(对EM的SRAM芯片)，理论值为1，2是经验值
	readWriteTiming.FSMC_DataSetupTime = 0x02;		  
	
	//设置总线转换周期，仅用于复用模式的NOR操作
	readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
	
	//设置时钟分频，仅用于同步类型的存储器
	readWriteTiming.FSMC_CLKDivision = 0x00;	

	//数据保持时间，仅用于同步型的NOR
	readWriteTiming.FSMC_DataLatency = 0x00;		
	
	//选择匹配SRAM的模式
	readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 
    

	// 选择FSMC映射的存储区域： Bank1 sram3
	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;
	
	//设置地址总线与数据总线是否复用，仅用于NOR
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; 
	
	//设置要控制的存储器类型：SRAM类型
	FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;   
	
	//存储器数据宽度：16位
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b; 
	
	//设置是否使用突发访问模式，仅用于同步类型的存储器
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;
	
	//设置是否使能等待信号，仅用于同步类型的存储器
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable;
	
	//设置等待信号的有效极性，仅用于同步类型的存储器
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	
	//设置是否支持把非对齐的突发操作，仅用于同步类型的存储器
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable; 
	
	//设置等待信号插入的时间，仅用于同步类型的存储器
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	
	//存储器写使能 
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	
	//不使用等待信号
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;  		
	
	// 不使用扩展模式，读写使用相同的时序
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable; 
	
	//突发写操作
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;  
	
	//读写时序配置
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;
	
	//读写同样时序，使用扩展模式时这个配置才有效
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &readWriteTiming; 

	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //初始化FSMC配置

	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);  // 使能BANK										  
											
}
	  		



/**
  * @brief  以“字”为单位向sdram写入数据 
  * @param  pBuffer: 指向数据的指针 
  * @param  uwWriteAddress: 要写入的SRAM内部地址
  * @param  uwBufferSize: 要写入数据大小
  * @retval None.
  */
void SRAM_WriteBuffer(uint32_t* pBuffer, uint32_t uwWriteAddress, uint32_t uwBufferSize)
{
  __IO uint32_t write_pointer = (uint32_t)uwWriteAddress;
 

  /* 循环写入数据 */
  for (; uwBufferSize != 0; uwBufferSize--) 
  {
    /* 发送数据到SRAM */
    *(uint32_t *) (Bank1_SRAM3_ADDR + write_pointer) = *pBuffer++;

    /* 地址自增*/
    write_pointer += 4;
  }
    
}

/**
  * @brief  从SRAM中读取数据 
  * @param  pBuffer: 指向存储数据的buffer
  * @param  ReadAddress: 要读取数据的地十
  * @param  uwBufferSize: 要读取的数据大小
  * @retval None.
  */
void SRAM_ReadBuffer(uint32_t* pBuffer, uint32_t uwReadAddress, uint32_t uwBufferSize)
{
  __IO uint32_t write_pointer = (uint32_t)uwReadAddress;
  
  
  /*读取数据 */
  for(; uwBufferSize != 0x00; uwBufferSize--)
  {
   *pBuffer++ = *(__IO uint32_t *)(Bank1_SRAM3_ADDR + write_pointer );
    
   /* 地址自增*/
    write_pointer += 4;
  } 
}


/**
  * @brief  测试SRAM是否正常 
  * @param  None
  * @retval 正常返回1，异常返回0
  */
uint8_t SRAM_Test(void)
{
  /*写入数据计数器*/
  uint32_t counter=0;
  
  /* 8位的数据 */
  uint8_t ubWritedata_8b = 0, ubReaddata_8b = 0;  
  
  /* 16位的数据 */
  uint16_t uhWritedata_16b = 0, uhReaddata_16b = 0; 
  
  SRAM_INFO("正在检测SRAM，以8位、16位的方式读写sram...");


  /*按8位格式读写数据，并校验*/
  
  /* 把SRAM数据全部重置为0 ，IS62WV51216_SIZE是以8位为单位的 */
  for (counter = 0x00; counter < IS62WV51216_SIZE; counter++)
  {
    *(__IO uint8_t*) (Bank1_SRAM3_ADDR + counter) = (uint8_t)0x0;
  }
  
  /* 向整个SRAM写入数据  8位 */
  for (counter = 0; counter < IS62WV51216_SIZE; counter++)
  {
    *(__IO uint8_t*) (Bank1_SRAM3_ADDR + counter) = (uint8_t)(ubWritedata_8b + counter);
  }
  
  /* 读取 SRAM 数据并检测*/
  for(counter = 0; counter<IS62WV51216_SIZE;counter++ )
  {
    ubReaddata_8b = *(__IO uint8_t*)(Bank1_SRAM3_ADDR + counter);  //从该地址读出数据
    
    if(ubReaddata_8b != (uint8_t)(ubWritedata_8b + counter))      //检测数据，若不相等，跳出函数,返回检测失败结果。
    {
      SRAM_ERROR("8位数据读写错误！");
      return 0;
    }
  }
	
  
  /*按16位格式读写数据，并检测*/
  
  /* 把SRAM数据全部重置为0 */
  for (counter = 0x00; counter < IS62WV51216_SIZE/2; counter++)
  {
    *(__IO uint16_t*) (Bank1_SRAM3_ADDR + 2*counter) = (uint16_t)0x00;
  }
  
  /* 向整个SRAM写入数据  16位 */
  for (counter = 0; counter < IS62WV51216_SIZE/2; counter++)
  {
    *(__IO uint16_t*) (Bank1_SRAM3_ADDR + 2*counter) = (uint16_t)(uhWritedata_16b + counter);
  }
  
    /* 读取 SRAM 数据并检测*/
  for(counter = 0; counter<IS62WV51216_SIZE/2;counter++ )
  {
    uhReaddata_16b = *(__IO uint16_t*)(Bank1_SRAM3_ADDR + 2*counter);  //从该地址读出数据
    
    if(uhReaddata_16b != (uint16_t)(uhWritedata_16b + counter))      //检测数据，若不相等，跳出函数,返回检测失败结果。
    {
      SRAM_ERROR("16位数据读写错误！");

      return 0;
    }
  }
  
  SRAM_INFO("SRAM读写测试正常！"); 
  /*检测正常，return 1 */
  return 1;

}
```

```c
/*绝对定位方式访问SRAM,这种方式必须定义成全局变量*/
uint8_t testValue __attribute__((at(Bank1_SRAM3_ADDR)));



 /**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
 int main(void)
 {	     
	 
	LED_GPIO_Config();
	//串口初始化  
	Debug_USART_Config();		 	                       
  //初始化外部SRAM  
  FSMC_SRAM_Init();			     
	 
	printf ( "\r\n野火外部 SRAM 测试\r\n" );
	  	
  
  /*蓝灯亮，表示正在读写SRAM测试*/
  LED_BLUE;
  
  /*对SRAM进行读写测试，检测SRAM是否正常*/
  if(SRAM_Test()==1)
  {
		//测试正常 绿灯亮
    LED_GREEN;			  
  }
	else
	{
		//测试失败 红灯亮
		LED_RED;
	}
	
	/*指针方式访问SRAM*/
	{	
	 uint32_t temp;
	
	 printf("\r\n指针方式访问SRAM\r\n");
	/*向SRAM写入8位数据*/
	 *( uint8_t*) (Bank1_SRAM3_ADDR ) = (uint8_t)0xAA;
	 printf("\r\n指针访问SRAM，写入数据0xAA \r\n");

	 /*从SRAM读取数据*/
	 temp =  *( uint8_t*) (Bank1_SRAM3_ADDR );
	 printf("读取数据：0x%X \r\n",temp);

	 /*写/读 16位数据*/
	 *( uint16_t*) (Bank1_SRAM3_ADDR+10 ) = (uint16_t)0xBBBB;
	 printf("指针访问SRAM，写入数据0xBBBB \r\n");
	 
	 temp =  *( uint16_t*) (Bank1_SRAM3_ADDR+10 );
	 printf("读取数据：0x%X \r\n",temp);


	 /*写/读 32位数据*/
	 *( uint32_t*) (Bank1_SRAM3_ADDR+20 ) = (uint32_t)0xCCCCCCCC;
	 printf("指针访问SRAM，写入数据0xCCCCCCCC \r\n");	 
	 temp =  *( uint32_t*) (Bank1_SRAM3_ADDR+20 );
	 printf("读取数据：0x%X \r\n",temp);

	}
	
	/*绝对定位方式访问SRAM,这种方式必须定义成全局变量*/
	{
		testValue = 0xDD;
		printf("\r\n绝对定位访问SRAM，写入数据0xDD,读出数据0x%X,变量地址为%X\r\n",testValue,(uint32_t )&testValue);	 
	}
	
	 
	 
}


```

