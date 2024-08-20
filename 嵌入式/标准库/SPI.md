# SPI-多主机通信(轮流做主机而非同时多个主机)（W25Q64）

## SPI介绍

![image-20240818123728081](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408181237133.png)

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408181237798.png" alt="image-20240818123753737" style="zoom:60%;" />

## SPI框图

![](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408200946814.png)

***我们一般用软件来管理NSS(CS,SS)，这样的话NSS引脚就能当作其他的功能，即可以当作一个正常的IO，既可以电灯也可以做其他***

![image-20240818125409383](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408181254432.png)

![image-20240818124303321](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408181243360.png)

![](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408181302231.png)

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408181243773.png" alt="image-20240818124315730" style="zoom:67%;" />

## SPI移位示意图

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408181317126.png" alt="image-20240818131731072" style="zoom:67%;" />

![image-20240818132239882](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408181322912.png)

主机和从机都根据约定好的先行方式和波特率，先移出一位，到MOSI或者MISO上，接着再移入移位寄存器中

## SPI工作模式

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408181324810.png" alt="image-20240818132437740" style="zoom:50%;" />

# 软件SPI

### 起始条件和终止条件

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408181327053.png" alt="image-20240818132736002" style="zoom:50%;" />

### 交换一个字节--模式0

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408181328300.png" alt="image-20240818132828246" style="zoom:50%;" />

### 交换一个字节--模式1

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408181328220.png" alt="image-20240818132806157" style="zoom:50%;" />

### 代码实现

```c
#ifndef __BSP_SPI_H
#define __BSP_SPI_H

#include "stm32f10x.h"                  // Device header

#define SPI_SCK_PORT  					GPIOA
#define SPI_MOSI_PORT  					GPIOA
#define SPI_MISO_PORT  					GPIOA

#define SPI_SCK_PIN  				GPIO_Pin_5
#define SPI_MOSI_PIN  			GPIO_Pin_7
#define SPI_MISO_PIN  			GPIO_Pin_6

#define SPI_CS_PORT  				GPIOC
#define SPI_CS_PIN  				GPIO_Pin_0



uint8_t MySPI_SwapByte(uint8_t ByteSend);
void My_SPI_Init(void);
void MySPI_Start(void);
void MySPI_Stop(void);

#endif //__BSP_SPI_H
```



