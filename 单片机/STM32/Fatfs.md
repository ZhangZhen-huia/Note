# 文件系统

- 文件系统是为了存储和管理数据，而在存储介质建立的一种组织结构，这些结构包括操作系统引导区、目录和文件
- 常见的 windows 下的文件系统格式 包括 FAT32、NTFS、exFAT
- 在使用文件系统前，要先对存储介质进行格式化,先擦除原来内容，在存储介质上新建一个文件分配表和目录。这样，文件系统就可以记录数据存放的物理地址，剩余空间。
- 实际结构：
  - 跟链表类似
    - 文件分配表，在文件不是连续的时候只利用目录来读取文件并不完整, 所以创建文件分配表来记录文件的位置, 以及下一个存储文件的位置
    - 目录：记录文件的开始簇, 大小, 日期, 属性等

# Fatfs

使用的是c语言的文件操作函数

![image-20250224112722190](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202502241127342.png)

![image-20250224113557458](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202502241135483.png)

doc:帮助文档

src:源码

![image-20250224113621467](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202502241136503.png)

> history: 文件版本信息
>
> readme: 帮助信息
>
> integer.h 变量typedef
>
> diskio.c: 底层操作函数, 需要自己实现
>
> ff.c核心文件, 实现管理方法, 文件系统和底层的转换, 根据配置实现函数
>
> ffconf.h:配置文件，宏定义，和rtos的差不多

![image-20250224113725860](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202502241137891.png)

cc936.c:本文件在option目录下，是简体中文支持所需要添加的文件，包含了简体中文的GBK和Unicode相互转换功能函数。

> fatfs程序结构图

![image-20250224114634083](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202502241146129.png)

# 移植

![](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202502241328405.png)

![image-20250224121652197](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202502241216228.png)

官方源码是一个实例文件，所以有些头文件我们没有，就注释掉

范例文件缺少了一个函数，手动添加进去

![image-20250224122705781](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202502241227810.png)

## 底层函数实现

### disk_initialize()

![image-20250224122840025](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202502241228083.png)

- 参数是要初始化的内存编号，使得该编号的内存可读可写，然后返回值把STA_NOINIT清零
- 由ff.c里面的上层函数f_mount调用

```c
DSTATUS disk_initialize (BYTE pdrv)
{
	DSTATUS stat;
	int result;
	switch (pdrv) {
	case SD_CARD :
		return stat;
	case SPI_FLASH :
		SPI_FLASH_Init();//初始化
		SPI_Flash_WAKEUP();//唤醒
		stat = disk_status(SPI_FLASH);		
		return stat;
	}
	return STA_NOINIT;
}
```

### disk_status()

![image-20250224124643369](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202502241246436.png)

- 参数是要获取状态的内存编号
- 初始化正常就返回0，若不正常就
  - 返回值有STA_NOINIT，STA_PROTECT，STA_NODISK
  - Fatfs只有STA_NOINIT和STA_PROTECT两种状态

```c
DSTATUS disk_status (BYTE pdrv)
{
	DSTATUS stat;
	int result;

	switch (pdrv) {
	case SD_CARD :
		return stat;
	case SPI_FLASH :
		if(SPI_FLASH_ReadID() == sFLASH_ID)
		{
			stat = 0;
		}
		else
		{
			stat = STA_NOINIT;
		}
		return stat;
	}
	return STA_NOINIT;
}
```

### disk_read()

![image-20250224125149651](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202502241251716.png)

- 要读取的内存编号，接收数据的buff指针，开始读取的扇区编号（不能超过32位），读取扇区的数量（ 1 to 128）

- 返回值，RES_OK ，RES_ERROR，RES_NOTRDY，RES_PARERR

  ```c
  /* Results of Disk Functions */
  typedef enum {
  	RES_OK = 0,		/* 0: 成功 */
  	RES_ERROR,		/* 1: 读写错误 */
  	RES_WRPRT,		/* 2: 写保护 */
  	RES_NOTRDY,		/* 3: 未初始化 */
  	RES_PARERR		/* 4: 参数错误 */
  } DRESULT;
  ```

  ```c
  DRESULT disk_read (BYTE pdrv,BYTE *buff,	DWORD sector,UINT count)
  {
  	DRESULT res;
  //	int result;
  
  	switch (pdrv) {
  	case SD_CARD :
  		return res;
              
  	case SPI_FLASH :
  	//一个扇区是4096个字节，我们实现的spi读flash函数是一个字节一个字节的读
  	//所以第n个扇区的地址就是sector*4096，要读取的字节数就是要读取的扇区数×4096个字节
  		SPI_FLASH_BufferRead(buff,sector*4096,count*4096);
  		res = RES_OK;//这里我们认为flash读取一定能成功，所以没有判断，若是想要严谨些，那么就需要判断一下读取状态然后再返回值
  		return res;
  	}
  	return RES_PARERR;
  }
  
  ```

  ### disk_write()

  ![image-20250224131411936](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202502241314002.png)

