[TOC]

# 通信的基本概念





<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408142218026.png" alt="image-20240814221851894" style="zoom:50%;" />





## 数据传输方式：并行/串行





<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408142219364.png" alt="image-20240814221934275" style="zoom:40%;" />



***串行：USART，SPI，IIC，W500***

***并行：SDIO（四位），FSMC（16位），W5100(16位)***



<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408142220734.png" alt="image-20240814222000690" style="zoom:67%;" />









## 通信方向

**全双工：**同一时刻，两个设备之间可以同时收发数据 USART，SPI

**半双工：**两个设备之间可以收发数据，但不能在同一时间 SPI

**单工：**只有一条数据线，只能一个方向传输，IIC 







## 同步方式

同步：有时钟信号，效率高，对时钟要求比较高

异步：没有时钟信号，为了保证准确性，加了辅助标识符





## 通信速率

**比特率：**每秒钟传输的二进制位数，单位为比特每秒(bit/s)

**波特率：**每秒钟传输的码元个数 



二者关系：

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408142221515.png" alt="image-20240814222150476" style="zoom:70%;" />





<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408142222629.png" alt="image-20240814222227542" style="zoom:50%;" />

***此时波特率 ！= 比特率***



<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408142222182.png" alt="image-20240814222256127" style="zoom:50%;" />

***此时波特率 = 比特率***







# 串口通信



## 通信协议





### 物理层(硬件部分)

RS232电平：1表示-15v~-3v 0表示+3v~15v

TTL电平：直接从单片机/芯片里面出来的 5v或者3.3v 

COMS电平: 1: 3.3V, 0: 0V





#### ***RS232***

​	**Rs232标准串口通讯结构图**

​	

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408142224030.png" alt="image-20240814222415941" style="zoom:67%;" />

**RS232标准串口通常用于工业设备直接通信**

**电平转换芯片一般有MAX3232（3.3v/5v），SP3232（3.3v）**

------





#### ***DB9***



<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408142225794.png" alt="image-20240814222507661" style="zoom:50%;" />





#### USB转串口

主要用于设备跟电脑通信



电平转换芯片：CH340,PL2303,CP2102,FT232

使用的时候需要电脑安装电平转换芯片的驱动，例如学51单片机时候安装的CH340驱动



<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408142227623.png" alt="image-20240814222715539" style="zoom:67%;" />





#### TTL电平（原生的串口到串口）



![image-20240814222745251](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408142227373.png)









### 协议层



<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408142228687.png" alt="image-20240814222822618" style="zoom:67%;" />





***起始位***是一个逻辑0的数据位表示

***结束位***是0.5、1、1.5或2个逻辑1表示

***有效数据:*** 经常被约定为5, 6, 7, 8, 9位长度, LSP在前面(低位在前), MSP在后

***校验位***:

- 奇校验,     加上校验位之后1的个数为奇数
- 偶校验,     加上校验位之后1的个数为偶数
- 无校验，没有校验位
- 零校验，校验位为0
- 一校验，校验位为1



<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408142229540.png" alt="image-20240814222905477" style="zoom:50%;" />







# USART



![image-20240814223030725](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408142230865.png)





- ***TX: 数据发送***
- ***RX: 数据接收***
- ***SCLK: 时钟, 可选***
- ***nRTX: 请求发送, 可选,     n表示低电平有效, 由硬件控制***
- ***nCTX: 允许发送，低电平有效***
- ***其他引脚: 红外编码模块***







- ***串口的实际工作流程***
  1. 首先UE(串口使能),TE(发送使能),RE(接收使能)都打开，记得配置时钟
  2. ***发送***：然后数据放到了TDR中，然后进入发送移位寄存器，此时TXE为1，表示TDR为空，此时还不能继续写入TDR，等到TC置1（由软件复位0）后表示第一个数据已经发送到了TX,若此时TCIE为1，则可以产生中断

​		***接收***：由RX的数据进入接受移位寄存器，然后进入RDR，当RDR移位寄存器中的数据进入USART_DR寄存器中后，RXNE由硬件		置1，若此时RXNEIE也为1（对USART_DR的读操作可以清零该位），则可以产生中断





***串口波特率的计算与配置(USART_BRR)***

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408142232919.png" alt="image-20240814223203867" style="zoom:50%;" />



<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408142232708.png" alt="image-20240814223219664" style="zoom:50%;" />





------



***实际使用***

- ***初始化GPIO和USART以及各自时钟--分别初始化TX（推挽输出），RX（浮空输入）的GPIO，初始化USART结构体***
- ***USART使能***
- ***如果用到了RX就初始化NVIC***
- ***初始化中断，如果读取寄存器不为空的时候就产生中断***
- ***立标志位 TC，TXE…***