```c
#include "bsp_spi.h"

/*
        typedef enum
        { Bit_RESET = 0,
          Bit_SET
        }BitAction;
    是stm32f10x_gpio.h里面定义的枚举
*/
void MySPI_W_CS(uint8_t BitValue)
{
	GPIO_WriteBit(SPI_CS_PORT,SPI_CS_PIN,(BitAction) BitValue);
}

void MySPI_W_SCK(uint8_t BitValue)
{
	GPIO_WriteBit(SPI_SCK_PORT,SPI_SCK_PIN,(BitAction) BitValue);
}

void MySPI_W_MOSI(uint8_t BitValue)
{
	GPIO_WriteBit(SPI_MOSI_PORT,SPI_MOSI_PIN,(BitAction) BitValue);
}

uint8_t MySPI_R_MISO(void)
{
	return GPIO_ReadInputDataBit(SPI_MISO_PORT,SPI_MISO_PIN);
}

void My_SPI_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct;//定义GPIO结构体

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC,ENABLE);//
	
	//设置速度和输出方式
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;//通用推挽输出
	GPIO_InitStruct.GPIO_Pin   = SPI_SCK_PIN | SPI_MOSI_PIN ;
	GPIO_Init(SPI_SCK_PORT,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin   = SPI_CS_PIN;
	GPIO_Init(SPI_CS_PORT,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IPD;
	GPIO_InitStruct.GPIO_Pin   = SPI_MISO_PIN;
	GPIO_Init(SPI_MISO_PORT,&GPIO_InitStruct);
	
	MySPI_W_CS(1);//默认不选择从机
	MySPI_W_SCK(0);//使用模式0
}

void MySPI_Start()
{
	MySPI_W_CS(0);
}

void MySPI_Stop()
{
	MySPI_W_CS(1);
}

//模式0
//交换一个字节，返回值为交换后的字节，参数为发送的字节
uint8_t MySPI_SwapByte(uint8_t ByteSend)
{
	uint8_t ByteReceive = 0x00;
	uint8_t i;
	
	for(i=0;i<=7;i++)
	{
		MySPI_W_MOSI(ByteSend & (0x80>>i));//第0个边沿，主机移出最高位
		MySPI_W_SCK(1);					   //模式0，空闲下SCK为低，这里是第一个边沿，拉高SCK
		if(MySPI_R_MISO() == 1){ByteReceive |= (0x80>>i);}//第一个边沿，主机移入数据
		MySPI_W_SCK(0);						//拉低SCK
	}
	return ByteReceive;
}


//另一种实现方式，更接近于笔记中的移位示意图
//不过ByteSend被改变了
/*
uint8_t MySPI_swapByte(uint8_t ByteSend)
{
	uint8_t i;
	
	for(i=0;i<+7;i++)
	{
		MySPI_W_MOSI(ByteSend & (0x80>>i));//第0个边沿，主机移出最高位
		ByteSend <<= 1;					 //移出最高位，最低为补零
		MySPI_W_SCK(1);					//第一个边沿
		if(MySPI_R_MISO() == 1){ByteSend |= 0x01;}//主机移入数据到ByteSend最低位
		MySPI_W_SCK(0);
	}
	return ByteSend;
}
*/

//模式1       就是修改极性相位
//交换一个字节，返回值为交换后的字节，参数为发送的字节
//uint8_t MySPI_swapByte(uint8_t ByteSend)
//{
//	uint8_t ByteReceive = 0x00;
//	uint8_t i;
//	
//	for(i=0;i<+7;i++)
//	{
//		MySPI_W_SCK(1);										//第一个边沿
//		MySPI_W_MOSI(ByteSend & (0x80>>i));					//取出最高位
//		MySPI_W_SCK(0);										//第二个边沿
//		if(MySPI_R_MISO() == 1){ByteReceive |= (0x80>>i);}	//主机移入数据
//	}
//	return ByteReceive;
//}
```



# 硬件SPI

![image-20240820094735191](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408200947240.png)



## 非连续传输

![image-20240820094832677](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408200948727.png)

![image-20240820095505767](C:\Users\a1874\AppData\Roaming\Typora\typora-user-images\image-20240820095505767.png)

## 连续传输

![image-20240820094759523](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408200947583.png)

## 代码实现

```c
#ifndef __BSP_SPI_H
#define __BSP_SPI_H

#include "stm32f10x.h"                  // Device header

#define SPI_SCK_PORT  					GPIOA
#define SPI_MOSI_PORT  					GPIOA
#define SPI_MISO_PORT  					GPIOA

#define SPI_SCK_PIN  				GPIO_Pin_5
#define SPI_MOSI_PIN  				GPIO_Pin_7
#define SPI_MISO_PIN  				GPIO_Pin_6

#define SPI_CS_PORT  				GPIOC
#define SPI_CS_PIN  				GPIO_Pin_0




void My_SPI_Init(void);
void MySPI_Start(void);
void MySPI_Stop(void);
uint8_t MySPI_SwapByte(uint8_t ByteSend);
	

#endif //__BSP_SPI_H

```