- 要写入的内存编号，要写入数据的buff指针，开始写入的扇区编号（不能超过32位），写入扇区的数量（ 1 to 128）
- 返回值，RES_OK ，RES_ERROR，RES_NOTRDY，RES_PARERR
- 当_FS_READONLY == 1时，该函数不可用

```c
#if _USE_WRITE
DRESULT disk_write (BYTE pdrv,const BYTE *buff,DWORD sector,UINT count)
{
	DRESULT res;
//	int result;

	switch (pdrv) {
	case SD_CARD :
		return res;

	case SPI_FLASH :
		//一个扇区是4096个字节，我们实现的spi读flash函数是一个字节一个字节的读
	//所以第n个扇区的地址就是sector*4096，要写入的字节数就是要读取的扇区数×4096个字节
     //写入之前要先擦除
		SPI_FLASH_SectorErase(sector*4096);	
		SPI_FLASH_BufferWrite((u8 *)buff,sector*4096,count*4096);
		res = RES_OK;
		return res;
	}

	return RES_PARERR;
}
#endif
```

### disk_ioctl()

![image-20250224131718103](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202502241317218.png)

- 通过一些命令来实现一些杂项功能
  - CTRL_SYNC: 用来确保信息已经保存进入了扇区而不是内存，（可以理解为确保我们windows写文件后是否点击了保存）
  - GET_SECTOR_COUNT: 获取扇区的数量
  - GET_SECTOR_SIZE:若是最大扇区和最小扇区大小不一样，需要告诉上层每个扇区有多大
  - GET_BLOCK_SIZE: 每次擦除的块的大小

```c

#if _USE_IOCTL
DRESULT disk_ioctl (BYTE pdrv,BYTE cmd,void *buff)
{
	DRESULT res;
//	int result;

	switch (pdrv) {
	case SD_CARD :
		return res;

	case SPI_FLASH :
				switch(cmd)
				{
					case CTRL_SYNC:
							
					
					//返回扇区大小
					case GET_SECTOR_COUNT:
						//flash为8M字节，一个扇区4kb字节，所以一共8*1024/4 = 2048个
						*(DWORD*)buff = 2048;
						break;
					
					//每个扇区的大小
					case GET_SECTOR_SIZE:
						*(WORD*)buff = 4096;
						break;
					
					//返回最小擦除块的大小(flash最小擦除单位是扇区)
					case GET_BLOCK_SIZE:
						*(DWORD*)buff = 1;
					
				}

			res = RES_OK;
		return res;
	}

	return RES_PARERR;
}
```

### 直接使用

```c
filefeedback = f_mount(&file,"1:",1);
```

返回值为：11 FR_INVALID_DRIVE，逻辑设备号不可用

原因：默认的配置是只有一个设备，而我们用了两个设备，一个SD卡，一个Flash

```c
#define _VOLUMES	1 /* Number of volumes (logical drives) to be used. */
```

改成2之后卡死了, 原因是溢出, 具体是扇区返回值和初始化的值不一样

```c
#define	_MIN_SS		512
#define	_MAX_SS		512	//改成4096，一个扇区4096个字节
```

然后返回13 FR_NO_FILESYSTEM，当前设备上不存在文件系统

接着使用f_mkfs函数格式化flash，使用前

```c
#define	_USE_MKFS		1
/* This option switches f_mkfs() function. (0:Disable or 1:Enable) */
```

```c
FRESULT f_mkfs (
  const TCHAR* path,  /*设备号*/
  BYTE  sfd,          /* FDISK(0) or SFD(1)，使用软盘用SFD，其他都用FDISK*/
  UINT  au            /*每个簇的大小，若是给0就自动分配*/
);
```

```c
	if(f_mount(&file,"1:",1) ==13)
	{
		filefeedback = f_mkfs("1:",0,0);
		
		//格式化之后要重新挂载文件系统
		f_mount(NULL,"1:",1);
		f_mount(&file,"1:",1)
	}
```

