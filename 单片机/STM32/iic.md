# IIC--同步串行半双工总线

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408171603918.png" alt="image-20240817160327852" style="zoom:50%;" />

- 两根总线 SCL-串行时钟总线  SDA双向串行数据总线 
- “总线”指多个设备共用的信号线

- 在一个 I2C 通讯总线中，可连接多个 I2C 通讯设备，支持多个通讯主机及多个通讯从机。多为一主多从

- 寻址机制: 通过一个地址, 在设计硬件的时候就已经设置好了, 通过SDA线进行发送, 有七位或者是十位
- 总线通过上拉电阻接到电源。当I2C设备空闲时，会输出高阻态(防止不同从机之间导通短路)，而当所有设备都空闲，都输出高阻态时，由上拉电阻把总线拉成高电平。“线与”逻辑，一低全低，IO口使用开漏输出
- 多个主机同时使用总线时，为了防止数据冲突，会利用仲裁方式决定由哪个设备占用总线。
-  具有三种传输模式：标准模式传输速率为 1 00kbit/s ，快速模式为 400kbit/s ，高速模式下可达 3.4Mbit/s，但目前大多 I2C 设备尚不支持高速模式。
- 连接到相同总线的 IC     数量受到总线的最大电容 400pF 限制 。

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408171603026.png" alt="image-20240817160339965" style="zoom:50%;" />

## 软件IIC



### 时序总结

***在SCL低电平期间，SDA可以随便改变***，***但是在SCL高电平期间，只要SDA发生了变化，那么就一定会产生起始或者终止信号***，在发过起始信号后，***SCL只在读取SDA的时候为高，其他时间都为低***，应答就是发送/接收一个字节的一位，即去掉for循环的部分



### 起始条件和终止条件



起始条件：在SCL高电平期间，SDA拉低

终止条件：在SCL高电平期间，SDA拉高

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408171554674.png" alt="image-20240817155404624" style="zoom:50%;" />

### 发送一个字节

SCL低电平期间，**主机**将数据位依次放在SDA线上（**高位在前**），然后拉高SCL，从机将在SCL高电平期间读取数据位，依次遵循上述过程8次，即可发送一个字节

**注意**：由于是在SCL高电平期间读取的数据，故在SCL高电平期间不允许SDA发生任何变化，否则就会被当作起始或终止信号

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408171557242.png" alt="image-20240817155707193" style="zoom:50%;" />

### 接收一个字节

在SCL低电平期间，***从机***将数据位依次放在SDA线上（***高位在前***），然后拉高SCL，主机将在SCL高电平期间读取数据位，依次遵循上述过程8次，即可接收一个字节

**注意：**

- 主机在接收之前，需要释放SDA，**即把SDA置1**，把控制权交给从机

- 由于是在SCL高电平期间读取的数据，故在SCL高电平期间不允许SDA发生任何变化，否则就会被当作起始或终止信号

  

### 应答信号



发送应答：在接收完一个字节后，主机在下一个时钟发送一位数据，数据0表示应答，数据1表示非应答（同发送一个字节操作）

接收应答：在发送完一个字节后，主机在下一个时钟接收一位数据，**判断从机是否应答**，数据0表示应答，数据1表示非应答（主机在接收之前需要释放SDA，把控制权给到从机，同接收一个字节操作）

***理解：在上拉电阻的影响下，SDA在默认情况下是高电平，从而从机拉低SDA表示从机给了应答***

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408171600380.png" alt="image-20240817160059338" style="zoom:50%;" />

## 硬件IIC

### 协议层

同软件IIC

### 框图

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408171612318.png" alt="image-20240817161242256" style="zoom:67%;" />

①通讯引脚, 还有一个引脚是为了兼容**SMBus**协议

②时钟控制逻辑, SCL线的时钟信号, 根据寄存器CCR控制, 在快速模式下可以选择时钟的**占空比**, 外设时钟源为PCLK1

③数据控制逻辑, 比较器用来比较自身的地址, PEC寄存器拿来数据校验, 很少使用

④整体控制逻辑.