```c
 void USART_Config()
{
	//初始化GPIO和USART
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	//初始化GPIO和USART时钟
	RCC_APB2PeriphClockCmd(DEBUG_USART_GPIO_CLK | DEBUG_USART_CLK,ENABLE);
	
	//TX
	GPIO_InitStructure.GPIO_Pin   = DEBUG_USART_GPIO_TX_PIN;
	GPIO_InitStructure.GPIO_Mode  = DEBUG_USART_GPIO_TX_MODE;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART_GPIO_TX_PORT,&GPIO_InitStructure);
	
	//RX
	GPIO_InitStructure.GPIO_Pin   = DEBUG_USART_GPIO_RX_PIN;
	GPIO_InitStructure.GPIO_Mode  = DEBUG_USART_GPIO_RX_MODE;
	GPIO_Init(DEBUG_USART_GPIO_RX_PORT,&GPIO_InitStructure);
	
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//字长
	USART_InitStructure.USART_StopBits   = USART_StopBits_1;			//停止位
	USART_InitStructure.USART_BaudRate	 = DEBUG_USART_BAUDRATE;	//波特率
	USART_InitStructure.USART_Parity 		 = USART_Parity_No;				//校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件流控制
	USART_InitStructure.USART_Mode		 	 = USART_Mode_Tx | USART_Mode_Rx;					//模式
	USART_Init(DEBUG_USARTx,&USART_InitStructure);
	
	//配置中断优先级
	USART_NVIC_Config();
	
	//使能串口接收中断
	USART_ITConfig(DEBUG_USARTx,USART_IT_RXNE,ENABLE);
	
	//使能串口
	USART_Cmd(DEBUG_USARTx,ENABLE);
}
void Usart_SendByte(USART_TypeDef* USARTx ,uint8_t Byte)//发送一个字节
{
	USART_SendData(USARTx,Byte);
	while(USART_GetFlagStatus(DEBUG_USARTx,USART_FLAG_TXE) == RESET);
}

void Usart_SendHalfWord(USART_TypeDef* USARTx ,uint16_t Byte)//发送两个字节
{
	uint16_t temp_h,temp_l;
	temp_h=(Byte&0xff00)>>8;
	temp_l=Byte&0xff;
	
	USART_SendData(USARTx,temp_h);
	while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE) == RESET);
	USART_SendData(USARTx,temp_l);
	while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE) == RESET);
}

void Usart_SendArray(USART_TypeDef* USARTx ,uint8_t *array,uint8_t num)//发送存放八位数据的数组
{
	uint8_t i;
	for(i=0;i<num;i++)
	{
		Usart_SendByte(USARTx,array[i]);
	}
	while(USART_GetFlagStatus(USARTx,USART_FLAG_TC) == RESET);
}

//发送字符串
void Usart_SendStr(USART_TypeDef* USARTx,uint8_t *str)
{
	uint8_t i=0;
	do
	{
		Usart_SendByte(USARTx,str[i]);
		i++;
	}while(str[i] != '\0');
	while(USART_GetFlagStatus(USARTx,USART_FLAG_TC) == RESET);
}
//重定义C库函数
int fputc(int ch, FILE *f)
{
	USART_SendData(DEBUG_USARTx, (uint8_t)ch);
	while(USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);
	return ch;
}

int fgetc(FILE *f)
{
	while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET);
	return (int)USART_ReceiveData(DEBUG_USARTx);
}
在使用后fgetc函数的时候最好不初始化中断, 或者重写中断函数, 否则会卡住

```





<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408142234309.png" alt="image-20240814223445245" style="zoom:70%;" />



## 串口收发数据包（来自江科大）

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408142235277.png" alt="image-20240814223533183" style="zoom:50%;" />





<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408142235921.png" alt="image-20240814223547836" style="zoom:50%;" />



<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408142236619.png" alt="image-20240814223600525" style="zoom:50%;" />



<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408142236049.png" alt="image-20240814223613954" style="zoom:50%;" />





## USART使用的一些问题

### 双板通信

记得T接R，R接T，然后切除vcc，gnd，尤其是gnd，防止烧板子



### 首字节丢失

场景：在进行A，C板串口通信的时候，发送的uwb数据的第一位总是丢失，然后把数据统一往外移了一位就解决了，但是第一位数据还是丢失了，即0x55还是总是没有接收到，但是由于我把数据都后移了一位所以也不影响我使用

![image-20240925212752093](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202409252127222.png)

分析原因：

直接发送数据，由于数据缓冲区（TDR)向移位寄存器发送数据是并行发送，时间较快，而从移位寄存器向外传输数据是串行输出，耗时较长，如此可能导致数据丢失多个，或只接收到最后一个，因为后面并行传输的快，还没等数据发送完成，下一个数据就覆盖住了上一个数据。

解决方法：

清除TC标志位：在发送第一个字节前，确保TC（发送完成）标志位已经清除。可以在发送数据前先读SR寄存器，再写DR寄存器来清除它。

![image-20240925221551836](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202409252215908.png)

![image-20240925221625724](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202409252216799.png)