```c
/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "timers.h"

/* 开发板硬件bsp头文件 */
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_key.h"
#include "limits.h"
#include "bsp_spi.h"
#include "ff.h"
/*任务*/

TaskHandle_t ReadFlashTaskHandle;
void ReadFlashTask(void* arg);


StaticTask_t Idle_Task_TCB;
StackType_t Idle_Task_Stack[configMINIMAL_STACK_SIZE];

StaticTask_t Timer_Task_TCB;
StackType_t Timer_Task_Stack[configTIMER_TASK_STACK_DEPTH ];

void AppTaskCreatTask(void*arg);
TaskHandle_t StartTaskHandle;



static void BSP_Init(void);/* 用于初始化板载相关资源 */


FATFS file;
FIL fp;
FRESULT filefeedback;
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, 
																	 StackType_t **ppxIdleTaskStackBuffer, 
																	 uint32_t *pulIdleTaskStackSize);


int main(void)
{	
	
	
  /* 开发板硬件初始化 */
  	BSP_Init();

	if(xTaskCreate(AppTaskCreatTask,"StartTask",512,NULL,1,&StartTaskHandle)!= errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)
		vTaskStartScheduler();
	else
		return -1;
  while(1);   /* 正常不会执行到这里 */    
}


void AppTaskCreatTask(void*arg)
{

	taskENTER_CRITICAL();
	if((xTaskCreate(ReadFlashTask,"ReadFlash",1024,NULL,2,&ReadFlashTaskHandle)) != errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)



	vTaskDelete(NULL);
	taskEXIT_CRITICAL();
	while (1)
	{
		/* code */
	}
	
}

void ReadFlashTask(void* arg)
{
	UINT bw,br;
	char Writebyte[] = "现在是2025年2月24日!\n";
	char Readbyte[200] = "";
	f_open(&fp,"1:Hui.txt",FA_READ | FA_OPEN_ALWAYS | FA_WRITE);
	while(1)
	{
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
		{
			f_write(&fp,Writebyte,sizeof(Writebyte),&bw);
			
		}
		else if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)
		{
			f_lseek(&fp,0);
			f_read(&fp,Readbyte,f_size(&fp),&br);
			f_close(&fp);
		}	
		vTaskDelay(10);
	}	
}

static void BSP_Init(void)
{


	/*
	 * STM32中断优先级分组为4，即4bit都用来表示抢占优先级，范围为：0~15
	 * 优先级分组只需要分组一次即可，以后如果有其他的任务需要用到中断，
	 * 都统一用这个优先级分组，千万不要再分组，切忌。
	 */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	
	/* LED 初始化 */
	LED_GPIO_Config();

	/* 按键初始化 */
	Key_GPIO_Config();
	
	/* 串口初始化	*/
	USART_Config();
	
	/* SPI初始化 */
	SPI_FLASH_Init();
	
	if(f_mount(&file,"1:",1) ==13)
	{
		filefeedback = f_mkfs("1:",0,0);
		
		//格式化之后要重新挂载文件系统
		f_mount(NULL,"1:",1);
		f_mount(&file,"1:",1);
	}
}
 
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, 
								   StackType_t **ppxIdleTaskStackBuffer, 
								   uint32_t *pulIdleTaskStackSize)
{
	*ppxIdleTaskTCBBuffer=&Idle_Task_TCB;/* 任务控制块内存 */
	*ppxIdleTaskStackBuffer=Idle_Task_Stack;/* 任务堆栈内存 */
	*pulIdleTaskStackSize=configMINIMAL_STACK_SIZE;/* 任务堆栈大小 */
}

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
	*ppxTimerTaskTCBBuffer = &Timer_Task_TCB;
	*ppxTimerTaskStackBuffer = Timer_Task_Stack;
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
```

> **注意，如果在任务中使用，注意任务的栈空间，防止栈溢出，建议直接定义成全局变量**

## 实现中文命名

- 添加cc936.c文件
- #define _CODE_PAGE	936
- #define	_USE_LFN	1 //是否支持长文件名，建议存储在全局变量区，因为存在栈区容易溢出
  - 0: 默认不允许长文件命名.
    1: 像全局变量一样存储长文件名
    2: 动态保存在栈区.
    3: 动态保存在堆区（要实现malloc函数和free函数）

# 文件系统的其他操作

文件官网：[fatfs](http://elm-chan.org/fsw/ff/00index_e.html)

![image-20250302153853840](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503021538989.png)