### 时钟频率计算

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408171613696.png" alt="image-20240817161310582" style="zoom:50%;" />

### 通讯过程

#### 主机发送

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408171613296.png" alt="image-20240817161348232" style="zoom:60%;" />

```
SB=1--起始位发送完成
ADDR=1---地址发送完成
TxE=1---数据寄存器为空
BTF=1---数据字节发送完成（移位寄存器为空）
```

#### 主机接收

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408171614233.png" alt="image-20240817161448169" style="zoom:67%;" />

```
RxNE = 1: 接收的时候数据寄存器为非空
状态位需要清除,一般是读取相关寄存器后就会清楚，具体要看寄存器的描述
使用函数I2C_CheckEvent可以直接检查对应的事件, 这个函数自动清除事件
```

# 读写EEPROM（AT24C02）

![image-20240817202159015](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408172021077.png)

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408172022121.png" alt="image-20240817202251076" style="zoom:200%;" />

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408172022394.png" alt="image-20240817202225319" style="zoom:50%;" />

​	<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408172027375.png" alt="image-20240817202713335" style="zoom:100%;" />

***由手册可知，AT24C02的写入周期最多要5ms，但是为了保险起见，我们还是延时＞5ms为好***

![image-20240817202334032](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408172023065.png)

## 读写时序

![image-20240817202459523](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408172024598.png)

### 写时序

#### Byte Write

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408172025667.png" alt="image-20240817202529584" style="zoom:50%;" />

####  Page Write

![image-20240817202939444](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408172029483.png)

### 读时序

#### Current Address Read

芯片24C08内部有一个地址计数器，记录了上一个数据访问的地址，不论是读取还是写入。因此，若上一个操作的地址是n，那么么下一步执行读取当前地址所得到的数据就是n+1。当24C08收到Control Byte后，它在SDA总线上生成ACK信号以及8bit地址n+1，主控制器则在SDA上产生not ACK信号示意24C08停止继续传输，最后主控制器产生stop结束此次任务。

注意：AT24C02 EEPROM芯片**不支持当前地址读取**功能，而C04往后的就支持了

![image-20240817203202048](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408172032090.png)

#### Random Read

随机读取并不是真的“随机”，而是读取指定地址是的数据，意思是允许主控器读取被控器任意地址的数据。

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408172025221.png" alt="image-20240817202543146" style="zoom:50%;" />

#### Sequential Read

顺序读

顺序读取是主机读取某一特定地址及其之后的若干个数据

顺序读取前面的逻辑和随机读取一致，直到最后一步：主机在接收到数据后**并不发送not ACK而是发送ACK**，这样从机继续向主控器发送数据，直到主机产生not ACK为止，最后主机会在发送not ACK之后发送stop结束任务。

![image-20240817203931167](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408172039212.png)

## 代码实现

### 硬件

```c
#ifndef __BSP_I2C_H
#define __BSP_I2C_H

#include "stm32f10x.h"

#define EEPROM_I2Cx      									I2C1
#define EEPROM_ADDRESS    								0xA0
#define EEPROM_I2C_GPIO_SDA_PORT 					GPIOB
#define EEPROM_I2C_GPIO_SCL_PORT 					GPIOB
#define EEPROM_I2Cx_ADDRESS   						0x51

#define EEPROM_I2C_GPIO_SCL_PIN			GPIO_Pin_6
#define EEPROM_I2C_GPIO_SDA_PIN			GPIO_Pin_7

void I2C_Config(void);
void EEPROM_Byte_Write(uint8_t addr,uint8_t data);
void EEPROM_Write(uint8_t addr,uint8_t *data,uint8_t num);
void EEPROM_ReadByte(uint8_t addr,uint8_t *data,uint32_t num);
void I2C_EEPROM_BufferWrite(uint8_t WriteAddr,uint8_t *pBuffer,uint16_t NumByteToWrite);
void EEPROM_WaitForWrite_END(void);


#endif


```



