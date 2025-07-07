# 以太网DMA

从网络层传输信息到MAC内核，通过的就是以太网DMA，从MAC内核到网络层也是通过以太网DMA完成的，属于存储器到存储器的传输

而DMA的缓冲区就是由以太网DMA描述符管理的

# 以太网DMA描述符

- 发送：不需要CPU的参与下，把描述符指向的缓冲区数据传输到Tx FIFO当中
- 接收：不需要CPU的参与下，将Rx FIFO中的数据传输到描述符指向的缓冲区当中

![image-20250418202429343](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504182024393.png)

## 结构体

```c
//stm32f4/f7/h7xx_hal_eth.h

typedef struct 
{ 
	__IO uint32_t Status; 		/* 状态 */ 
	uint32_t ControlBufferSize; 	/* 缓冲区1和2的大小 */ 
	uint32_t Buffer1Addr; 		/* 缓冲区1的地址 */ 
	uint32_t Buffer2NextDescAddr; /* 缓冲区2的地址/指向下一个描述符 */ 
	/* 以下成员变量为增强描述符的 */ 
    	/* ……………………*/
} 
ETH_DMADescTypeDef;
```

![image-20250419114448310](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504191144373.png)

![image-20250419114503109](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504191145169.png)

## 结构

环形单向链表，但其实不是链表，因为他的成员不是指针

![image-20250419114846611](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504191148645.png)

![image-20250419115024608](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504191150656.png)

在实际实现的时候, 使用ETH_InitTypeDef（ETH_HandleTypeDef）里面的两个变量, RxDesc和TxDesc进行追踪