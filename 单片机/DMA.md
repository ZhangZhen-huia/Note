# DMA

***Direct Memory Access***，直接**存储器**访问。主要功能是搬运数据，并且不占用CPU，通过硬件为RAM和IO开辟一条通道

## 存储器

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408162122356.png" alt="image-20240816212237242" style="zoom:50%;" />

## DMA框图

![image-20240816212508414](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408162125457.png)

### 仲裁器的作用

仲裁器来决定优先级，先看软件优先级，后看硬件优先级

即当多个DMA请求同时发生时

由仲裁器来判断哪个优先级高

### DMA优先级

***DMA优先级和中断一样分为软件和硬件***

- 软件：由`DMA_CRRx`寄存器配置，有最高，高，中和低四个等级
- 硬件：软件相同时，比较硬件，编号越小，优先级越高，（大容量芯片`DMA1`高于`DMA2`）
- ***注意：多个请求通过逻辑或输入到`DMA`时，只能有一个请求有效***

 

***M->M时所有的通道都能使用，***可以使用软件控制`DMA_CCRx`的PL, 硬件优先级, `DMA1`高于`DMA2`



<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408162131755.png" alt="image-20240816213113710" style="zoom:80%;" />

## DMA实现结构



<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408162127095.png" alt="image-20240816212710011" style="zoom:50%;" />



- **传输计数器：**递减，减到零后，自增的地址就会返回到起始地址。***写入传输计数器的时候必须先关闭使能`DMA`再写入--`DMA_CNDTRx`***

- **自动重装器：**是否恢复传输计数器的最初的值，即是否循环转运数据

- **软件触发：**以最快的速度连续不断的转运数据，一般用于存储器到存储器的转运，一般不与循环模式同时使用

- **硬件触发：**一般是外设，这些转运需要一定的时机，比如ADC转换完成，串口收到数据，定时时间……

- **开关控制：**`DMA_Cmd`函数

- **数据宽度：**少传多，高位补零；多传少，舍弃高位



## DMA寄存器

![image-20240816213207711](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408162132743.png)

***<u>注意</u>：***

- **CNDTRx** 

- **CPARx** 

- **CMARx** 

  

  以上三个寄存器都是在DMA**关闭**时写入



## DMA标准库编程1-M-->M



<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408162147662.png" alt="image-20240816214703592" style="zoom:50%;" />

```c
//const使变量存放在flash中
const uint32_t aSRC_Const_Buffer[BUFFER_SIZE] = { 0x12345678,0x12345677,0x11111111,
																																						  0x32165478,0x22554433,0x22335544,
																																						  0x33554488,0x22333111,0x54646546
												 };//原始数据-Source

//stm32的变量存放在内部SRAM中
uint32_t aDST_Buffer[BUFFER_SIZE];																							
//则此时的为  flash->SRAM  M->M																					
void MtM_DMA_Config()
{
	//初始化时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	DMA_InitTypeDef DMA_InitStruct;
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)aSRC_Const_Buffer; /*数组表示的就是首地址,是一个指针,
																																			 要用uint32_t 来强制类型转换,原始数据地址*/
	DMA_InitStruct.DMA_MemoryBaseAddr			= (uint32_t)aDST_Buffer;//存储器地址
	DMA_InitStruct.DMA_DIR							  = DMA_DIR_PeripheralSRC;//外设是aSRC_Const_Buffer，是存放在flash中的，作为源
	DMA_InitStruct.DMA_BufferSize					= BUFFER_SIZE;//传输计数器
	DMA_InitStruct.DMA_PeripheralInc			= DMA_PeripheralInc_Enable;//原始数据地址增量
	DMA_InitStruct.DMA_MemoryInc         	= DMA_MemoryInc_Enable;//存储器地址增量
	DMA_InitStruct.DMA_MemoryDataSize			= DMA_MemoryDataSize_Word;//32(64)位计算机的一个字是32(64)位,设置宽度
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;//设置宽度
	DMA_InitStruct.DMA_Mode								= DMA_Mode_Normal;//不循环
	DMA_InitStruct.DMA_Priority						= DMA_Priority_VeryHigh;//优先级
	DMA_InitStruct.DMA_M2M								= DMA_M2M_Enable;//M->M
	DMA_Init(DMA1_Channel6,&DMA_InitStruct);
	
	//清除标志位
	DMA_ClearFlag(DMA1_FLAG_TC6);
	//DMA使能
	DMA_Cmd(DMA1_Channel6,ENABLE);
}

uint8_t Buffercmp(const uint32_t* pBuffer,uint32_t* pBuffer1, uint16_t BufferLength)
{
	  /* 数据长度递减 */
  while(BufferLength--)
  {
    /* 判断两个数据源是否对应相等 */
    if(*pBuffer != *pBuffer1)
    {
      /* 对应数据源不相等马上退出函数，并返回0 */
      return 0;
    }
    /* 递增两个数据源的地址指针 */
    pBuffer++;
    pBuffer1++;
  }
  /* 完成判断并且对应数据相对 */
  return 1;  
}
```

## DMA标准库编程2-P-->M-->串口数据传输