```c
#include "bsp_i2c.h"
#include "bsp_led.h"



void I2C_Config()
{
	//初始化EEPROM的GPIO引脚 
	//初始化I2C
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStruct;
	//开启GPIO，I2C时钟
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,DISABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	
	//SCL
	GPIO_InitStructure.GPIO_Pin = EEPROM_I2C_GPIO_SCL_PIN ;//PB6
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(EEPROM_I2C_GPIO_SCL_PORT,&GPIO_InitStructure);
	
	//SDA
	GPIO_InitStructure.GPIO_Pin = EEPROM_I2C_GPIO_SDA_PIN ;//PB7
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(EEPROM_I2C_GPIO_SDA_PORT,&GPIO_InitStructure);

	I2C_InitStruct.I2C_ClockSpeed = 400000;
	I2C_InitStruct.I2C_Mode				= I2C_Mode_I2C; //I2C模式还是MBus模式
	I2C_InitStruct.I2C_DutyCycle  = I2C_DutyCycle_2; //占空比
	I2C_InitStruct.I2C_OwnAddress1 = EEPROM_I2Cx_ADDRESS; //地址随便给，只要不和总线上其他设备一样
	I2C_InitStruct.I2C_Ack         = I2C_Ack_Enable;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;//7or10位地址
	I2C_Init(EEPROM_I2Cx,&I2C_InitStruct);
	
	I2C_Cmd(EEPROM_I2Cx,ENABLE);
}





//从EEPROM写入数据
void EEPROM_Byte_Write(uint8_t addr,uint8_t data)
{
	I2C_GenerateSTART(EEPROM_I2Cx,ENABLE);//起始信号

	
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_MODE_SELECT)==ERROR);//EV5 等待起始信号发送完成
	
	I2C_Send7bitAddress(EEPROM_I2Cx,EEPROM_ADDRESS,I2C_Direction_Transmitter);	//发送EEPROM从机地址
	
	//因为EV8是指数据寄存器为空，此事件比较普遍，一般都会发生，所以此处略去
	
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR);//EV6 等待地址发送完成

	I2C_SendData(EEPROM_I2Cx,addr);	//发送要操作的地址
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR);//EV8 

	I2C_SendData(EEPROM_I2Cx,data);	//发送数据
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);//EV8_2 
	
	I2C_GenerateSTOP(EEPROM_I2Cx,ENABLE);
}

//页写
void EEPROM_Write(uint8_t addr,uint8_t *data,uint8_t num)
{
	I2C_GenerateSTART(EEPROM_I2Cx,ENABLE);//起始信号
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_MODE_SELECT) == ERROR);//EV5 等待起始信号发送完成
	
	I2C_Send7bitAddress(EEPROM_I2Cx,EEPROM_ADDRESS,I2C_Direction_Transmitter);	//发送EEPROM从机设备地址
	
	//因为EV8是指数据寄存器为空，此事件比较普遍，一般都会发生，所以此处略去
	
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR);//EV6 等待地址发送完成

	I2C_SendData(EEPROM_I2Cx,addr);	//发送要操作的地址(字地址)
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR);//EV8 
	
	while(num)
	{
		I2C_SendData(EEPROM_I2Cx,*data);
		while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);
		data++;//数组地址++
		num--;
	}
	I2C_GenerateSTOP(EEPROM_I2Cx,ENABLE);
}

//从EEPROM读取数据
void EEPROM_ReadByte(uint8_t addr,uint8_t *data,uint32_t num)
{
	I2C_GenerateSTART(EEPROM_I2Cx,ENABLE);//起始信号
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_MODE_SELECT) == ERROR);//EV5 等待起始信号发送完成
	
	I2C_Send7bitAddress(EEPROM_I2Cx,EEPROM_ADDRESS,I2C_Direction_Transmitter);	//发送EEPROM从机设备地址
	
	
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR);//EV6 等待地址发送完成

	I2C_SendData(EEPROM_I2Cx,addr);	//发送要操作的地址-字地址
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR);//EV8 

	I2C_GenerateSTART(EEPROM_I2Cx,ENABLE);//第二次起始信号
	while(I2C_CheckEvent(EEPROM_I2Cx,I2C_EVENT_MASTER_MODE_SELECT) == ERROR);//EV5 等待起始信号发送完成
	

	I2C_Send7bitAddress(EEPROM_I2Cx, EEPROM_ADDRESS, I2C_Direction_Receiver);	//发送地址
	while(I2C_CheckEvent(EEPROM_I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)==ERROR);//EV6
	
	while(num)
	{
		if(num==1)
		{
			I2C_AcknowledgeConfig(EEPROM_I2Cx, DISABLE);	//提前设置不响应
		}
		while(I2C_CheckEvent(EEPROM_I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)==ERROR); //EV7
		*data = I2C_ReceiveData(EEPROM_I2Cx);
		data++;
		num--;
	}
	I2C_GenerateSTOP(EEPROM_I2Cx, ENABLE);
	
	I2C_AcknowledgeConfig(EEPROM_I2Cx, ENABLE);	//恢复自动响应	
}

/**
  * @brief   将缓冲区中的数据写到I2C EEPROM中
  * @param   
  *		@arg pBuffer:缓冲区指针
  *		@arg WriteAddr:写地址
  *     @arg NumByteToWrite:写的字节数
  * @retval  无
  *
***************整体思路************y********
1.先算地址是否对齐，若不对齐就算出对齐需要的数据个数->count
2.算出页数，不足一页的数据数
3.若是地址对齐了，若只有一页就直接写入
	若有多页且存在非整页数，就先写入整页的数据
	然后while循环地址一次加一页，页数依次减少,直到页数"小于"1，再把不足一页的写入
4.若是地址未对齐，若只有一页就直接写入，若有多页且存在非整页数，就先输入count个数据补齐地址
	此时总个数和地址记得减去count个
	此时对齐了，就重复步骤三的后部分就OKl了
*********************************************/
void I2C_EEPROM_BufferWrite(uint8_t WriteAddr,uint8_t *pBuffer,uint16_t NumByteToWrite)
{
	uint8_t NumOfPage = 0,NumOfSingle = 0,Addr = 0,count = 0,I2C_PageSize = 8;
	
	Addr = WriteAddr % I2C_PageSize;//判断是否地址对齐，Addr->多出来的地址，也可以说是字节个数
	count = I2C_PageSize - Addr; // 需要填充的地址数目
  NumOfPage =  NumByteToWrite / I2C_PageSize; //页数
  NumOfSingle = NumByteToWrite % I2C_PageSize; //不足一页的数据-非整页
	
	
	/*地址对齐*/
	if(Addr == 0)
	{
		//写的字节数小于一页所能写的最大字节数,即页数"小于"1
		if(NumOfPage == 0)
		{
			EEPROM_Write(WriteAddr,pBuffer,NumOfSingle);
			EEPROM_WaitForWrite_END();//等待写入完成
		}
		//写的字节数大于一页所能写的最大字节数，即NumOfPage != 0
		else
		{
			//先把整页的写完
			while(NumOfPage--)//a--   ***先使用后减减***
			{
				EEPROM_Write(WriteAddr,pBuffer,I2C_PageSize);
				EEPROM_WaitForWrite_END();//等待写入完成
				/*换下一页的地址*/
				WriteAddr += I2C_PageSize;
				pBuffer	  += I2C_PageSize;
			}
			//上面while循环写到NumOfPage = 0 时停下，若此时有不足一页的数据，就写完(上一次循环完成之前已经把地址换到了下一页了)
			if(NumOfSingle != 0)
			{
				EEPROM_Write(WriteAddr,pBuffer,NumOfSingle);
				EEPROM_WaitForWrite_END();//等待写入完成
			}
		}
	}
	/*地址没有对齐*/
	else
	{
		//写的字节数小于一页所能写的最大字节数,即页数"小于"1
		//经测验可知若写入的数据小于页容量，则不用地址对齐
		if(NumOfPage == 0)
		{
			EEPROM_Write(WriteAddr,pBuffer,NumOfSingle);
			EEPROM_WaitForWrite_END();//等待写入完成
		}
		//写的字节数大于一页所能写的最大字节数，即NumOfPage != 0
		else
		{
			NumByteToWrite -= count; //
      NumOfPage =  NumByteToWrite / I2C_PageSize;
      NumOfSingle = NumByteToWrite % I2C_PageSize;	
			

			if(count != 0)
			{
				EEPROM_Write(WriteAddr,pBuffer,count);//先写入count个数据-把地址补齐
        EEPROM_WaitForWrite_END();
        WriteAddr += count;//写入地址不齐
        pBuffer += count;//地址补齐
			}
			/*下面的同上方地址对齐的一致*/
			while(NumOfPage--)
			{
				EEPROM_Write(WriteAddr,pBuffer,I2C_PageSize);
				EEPROM_WaitForWrite_END();//等待写入完成
				/*换下一页的地址*/
				WriteAddr += I2C_PageSize;
				pBuffer	  += I2C_PageSize;
			}
			//上面while循环写到NumOfPage = 0 时停下，若此时有不足一页的数据，就写完(上一次循环完成之前已经把地址换到了下一页了)
			if(NumOfSingle != 0)
			{
				EEPROM_Write(WriteAddr,pBuffer,NumOfSingle);
				EEPROM_WaitForWrite_END();//等待写入完成
			}
		}
	}
}

void EEPROM_WaitForWrite_END(void)
{
	do{
		
		I2C_GenerateSTART(EEPROM_I2Cx, ENABLE);
		while(I2C_GetFlagStatus(EEPROM_I2Cx, I2C_FLAG_SB)==RESET);//EV5
		I2C_ClearFlag(EEPROM_I2Cx, I2C_FLAG_SB);//清楚标志位

		//发送地址, 这时候为读, 如果间隔较小会在这一步没有应答
		I2C_Send7bitAddress(EEPROM_I2Cx, EEPROM_ADDRESS, I2C_Direction_Transmitter);
	}
	while(I2C_GetFlagStatus(EEPROM_I2Cx, I2C_FLAG_ADDR) == RESET);//检测是否收到EEPROM的写完之后的应答
	
	//实现之后直接结束这一次==测试没有也行
	//I2C_GenerateSTOP(EEPROM_I2Cx, ENABLE);
}


```