```c
#include "bsp_spi.h"

void MySPI_W_CS(uint8_t BitValue)
{
	GPIO_WriteBit(SPI_CS_PORT,SPI_CS_PIN,(BitAction) BitValue);
}


void My_SPI_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct;//定义GPIO结构体
	//开启GPIO和SPI_1的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_SPI1,ENABLE);
	
	//设置速度和输出方式
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;//复用推挽输出
	GPIO_InitStruct.GPIO_Pin   = SPI_SCK_PIN | SPI_MOSI_PIN ;
	GPIO_Init(SPI_SCK_PORT,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;//通用推挽输出
	GPIO_InitStruct.GPIO_Pin   = SPI_CS_PIN;
	GPIO_Init(SPI_CS_PORT,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IPD;//上拉
	GPIO_InitStruct.GPIO_Pin   = SPI_MISO_PIN;
	GPIO_Init(SPI_MISO_PORT,&GPIO_InitStruct);
	
	
	//初始化SPI
	SPI_InitTypeDef SPI_InitSturct;
	SPI_InitSturct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//双线全双工
	SPI_InitSturct.SPI_Mode = SPI_Mode_Master;//作主机
	SPI_InitSturct.SPI_DataSize = SPI_DataSize_8b;//8位数据帧
	SPI_InitSturct.SPI_CPOL = SPI_CPOL_Low;//时钟极性，使用模式0，空闲时SCK为低电平
	SPI_InitSturct.SPI_CPHA = SPI_CPHA_1Edge;//时钟相位，模式0，第一个边沿采样，即移入
	SPI_InitSturct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;//配置SCK时钟频率，经测试小于32分频的都不行
	SPI_InitSturct.SPI_NSS = SPI_NSS_Soft;//软件模拟CS片选
	SPI_InitSturct.SPI_FirstBit = SPI_FirstBit_MSB;//高位先行
	SPI_InitSturct.SPI_CRCPolynomial = 7;//给默认值7
	
	SPI_Init(SPI1,&SPI_InitSturct);
	
	SPI_Cmd(SPI1,ENABLE);
	
	MySPI_W_CS(1);//默认不选择从机

}

void MySPI_Start(void)
{
	MySPI_W_CS(0);
}
void MySPI_Stop(void)
{
	MySPI_W_CS(1);
}


//模式0
//交换一个字节，返回值为交换后的字节，参数为发送的字节
uint8_t MySPI_SwapByte(uint8_t ByteSend)
{
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE) == RESET);		//TXE不为0，即为1时表示发送缓冲区为空，就退出while循环
	
	SPI_I2S_SendData(SPI1,ByteSend);									//发送一个字节
	
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE) == RESET);		//RXNE不为0，即为1表示接收缓冲区中有，就退出while循环

	return SPI_I2S_ReceiveData(SPI1);									//接收并返回一个字节

}

```



# W25Q64

## 简介

***具体的看芯片手册***

Nor Flash闪存

Flash->掉电不丢失

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408201014970.png" alt="image-20240820101456886" style="zoom:70%;" />

### 硬件电路

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408201015684.png" alt="image-20240820101511600" style="zoom:70%;" />

## 内存划分

![image-20240820101608102](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408201016173.png)

## 操作注意事项

![](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408201037759.png)

注意：擦出的时候最小只能一个一个扇区的擦除，且最好写扇区的首地址，一个扇区4kb，也就是4096个字节

1KB=1024B

1B(Byte)=8b(bit)

故1B就是1字节

​		

#### 写使能与失能

![image-20240820101639131](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408201016193.png)

![image-20240820101725060](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408201017120.png)

#### Busy

![image-20240820101705585](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408201017651.png)

#### 指令集

![image-20240820101744620](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408201017681.png)

![image-20240820101752493](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408201017560.png)

![image-20240820101759246](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408201017313.png)

![image-20240820101807244](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408201018306.png)



## 代码实现

```c
#ifndef __BSP_W25Q64_H
#define __BSP_W25Q64_H

#include "stm32f10x.h"                  // Device header
#include "bsp_spi.h"

void W25Q62_ReadID(uint8_t *MID,uint16_t *DID);
void W25Q64_ReadData(uint32_t Address, uint32_t *DataArray ,uint32_t Count);
void W25Q64_SectorErase(uint32_t Address);
void W25Q64_PageProgram(uint32_t Address, uint32_t *DataArray ,uint16_t Count);
void W25Q64_BufferWrite( u32 WriteAddr, u32* pBuffer,u32 NumByteToWrite);
void W25Q64_ChipErase(uint32_t Address);
void W25Q64_PowerDown(void); 
void W25Q64_WAKEUP(void);



#define W25Q64_WRITE_ENABLE 						0x06
#define W25Q64_READ_STATUS_REGISTER_1		0x05
#define W25Q64_DUMMY_BYTE 							0xFF   /* DUMMY无用数据 */
#define W25Q64_JEDEC_ID 								0x9F
#define W25Q64_PAGE_PROGRAM 						0x02
#define W25Q64_SECTOR_EARSE_4KB         0x20
#define W25Q64_CHIP_EARSE        				0xC7
#define W25Q64_READ_DATA								0x03
#define W25Q64_POWERDOWN								0xB9
#define W25Q64_RELEASEPOWER							0xAB
#define W25Q64_DEVICE_ID								0xAB

#endif 
```