```c
uint8_t start[5]={'s','t','a','r','t'};
uint8_t stop[4]={'s','t','o','p'};												
uint8_t Send_Buffer[8]  = {"你好！\n"};	//串口发送的数据是8位的
uint8_t Rec_buff[5];

void USARTx_Config()
{
	//定义结构体
	USART_InitTypeDef USART1_InitStruct;
	GPIO_InitTypeDef  GPIO_USART_InitStruct;
	//开启时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA,ENABLE);
	
	//初始化串口
	USART1_InitStruct.USART_BaudRate = 115200;
	USART1_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART1_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART1_InitStruct.USART_Parity = USART_Parity_No;
	USART1_InitStruct.USART_StopBits = USART_StopBits_1;
	USART1_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1,&USART1_InitStruct);
	
	//初始化GPIO
	GPIO_USART_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_USART_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_USART_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_USART_InitStruct);
	
	GPIO_USART_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_USART_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_USART_InitStruct);

	NVIC_InitTypeDef USART_NVIC;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	USART_NVIC.NVIC_IRQChannel = USART1_IRQn;
	USART_NVIC.NVIC_IRQChannelCmd = ENABLE;
	USART_NVIC.NVIC_IRQChannelPreemptionPriority = 1;
	USART_NVIC.NVIC_IRQChannelSubPriority =1;
	
	NVIC_Init(&USART_NVIC);
	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
	USART_Cmd(USART1, ENABLE);
	

}

//Memory->P(USART1->DR)---先把数据从存储器搬运到USART1_DR，然后再通过串口助手显示在电脑上
void USARTx_DMA_Config()
{
	//定义结构体
	DMA_InitTypeDef USART1_DMA_InitStruct_Rx,USART1_DMA_InitStruct_Tx;
	
	//开启时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
    
    //RX
    //我们接收到的数据存放在USART的DR寄存器，然后通过DMA运输到Rec_buff里面，此为接收
	USART1_DMA_InitStruct_Rx.DMA_PeripheralBaseAddr = (USART1_BASE+0x04);
	USART1_DMA_InitStruct_Rx.DMA_MemoryBaseAddr 		 = (uint32_t)Rec_buff;
	USART1_DMA_InitStruct_Rx.DMA_DIR								 = DMA_DIR_PeripheralSRC;
	USART1_DMA_InitStruct_Rx.DMA_BufferSize       	 = 5;
	USART1_DMA_InitStruct_Rx.DMA_PeripheralInc		   = DMA_PeripheralInc_Disable;//串口只有一个DR寄存器，所以不递增
	USART1_DMA_InitStruct_Rx.DMA_MemoryInc				 	 = DMA_MemoryInc_Enable;
	USART1_DMA_InitStruct_Rx.DMA_MemoryDataSize		 = DMA_PeripheralDataSize_Byte;
	USART1_DMA_InitStruct_Rx.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
	USART1_DMA_InitStruct_Rx.DMA_Mode               = DMA_Mode_Circular;
	USART1_DMA_InitStruct_Rx.DMA_Priority					 = DMA_Priority_VeryHigh;
	USART1_DMA_InitStruct_Rx.DMA_M2M								 = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel5,&USART1_DMA_InitStruct_Rx);
	
    
    //TX
    //我们要发送的数据存放在Send_Buffer，然后通过DMA运输到USART的DR寄存器里面，此为发送
	USART1_DMA_InitStruct_Tx.DMA_PeripheralBaseAddr = (USART1_BASE+0x04);
	USART1_DMA_InitStruct_Tx.DMA_MemoryBaseAddr 		 = (uint32_t)Send_Buffer;
	USART1_DMA_InitStruct_Tx.DMA_DIR								 = DMA_DIR_PeripheralDST;
	USART1_DMA_InitStruct_Tx.DMA_BufferSize       	 = 8;
	USART1_DMA_InitStruct_Tx.DMA_PeripheralInc		   = DMA_PeripheralInc_Disable;
	USART1_DMA_InitStruct_Tx.DMA_MemoryInc				 	 = DMA_MemoryInc_Enable;//串口只有一个DR寄存器，所以不递增
	USART1_DMA_InitStruct_Tx.DMA_MemoryDataSize		 = DMA_PeripheralDataSize_Byte;
	USART1_DMA_InitStruct_Tx.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
	USART1_DMA_InitStruct_Tx.DMA_Mode               = DMA_Mode_Normal;
	USART1_DMA_InitStruct_Tx.DMA_Priority					 = DMA_Priority_VeryHigh;
	USART1_DMA_InitStruct_Tx.DMA_M2M								 = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel4,&USART1_DMA_InitStruct_Tx);
	
	DMA_ClearFlag(DMA1_FLAG_TC4);
	
	DMA_Cmd(DMA1_Channel5,ENABLE);
	DMA_Cmd(DMA1_Channel4,ENABLE);
}

//判断DMA运输是否有误
int StringCompare(uint8_t rcData[5],uint8_t rcData2[5])
{
	for(uint8_t i = 0 ; i < 5 ; i++){
		if (rcData[i] != rcData2[i]) 	 return 0;
	}
	return 1;
}

int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t)ch);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	return ch;

```



- 定义一个源数据八位数组(因为串口传输的是8位)，用来给串口的USART_DR寄存器传输数据
- 传输完成后只需在main里加上USART_Config()函数，即可实现串口与计算机的通信，因为下图
- 另外记得串口DMA使能

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408162146359.png" alt="image-20240816214616302" style="zoom:50%;" />