### 软件

```c
#ifndef _BSP_I2C_GPIO_H
#define _BSP_I2C_GPIO_H


#include <inttypes.h>


#define EEPROM_I2C_WR	0		/* 写控制bit */
#define EEPROM_I2C_RD	1		/* 读控制bit */


/* 定义I2C总线连接的GPIO端口, 用户只需要修改下面4行代码即可任意改变SCL和SDA的引脚 */
#define EEPROM_GPIO_PORT_I2C	GPIOB			/* GPIO端口 */
#define EEPROM_RCC_I2C_PORT 	RCC_APB2Periph_GPIOB		/* GPIO端口时钟 */
#define EEPROM_I2C_SCL_PIN		GPIO_Pin_6			/* 连接到SCL时钟线的GPIO */
#define EEPROM_I2C_SDA_PIN		GPIO_Pin_7			/* 连接到SDA数据线的GPIO */


/* 定义读写SCL和SDA的宏，已增加代码的可移植性和可阅读性 */
#define EEPROM_I2C_SCL_1()  GPIO_SetBits(EEPROM_GPIO_PORT_I2C, EEPROM_I2C_SCL_PIN)		/* SCL = 1*/
#define EEPROM_I2C_SCL_0()  GPIO_ResetBits(EEPROM_GPIO_PORT_I2C, EEPROM_I2C_SCL_PIN)	/* SCL = 0*/
#define EEPROM_I2C_SDA_1()  GPIO_SetBits(EEPROM_GPIO_PORT_I2C, EEPROM_I2C_SDA_PIN)		/* SDA = 1*/
#define EEPROM_I2C_SDA_0()  GPIO_ResetBits(EEPROM_GPIO_PORT_I2C, EEPROM_I2C_SDA_PIN)	/* SDA = 0*/

/* 读SDA口线状态 */
#define EEPROM_I2C_SDA_READ()  GPIO_ReadInputDataBit(EEPROM_GPIO_PORT_I2C, EEPROM_I2C_SDA_PIN)	



void i2c_Start(void);
void i2c_Stop(void);
void i2c_SendByte(uint8_t _ucByte);
uint8_t i2c_ReadByte(void);
uint8_t i2c_WaitAck(void);
void i2c_Ack(void);
void i2c_NAck(void);
uint8_t i2c_CheckDevice(uint8_t _Address);


#endif
```