```c
//参数是两个指针
//因为返回值要有两个，一个MID，一个DID，不好操作
//而两个指针变量是对地址进行操作，可以直接读取指针的值
void W25Q62_ReadID(uint8_t *MID,uint16_t *DID)
{
	MySPI_Start();
	MySPI_SwapByte(W25Q64_JEDEC_ID);
	*MID = MySPI_SwapByte(W25Q64_DUMMY_BYTE);
	*DID = MySPI_SwapByte(W25Q64_DUMMY_BYTE);
	*DID <<= 8;
	*DID |= MySPI_SwapByte(W25Q64_DUMMY_BYTE);
	MySPI_Stop();
}


//写使能
void W25Q64_WriteEnable(void)
{
	MySPI_Start();
	MySPI_SwapByte(W25Q64_WRITE_ENABLE);
	MySPI_Stop();
}

//等待忙
void W25Q64_WaitBusy(void)
{
	uint32_t TimeOut;
	MySPI_Start();
	MySPI_SwapByte(W25Q64_READ_STATUS_REGISTER_1);
	TimeOut = 100000;
	while((MySPI_SwapByte(W25Q64_DUMMY_BYTE) & 0x01) == 0x01)//1表示还是忙，手册可知状态寄存器可以被连续读出
	{
		TimeOut--;
		if(TimeOut == 0)break;
	}
	MySPI_Stop();
}

void W25Q64_Address_Edit(uint32_t Address)
{
	MySPI_SwapByte(Address >> 16);//0x12
	MySPI_SwapByte(Address >> 8);//0x34
	MySPI_SwapByte(Address);//0x56
}
//页写
//一页最大256个字节，uint8_t 最大为255，所以不够
void W25Q64_PageProgram(uint32_t Address, uint32_t *DataArray ,uint16_t Count)
{
	uint16_t i;
	W25Q64_WriteEnable();//手册规定写入，擦除等等之前要写使能
	
	MySPI_Start();
	MySPI_SwapByte(W25Q64_PAGE_PROGRAM);
	//地址是24位的，24/4=6，所以假设为0x123456
	//MySPI_SwapByte()参数是8位的
	W25Q64_Address_Edit(Address);
	for(i=0;i<Count;i++)
	{
		MySPI_SwapByte(DataArray[i]);
	}
	MySPI_Stop();
	W25Q64_WaitBusy();//先等忙，忙完再退出

}


//扇区擦除，一般写一个扇区的首地址，一个扇区
void W25Q64_SectorErase(uint32_t Address)
{
	W25Q64_WriteEnable();//手册规定写入，擦除等等之前要写使能
	MySPI_Start();
	MySPI_SwapByte(W25Q64_SECTOR_EARSE_4KB);
	W25Q64_Address_Edit(Address);
	MySPI_Stop();
	W25Q64_WaitBusy();//先等忙，忙完再退出
}


//芯片擦除
void W25Q64_ChipErase(uint32_t Address)
{
	W25Q64_WriteEnable();//手册规定写入，擦除等等之前要写使能
	
	MySPI_Start();
	MySPI_SwapByte(W25Q64_CHIP_EARSE);
	MySPI_Stop();
	W25Q64_WaitBusy();//先等忙，忙完再退出
}

 /**
  * @brief  对FLASH写入数据，调用本函数写入数据前需要先擦除扇区
  * @param	pBuffer，要写入数据的指针
  * @param  WriteAddr，写入地址
  * @param  NumByteToWrite，写入数据长度
  * @retval 无
  */
//先看写地址是否对齐到每一页的首地址
//若未对齐就求出差多少个对齐
//求出页数和不满一页的数据
//分情况，两个大框架：地址对齐/不对齐
//对齐：先写整页，最后写不足一页的，若一共就不到页，那就直接写
//未对齐：不到一页的：写满当前的页，然后写下一页
//			有多页的：先写满当前页，对齐地址，然后以次写入
void W25Q64_BufferWrite( u32 WriteAddr,u32* pBuffer, u16 NumByteToWrite)
{
	uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;
	W25Q64_WriteEnable();
	Addr = WriteAddr % 256;
	count = 256 - Addr;
	NumOfPage = NumByteToWrite / 256;
	NumOfSingle = NumByteToWrite % 256;
	
	if(Addr == 0)
	{
		if(NumOfPage == 0)
		{
			W25Q64_PageProgram(WriteAddr,pBuffer,NumByteToWrite);
		}
		else
		{
			while(NumOfPage--)
			{
				W25Q64_PageProgram(WriteAddr,pBuffer,256);
				WriteAddr += 256;
				pBuffer += 256;
			}
			W25Q64_PageProgram(WriteAddr,pBuffer,NumOfSingle);
		}
	}
	else
	{
		if(NumOfPage == 0)
		{
			if(count < NumOfSingle)
			{
				temp = NumOfSingle - count;
				W25Q64_PageProgram(WriteAddr,pBuffer,count);
				WriteAddr += count;
				pBuffer += count;
				W25Q64_PageProgram(WriteAddr,pBuffer,temp);
			}
			else
			{
				W25Q64_PageProgram(WriteAddr,pBuffer,NumByteToWrite);
			}
		}
		else
		{
			NumByteToWrite -= count;
			NumOfSingle = NumByteToWrite % 256;
			NumOfPage = NumByteToWrite / 256; 
			
			W25Q64_PageProgram(WriteAddr,pBuffer,count);
			
			WriteAddr += count;
			pBuffer += count;
			while(NumOfPage--)
			{
				W25Q64_PageProgram(WriteAddr,pBuffer,256);
				WriteAddr += 256;
				pBuffer += 256;
			}
			if(NumOfSingle)
			{
				W25Q64_PageProgram(WriteAddr,pBuffer,NumOfSingle);
			}
		}
	}
	W25Q64_WaitBusy();
}

//读字节
//读字节没有限制，都多少都行，所以用最大的32
void W25Q64_ReadData(uint32_t Address, uint32_t *DataArray ,uint32_t Count)
{
	uint32_t i;
	MySPI_Start();
	MySPI_SwapByte(W25Q64_READ_DATA);
	//地址是24位的，24/4=6，所以假设为0x123456
	//MySPI_SwapByte()参数是8位的
	MySPI_SwapByte(Address >> 16);//0x12
	MySPI_SwapByte(Address >> 8);//0x34
	MySPI_SwapByte(Address);//0x56
	for(i=0;i<Count;i++)
	{
		DataArray[i] = MySPI_SwapByte(DataArray[W25Q64_DUMMY_BYTE]);
	}
	MySPI_Stop();
}

//进入掉电模式
void W25Q64_PowerDown(void)   
{ 
  //通讯开始：CS低 
   MySPI_Start();

  // 发送 上电 命令
  MySPI_SwapByte(W25Q64_POWERDOWN);

   // 停止信号 FLASH: CS 高 
  MySPI_Stop();
}   

//唤醒
void W25Q64_WAKEUP(void)   
{
  //选择 FLASH: CS 低 
  MySPI_Start();

  // 发送 上电 命令 
  MySPI_SwapByte(W25Q64_RELEASEPOWER);

   // 停止信号 FLASH: CS 高 
  MySPI_Stop();
}   

```



## 实验注意事项

擦除之后为FF 

flash只能1->0，不能0->1

写入不能跨页，即只写入了55，另外三个因为跨页写入而失效

![image-20240820101911424](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408201019487.png)

下一页地址:0x000100

![image-20240820101921084](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408201019147.png)

读取可跨页，所以读到的三个ff ff ff 是下一页的数据

![image-20240820101934910](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408201019969.png)

![image-20240820101940640](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408201019703.png)