```c
/*
	应用说明：
	在访问I2C设备前，请先调用 i2c_CheckDevice() 检测I2C设备是否正常，该函数会配置GPIO
*/

#include "bsp_i2c_gpio.h"
#include "stm32f10x.h"

static void i2c_CfgGpio(void);


/*
*********************************************************************************************************
*	函 数 名: i2c_Delay
*	功能说明: I2C总线位延迟，最快400KHz
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void i2c_Delay(void)
{
	uint8_t i;

	/*　
	 	下面的时间是通过逻辑分析仪测试得到的。
    工作条件：CPU主频72MHz ，MDK编译环境，1级优化
  
		循环次数为10时，SCL频率 = 205KHz 
		循环次数为7时，SCL频率 = 347KHz， SCL高电平时间1.5us，SCL低电平时间2.87us 
	 	循环次数为5时，SCL频率 = 421KHz， SCL高电平时间1.25us，SCL低电平时间2.375us 
	*/
	for (i = 0; i < 10; i++);
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Start
*	功能说明: CPU发起I2C总线启动信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_Start(void)
{
	/* 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号 */
	EEPROM_I2C_SDA_1();
	EEPROM_I2C_SCL_1();
	i2c_Delay();
	EEPROM_I2C_SDA_0();
	i2c_Delay();
	EEPROM_I2C_SCL_0();
	i2c_Delay();
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Stop
*	功能说明: CPU发起I2C总线停止信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_Stop(void)
{
	/* 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号 */
	EEPROM_I2C_SDA_0();
	EEPROM_I2C_SCL_1();
	i2c_Delay();
	EEPROM_I2C_SDA_1();
}

/*
*********************************************************************************************************
*	函 数 名: i2c_SendByte
*	功能说明: CPU向I2C总线设备发送8bit数据
*	形    参：_ucByte ： 等待发送的字节
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_SendByte(uint8_t _ucByte)
{
	uint8_t i;

	/* 先发送字节的高位bit7 */
	for (i = 0; i < 8; i++)
	{		
		if (_ucByte & 0x80)
		{
			EEPROM_I2C_SDA_1();
		}
		else
		{
			EEPROM_I2C_SDA_0();
		}
		i2c_Delay();
		EEPROM_I2C_SCL_1();
		i2c_Delay();	
		EEPROM_I2C_SCL_0();
		if (i == 7)
		{
			 EEPROM_I2C_SDA_1(); // 释放总线
		}
		_ucByte <<= 1;	/* 左移一个bit */
		i2c_Delay();
	}
}

/*
*********************************************************************************************************
*	函 数 名: i2c_ReadByte
*	功能说明: CPU从I2C总线设备读取8bit数据
*	形    参：无
*	返 回 值: 读到的数据
*********************************************************************************************************
*/
uint8_t i2c_ReadByte(void)
{
	uint8_t i;
	uint8_t value;

	/* 读到第1个bit为数据的bit7 */
	value = 0;
	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		EEPROM_I2C_SCL_1();
		i2c_Delay();
		if (EEPROM_I2C_SDA_READ())
		{
			value++;
		}
		EEPROM_I2C_SCL_0();
		i2c_Delay();
	}
	return value;
}

/*
*********************************************************************************************************
*	函 数 名: i2c_WaitAck
*	功能说明: CPU产生一个时钟，并读取器件的ACK应答信号
*	形    参：无
*	返 回 值: 返回0表示正确应答，1表示无器件响应
*********************************************************************************************************
*/
uint8_t i2c_WaitAck(void)
{
	uint8_t re;

	EEPROM_I2C_SDA_1();	/* CPU释放SDA总线 */
	i2c_Delay();
	EEPROM_I2C_SCL_1();	/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
	i2c_Delay();
	if (EEPROM_I2C_SDA_READ())	/* CPU读取SDA口线状态 */
	{
		re = 1;
	}
	else
	{
		re = 0;
	}
	EEPROM_I2C_SCL_0();
	i2c_Delay();
	return re;
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Ack
*	功能说明: CPU发送一个ACK信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_Ack(void)
{
	EEPROM_I2C_SDA_0();	/* CPU驱动SDA = 0 */
	i2c_Delay();
	EEPROM_I2C_SCL_1();	/* CPU产生1个时钟 */
	i2c_Delay();
	EEPROM_I2C_SCL_0();
	i2c_Delay();
	EEPROM_I2C_SDA_1();	/* CPU释放SDA总线 */
}

/*
*********************************************************************************************************
*	函 数 名: i2c_NAck
*	功能说明: CPU产生1个NACK信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_NAck(void)
{
	EEPROM_I2C_SDA_1();	/* CPU驱动SDA = 1 */
	i2c_Delay();
	EEPROM_I2C_SCL_1();	/* CPU产生1个时钟 */
	i2c_Delay();
	EEPROM_I2C_SCL_0();
	i2c_Delay();	
}

/*
*********************************************************************************************************
*	函 数 名: i2c_CfgGpio
*	功能说明: 配置I2C总线的GPIO，采用模拟IO的方式实现
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void i2c_CfgGpio(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(EEPROM_RCC_I2C_PORT, ENABLE);	/* 打开GPIO时钟 */

	GPIO_InitStructure.GPIO_Pin = EEPROM_I2C_SCL_PIN | EEPROM_I2C_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  	/* 开漏输出 */
	GPIO_Init(EEPROM_GPIO_PORT_I2C, &GPIO_InitStructure);

	/* 给一个停止信号, 复位I2C总线上的所有设备到待机模式 */
	i2c_Stop();
}

/*
*********************************************************************************************************
*	函 数 名: i2c_CheckDevice
*	功能说明: 检测I2C总线设备，CPU向发送设备地址，然后读取设备应答来判断该设备是否存在
*	形    参：_Address：设备的I2C总线地址
*	返 回 值: 返回值 0 表示正确， 返回1表示未探测到
*********************************************************************************************************
*/
uint8_t i2c_CheckDevice(uint8_t _Address)
{
	uint8_t ucAck;

	i2c_CfgGpio();		/* 配置GPIO */

	
	i2c_Start();		/* 发送启动信号 */

	/* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
	i2c_SendByte(_Address | EEPROM_I2C_WR);
	ucAck = i2c_WaitAck();	/* 检测设备的ACK应答 */

	i2c_Stop();			/* 发送停止信号 */

	return ucAck;
}

```

```c
/**
  * @brief  AT24C02写入一个字节
  * @param  WordAddress 要写入字节的地址
  * @param  Data 要写入的数据
  * @retval 无
  */
void AT24C02_WriteByte(unsigned char WordAddress,Data)
{
	i2c_Start();
	i2i_SendByte(AT24C02_ADDRESS);
	i2c_WaitAck();
	i2i_SendByte(WordAddress);
	i2c_WaitAck();
	i2i_SendByte(Data);
	i2c_WaitAck();
	i2i_Stop();
}

/**
  * @brief  AT24C02读取一个字节
  * @param  WordAddress 要读出字节的地址
  * @retval 读出的数据
  */
unsigned char AT24C02_ReadByte(unsigned char WordAddress)
{
	unsigned char Data;
	i2c_Start();
	i2c_SendByte(AT24C02_ADDRESS);
	i2c_WaitAck();
	i2c_SendByte(WordAddress);
	i2c_WaitAck();
	i2c_Start();
	i2c_SendByte(AT24C02_ADDRESS|0x01);
	i2c_WaitAck();
	Data=i2c_ReadByte();
	i2c_NAck();
	i2c_Stop();
	return Data;
}
```

