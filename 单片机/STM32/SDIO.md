# SDIO简介

SPI，SDIO都可以和SD卡建立通信

SDIO 全称是安全数字输入/输出接口

![image-20250315101801515](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503151018552.png)

> - 多媒体卡(MMC)，可以说是 SD 卡的前身，现阶段已经用得很少
> - SD  I/O 卡本身不是用于存储的卡，它是指利用 SDIO 传输协议的一种外设。比如 Wi-Fi Card， 它主要是提供 Wi-Fi 功能，有些 Wi-Fi 模块是使用串口或者 SPI 接口进行通信的，但 Wi-Fi  SDIO Card 是使用 SDIO 接口进行通信的，并且一般设计 SD I/O 卡是可以插入到 SD 的插槽
> - CE-ATA 是专为轻薄笔记本硬盘设计的硬盘高速通讯接口

STM32F10x 系列控制器只支持 SD 卡规范版本 2.0，即只支持标准容量 SDSC 和高容量 SDHC 标准卡，不支持超大容量 SDXC 标准卡，所以可以支持的最高卡容量是 32GB

# SD卡物理结构

![image-20250315101913508](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503151019539.png)

- 存储单元：存储数据的部件，存储单元通过存储单元接口与卡控制单元进行数据传输，**NAND FLASH**
- 电源检测单元：保证 SD 卡工作在合适的电压下，如出现掉电或上状态时，它会使控制单元和存储单元接口复位
- 卡及接口控制单元：控制 SD 卡的运行状态，它 包括有 ***8*** 个寄存器
- 接口驱动器：控制 SD 卡引脚的输入输出

## SD卡寄存器

![image-20250315102237214](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503151022258.png)

SD卡的寄存器并不是像单片机的标准外设一样，一次读取一个寄存器，而是通过命令来访问的

# SDIO总线

SD卡一般都支持SPI和SDIO通信，但是STM32F10x系列只支持SDIO通信

四线并行传输或单线传输

![image-20250315102530347](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503151025391.png)

SD卡使用9pin接口，三根电源，四根数据，一根CLK，一根CMD

- CLK：时钟线，由SDIO主机产生即stm32，***上升沿有限***
- CMD：命令控制线，主机向SD卡发送命令，如果命令要求有应答，应答也从CMD线返回
- D0-3：数据线，D0拉低表示忙
- VDD，VSS1，VSS2：电源线和地线

***CLK只在上升沿有效***

SD卡操作过程中会使用到两种时钟频率

- 卡识别阶段：FOD，最高为400kHz，为了适应不同读写速度的SD卡
- 数据传输阶段：FPP，默认最高为25MHz，如果通过相关寄存器配置使 SDIO 工作在高速模式，此时数据传输模式最高频率为 50MHz

SDIO通信速率远大于SPI，即使SPI的是时钟频率最高36Mhz大于SDIO，但是SDIO是四线并行传输，而SPI是单线串行传输，所以SDIO的传输速率也大于SPI

> 对于 STM32 控制器只有一个 SDIO 主机，所以只能连接一个 SDIO 设备，开发板上集 成了一个 Micro SD 卡槽和 SDIO 接口的 WiFi 模块，要求只能使用其中一个设备。SDIO 接 口的 WiFi 模块一般集成有使能线，如果需要用到 SD 卡需要先控制该使能线禁用 WiFi 模 块。

## 总线协议

SD 总线通信是基于命令和数据传输的，通信由“0”作为起始位，由“1”作为停止位，一般是由主机发送一个命令，然后从机返回一个响应，如有需要在返回数据

> ![image-20250315104410235](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503151044268.png)
>
> 简单的通信

SD卡数据是以Block为单位来传输的，SDHC卡数据块长度一般为 512 字节，数据可以双向传输，需要CRC校验位来保证数据传输正确，由SD卡硬件自动生成，STM32 控制器可以控制使用单线或 4 线传输

> 多块写入操作
>
> ![image-20250315104612805](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503151046845.png)

- 数据写入前需要检测 SD 卡忙状态，因为 SD 卡在接收到数据 后编程到存储区过程需要一定操作时间。SD 卡忙状态通过把 D0 线拉低表示
- 数据块读操作与之类似，只是无需忙状态检测

## 数据传输

使用四线并行传输的时候，每次传输4bit数据，每根数据线都需要起始位和停止位和校验位，CRC 位每根数据线都要分别检查，并把检查结果汇总然后在数据传输完后通过 D0 线反馈给主机

### 常规数据包(8bit 宽)

它先发低字节再发高字节，而每个字节则是先发高位再发低位，DAT3 数据线发较高位，DAT0 数据线发较低位

![image-20250315104908689](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503151049734.png)

### 是宽位数据包格式（针对SSR寄存器）

SSR 寄存器总共有 512bit，在主机发出 ***ACMD13*** 命令后 SD 卡将 SSR 寄存器内容通过 DAT 线发送给主机

![image-20250315105050873](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503151050915.png)

## 命令

由主机发出，有广播和寻址命令两种

- 广播命令是针对与 SD 主机总线连 接的所有从设备发送的
- 寻址命令是指定某个地址设备进行命令传输

### 格式

固定48bit，都是通过CMD线连续传输的（数据线不参与）

- ![image-20250315105428630](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503151054666.png)
- 起始0，终止1
- CRC7，7位校验，校准失败，则意味着命令传输失败，SD 卡不执行命令
- 命令号，固定6bit，所以最多2六次方 = 64个命令（代号：CMD0~CMD63），部分命令不适用于 SD 卡操作，只是专门用于 MMC 卡或者 SD I/O 卡
- 地址/参数：每个命令有 32bit 地址信息/参数用于命令附加内容，例如，广播命令 没有地址信息，这 32bit 用于指定参数，而寻址命令这 32bit 用于指定目标 SD 卡 的地址

### 类型

- 无响应广播命令(bc)：发送到所有卡，不返回任务响应
- 带响应广播命令(bcr)：发送到所有卡，同时接收来自所有卡响应
- 寻址命令(ac)：发送到选定卡，DAT 线无数据传输
- 寻址数据传输命令(adtc)：发送到选定卡，DAT 线有数据传输

> 另外，SD 卡主机模块系统旨在为各种应用程序类型提供一个标准接口。在此环境中， 需要有特定的客户/应用程序功能。为实现这些功能，在标准中定义了两种类型的通用命令

***特定应用命令(ACMD)和常规命令(GEN_CMD)***

> 要使用 SD 卡制造商特定的 ACMD 命令如ACMD6，需要在发送该命令之前无发送 CMD55 命令，告知 SD 卡接下来的命令为特定应用命令。CMD55 命令只对紧接的第一个命令有效，SD 卡如果检测到 CMD55 之后的第一 条命令为 ACMD 则执行其特定应用功能，如果检测发现不是 ACMD 命令，则执行标准命令。

***填充位和保留位 都必须被设置为 0***

![image-20250315110028480](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503151100531.png)

![image-20250315110035445](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503151100486.png)

## 响应

- 由SD卡从机向主机发出
- SDIO 总共有 7 个响应类型(代号：R1~R7)，其中 SD 卡没有 R4、R5 类型响应。特定的命令对应有特定的响应类型，由CMD线连续传输
- 根据响应内容大小可以分为短响应 和长响应。短响应是 48bit 长度，只有 R2 类型是长响应，其长度为 136bit
- 除了 R3 类型之外，其他响应都使用 CRC7 校验来校验，对于 R2 类型是使用 CID 和 CSD 寄存器内部 CRC7

![image-20250315110502082](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503151105134.png)

![image-20250315110510899](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503151105949.png)

# SD卡操作模式及切换

STM32控制器对SD卡进行数据读写之前需要识别卡的种类：V1.0标准卡，V2.0标准卡，V2.0高容量卡或者不被识别卡

SD卡系统定义了两者操作模式:卡识别模式和数据传输模式

系统复位后主机处于卡识别模式，寻找总线上可用的SDIO设备，此时SD卡也是识别模式，直到被主机识别到，即SD卡接收到CMD3的命令后，SD卡就会进入数据传输模式

![image-20250316164510844](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503161645959.png)

## 卡识别模式

![image-20250316165528705](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503161655779.png)

> 在卡识别模式下，主机会复位所有处于“卡识别模式”的 SD 卡，确认其工作电压范 围，识别 SD 卡类型，并且获取 SD 卡的相对地址(卡相对地址较短，便于寻址)。在卡识别过程中，要求 SD 卡工作在识别时钟频率 FOD 的状态下

> SD 卡有一个电压 支持范围，主机当前电压必须在该范围可能才能与卡正常通信。SEND_IF_COND(CMD8) 命令就是用于验证卡接口操作条件的(主要是电压支持)。卡会根据命令的参数来检测操作 条件匹配性，如果卡支持主机电压就产生响应，否则不响应。而主机则根据响应内容确定 卡的电压匹配性。CMD8 是 SD 卡标准 V2.0 版本才有的新命令，所以如果主机有接收到响 应，可以判断卡为 V2.0 或更高版本 SD 卡

SD_SEND_OP_COND(ACMD41)：识别或拒绝不匹配它的电压范围的卡。 参数表示设置主机支持电压范围，卡响应会返回卡支持的电压范围

> 对于对 CMD8 有响应的卡，把 ACMD41 命令的 HCS 位设置为 1，可以测试卡的容量类型，如果卡响应的 CCS 位为 1 说明为高容量 SD 卡，否则为标准卡。卡在响应 ACMD41 之后进入准备状态，不响应 ACMD41 的卡为不可用卡，进入无效状态。ACMD41 是应用特定命令，发送该命令之前必须先发 CMD55
>
> 若是卡返回忙或者主机不支持电压，就会返回到空闲状态

CMD2（ALL_SEND_CID）：获取卡的卡识别号（CID），处于准备状态的 卡在发送 CID 之后就进入识别状态

CMD3（SEND_RELATIVE_ADDR）：让卡返回的相对地址（RCA）并响应命令

> 这个 RCA 是 16bit 地址，而 CID 是 128bit 地址，使用 RCA 简化通信。卡在接收到 CMD3 并发出响应后就进入数据传输模式， 并处于待机状态，主机在获取所有卡 RCA 之后也进入数据传输模式

## 数据传输模式

![image-20250316193006049](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503161930104.png)

- CMD3：从识别卡模式进入待机状态
- CMD15：强制进入无效状态
- CMD0：强制进入空闲状态
- CMD7：用来选定和取消指定的卡，用来在传输状态和待机状态进行切换（因为总线上 可能有多个卡都是出于待机状态，必须选择一个RCA 地址目标卡使其进入传输状态才可以 进行数据通信）
- CMD12：强制传输完成
- CMD7：可用强制从编程状态转换到断开连接状态
- 处于数据传输状态下可以将主机频率设置为FPP，默认25MHZ，频率切换可以通过CMD4来切换

# SDIO接口

![image-20250316194508083](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503161945132.png)

- SDIO 适配器提供 SDIO 主机功能，可以提供 SD 时钟、发送命令和进行数据传输
- AHB 接口用于控制器访问 SDIO 适配器寄存器并且可以产生中断和 DMA 请求信号，接到了内存
- SDIO_D[7:0]：STM32控制器的SDIO是针对MMC卡和SD卡的主设备，所以预留有八根数据线，对于SD卡最多用四根数据线

SDIO使用两个时钟信号（手册）

- SDIOCLK：等于HCLK = 72MHZ，可以通过设置SDIO_CLKCR寄存器中的BYPASS位来开关旁路，关闭旁路后，会对SDIOCLK根据CLKDIV的值来进行分频，注意，当处于卡识别模式下，SDIO_CK频率必须在400KHZ以下
  - ![image-20250316203257993](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503162032038.png)
  - ![image-20250316203338926](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503162033973.png)
- HCLK/2：总线时钟二分频 = 36MHZ

## SDIO适配器

![image-20250316195329021](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503161953067.png)

### 控制单元

![image-20250316195543488](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503161955530.png)

- 电源管理部件：会在系统断电和上电阶段禁止 SD 卡总线输出信号
- 时钟管理部件：控制 CLK 线时钟信号生成，一般使 用 SDIOCLK ，其实也可以通过（HCLK/2）分频得到

### 命令路径

![image-20250316195701936](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503161957982.png)

- 适配器寄存器
  - CMD：命令号
  - 参数：命令参数

![image-20250316201011996](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503162010832.png)

### 数据路径

![image-20250316195716926](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503161957968.png)

![image-20250316201058246](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503162010301.png)

### 数据FIFIO

- 先入先出，宽度32bit，深度32bit
- 状态寄存器 (SDIO_STA)的 TXACT 位用于指示当前正在发送数据，RXACT 位指示当前正在接收数据， 这两个位不可能同时为 1
  - 当 **TX**ACT 为 1 时，可以通过 AHB 接口将数据写入到传输 FIFO
  - 当 **RX**ACT 为 1 时，接收 FIFO 存放从数据路径部件接收到的数据

> 根据 FIFO 空或满状态会把 SDIO_STA 寄存器位值 1，并可以产生中断和 DMA 请求

# SDIO结构体

- 初始化结构体 SDIO_InitTypeDef
- 命令初始化结构体 SDIO_CmdInitTypeDef 
- 数据初始化结 构体 SDIO_DataInitTypeDef

##  SDIO_InitTypeDef

```c
typedef struct
{
  uint32_t SDIO_ClockEdge;            /*配置有效时钟沿，一般为高电平 */

  uint32_t SDIO_ClockBypass;          /*旁路时钟使能或者禁用，一般选择禁用时钟分频旁路。*/

  uint32_t SDIO_ClockPowerSave;       /* 使能或禁用节能模式选择，节能模式CLK线只有在总线激活时才有时钟输出*/

  uint32_t SDIO_BusWide;              /*数据线宽度选择，可选1，4，8，默认为1，操作 SD 卡时在数据传输模式下一般选择 4 位数据总线 ，一般在卡识别过程即初始化过程配置成1，在数据传输模式下配置成4，所以初始化函数可能要调用两次*/

  uint32_t SDIO_HardwareFlowControl;  /*选使能或禁用，硬件流控制选择，硬件流控制功能可以避免 FIFO 发送上溢和下溢错误 */

  uint8_t SDIO_ClockDiv;              /*时钟分频系数，CLK 线时钟频率=SDIOCLK/([CLKDIV+2]). */
                                           
} SDIO_InitTypeDef;
```

## SDIO_CmdInitTypeDef 

```c
typedef struct
{
  uint32_t SDIO_Argument;  /*命令参数	SDIO_ARG*/

  uint32_t SDIO_CmdIndex;  /*命令号选择 It must be lower than 0x40. */

  uint32_t SDIO_Response;  /*响应类型，长响应和短响应 SDIO 定义了四个 32 位的 SDIO 响应寄存器(SDIO_RESPx,x=1..4)，短响应只用到 SDIO_RESP1*/

  uint32_t SDIO_Wait;      /*等待类型选择，1、无等待状态，超时检测功能启动；2、等待中断，3、等待传输完成，设定 SDIO_CMD 寄存器的WAITPEND 位和 WAITINT 位的值。*/

  uint32_t SDIO_CPSM;      /*命令路径状态机控制，选使能或禁用 CPSM，设定 SDIO_CMD 寄存器的 CPSMEN 位的值。*/
} SDIO_CmdInitTypeDef;
```

## SDIO_DataInitTypeDef

```c
typedef struct
{
  uint32_t SDIO_DataTimeOut;    /*设置传输数据超时周期，单位是SD卡时钟总线周期（SDIO_DTIMER），在 DPSM 进入 Wait_R 或繁忙状态后开始递减，直到 0 还处于以上两种状态则将超时状态标志置 1 */

  uint32_t SDIO_DataLength;     /*设置传输数据长度，SDIO_DLEN. */
 
  uint32_t SDIO_DataBlockSize;  /*设置数据块大小，不同命令要求的数据块可能不同，SDIO_DCTRL的DBLOCKSIZE位 */
 
  uint32_t SDIO_TransferDir;    /*数据传输方向，SDIO_DCTRL 寄存器的 DTDIR 位的值*/
 
  uint32_t SDIO_TransferMode;   /*数据传输模式，数据块模式/数据流（MMC卡），SD卡用数据块，SDIO_DCTRL的DTMODE位*/
 
  uint32_t SDIO_DPSM;           /*数据路径状态机控制，可选使能或禁用 DPSM	SDIO_DCTRL的DTEN位*/
} SDIO_DataInitTypeDef;
```

# SD卡读写测试

![image-20250322153404114](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503221534218.png)

程序是从ST官方移植过来的

![image-20250415215102918](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504152151474.png)

![image-20250415215157301](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504152151350.png)

## SD_Init()——初始化完后就处于了传输状态，可以进行数据读写了

```c
/**
 * 函数名：SD_Init
 * 描述  ：初始化SD卡，使卡处于就绪状态(准备传输数据)
 * 输入  ：无
 * 输出  ：-SD_Error SD卡错误代码
 *         成功时则为 SD_OK
 * 调用  ：外部调用
 */
SD_Error SD_Init(void)
{
	/*重置SD_Error状态*/
  SD_Error errorstatus = SD_OK;
  //中断配置	  
  NVIC_Configuration();
	
  /* SDIO 外设底层引脚初始化 */
  GPIO_Configuration();

  /*对SDIO的所有寄存器进行复位,注意不是SD_DeInit()*/
  SDIO_DeInit();  
/*------------------------------空闲状态----------------------------------*/
  /*上电并进行卡识别流程，确认卡的操作电压和卡的类型，若是识别完成，卡就进准备状态 */
  errorstatus = SD_PowerON(); 

  /*如果上电，识别不成功，返回“响应超时”错误 */
  if (errorstatus != SD_OK)
  {
    /*!< CMD Response TimeOut (wait for CMDSENT flag) */
    return(errorstatus);	
  }
/*------------------------------准备状态----------------------------------*/
  /*卡识别成功，进行卡初始化    */
  errorstatus = SD_InitializeCards(); 

  if (errorstatus != SD_OK)	  //失败返回
  {
    /*!< CMD Response TimeOut (wait for CMDSENT flag) */
    return(errorstatus);
  }
/*------------------------------待机状态，数据传输模式----------------------------------*/
  /* 配置SDIO外设
   * 上电识别，卡初始化都完成后，进入数据传输模式，提高读写速度
   */
  
  /* SDIOCLK = HCLK, SDIO_CK = HCLK/(2 + SDIO_TRANSFER_CLK_DIV) 72M/3 = 24MHZ < 25MHZ*/  
  SDIO_InitStructure.SDIO_ClockDiv = SDIO_TRANSFER_CLK_DIV;

	/*上升沿采集数据 */
  SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;

	/* Bypass模式使能的话，SDIO_CK不经过SDIO_ClockDiv分频 */
  SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable; 
	
	/* 若开启此功能，在总线空闲时关闭sd_clk时钟 */
  SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;
	
  /* 暂时配置成1bit模式 ，后面才配成4线模式*/	
  SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_1b;

	/* 硬件流，若开启，在FIFO不能进行发送和接收数据时，数据传输暂停 */
  SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable; 
	
  SDIO_Init(&SDIO_InitStructure);
  
  if (errorstatus == SD_OK)
  {
    /* 用来读取csd/cid寄存器 */
    errorstatus = SD_GetCardInfo(&SDCardInfo);	
  }

  if (errorstatus == SD_OK)
  {
    /* 通过cmd7  ,rca选择要操作的卡 */
    errorstatus = SD_SelectDeselect((uint32_t) (SDCardInfo.RCA << 16));	  
  }
    //CMD7用于在待机状态和传输状态之间的切换
/*------------------------------传输状态---------------------------------------------*/

  if (errorstatus == SD_OK)
  {
			/* 最后为了提高读写，开启4bits模式 */
    errorstatus = SD_EnableWideBusOperation(SDIO_BusWide_4b);
  }  

  return(errorstatus);
/*-----------------------------初始化完成，就可以进行数据传输了-------------------------*/
}

```

### SD_Error

```c
typedef enum
{
/** 
  * @brief  SDIO specific error defines  
  */   
  SD_CMD_CRC_FAIL                    = (1), /*!< Command response received (but CRC check failed) */
  SD_DATA_CRC_FAIL                   = (2), /*!< Data bock sent/received (CRC check Failed) */
  SD_CMD_RSP_TIMEOUT                 = (3), /*!< Command response timeout */
  SD_DATA_TIMEOUT                    = (4), /*!< Data time out */
  SD_TX_UNDERRUN                     = (5), /*!< Transmit FIFO under-run */
  SD_RX_OVERRUN                      = (6), /*!< Receive FIFO over-run */
  SD_START_BIT_ERR                   = (7), /*!< Start bit not detected on all data signals in widE bus mode */
  SD_CMD_OUT_OF_RANGE                = (8), /*!< CMD's argument was out of range.*/
  SD_ADDR_MISALIGNED                 = (9), /*!< Misaligned address */
  SD_BLOCK_LEN_ERR                   = (10), /*!< Transferred block length is not allowed for the card or the number of transferred bytes does not match the block length */
  SD_ERASE_SEQ_ERR                   = (11), /*!< An error in the sequence of erase command occurs.*/
  SD_BAD_ERASE_PARAM                 = (12), /*!< An Invalid selection for erase groups */
  SD_WRITE_PROT_VIOLATION            = (13), /*!< Attempt to program a write protect block */
  SD_LOCK_UNLOCK_FAILED              = (14), /*!< Sequence or password error has been detected in unlock command or if there was an attempt to access a locked card */
  SD_COM_CRC_FAILED                  = (15), /*!< CRC check of the previous command failed */
  SD_ILLEGAL_CMD                     = (16), /*!< Command is not legal for the card state */
  SD_CARD_ECC_FAILED                 = (17), /*!< Card internal ECC was applied but failed to correct the data */
  SD_CC_ERROR                        = (18), /*!< Internal card controller error */
  SD_GENERAL_UNKNOWN_ERROR           = (19), /*!< General or Unknown error */
  SD_STREAM_READ_UNDERRUN            = (20), /*!< The card could not sustain data transfer in stream read operation. */
  SD_STREAM_WRITE_OVERRUN            = (21), /*!< The card could not sustain data programming in stream mode */
  SD_CID_CSD_OVERWRITE               = (22), /*!< CID/CSD overwrite error */
  SD_WP_ERASE_SKIP                   = (23), /*!< only partial address space was erased */
  SD_CARD_ECC_DISABLED               = (24), /*!< Command has been executed without using internal ECC */
  SD_ERASE_RESET                     = (25), /*!< Erase sequence was cleared before executing because an out of erase sequence command was received */
  SD_AKE_SEQ_ERROR                   = (26), /*!< Error in sequence of authentication. */
  SD_INVALID_VOLTRANGE               = (27),
  SD_ADDR_OUT_OF_RANGE               = (28),
  SD_SWITCH_ERROR                    = (29),
  SD_SDIO_DISABLED                   = (30),
  SD_SDIO_FUNCTION_BUSY              = (31),
  SD_SDIO_FUNCTION_FAILED            = (32),
  SD_SDIO_UNKNOWN_FUNCTION           = (33),

/** 
  * @brief  Standard error defines   
  */ 
  SD_INTERNAL_ERROR, 
  SD_NOT_CONFIGURED,
  SD_REQUEST_PENDING, 
  SD_REQUEST_NOT_APPLICABLE, 
  SD_INVALID_PARAMETER,  
  SD_UNSUPPORTED_FEATURE,  
  SD_UNSUPPORTED_HW,  
  SD_ERROR,  
  SD_OK = 0 
} SD_Error;
```

### NVIC_Configuration()

```c
/*
 * 函数名：NVIC_Configuration
 * 描述  ：SDIO 优先级配置为最高优先级。
 * 输入  ：无
 * 输出  ：无
 */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure the NVIC Preemption Priority Bits */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
```

### GPIO_Configuration()

```c
static void GPIO_Configuration(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  /*!< GPIOC and GPIOD Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD , ENABLE);

  /*!< Configure PC.08, PC.09, PC.10, PC.11, PC.12 pin: D0, D1, D2, D3, CLK pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /*!< Configure PD.02 CMD line */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_Init(GPIOD, &GPIO_InitStructure); 

  /*!< Enable the SDIO AHB Clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_SDIO, ENABLE);

  /*!< Enable the DMA2 Clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
}
```

### SDIO_DeInit()

复位所有寄存器

SD_DeInit()是复位所有的配置

```c
void SDIO_DeInit(void)
{
  SDIO->POWER = 0x00000000;
  SDIO->CLKCR = 0x00000000;
  SDIO->ARG = 0x00000000;
  SDIO->CMD = 0x00000000;
  SDIO->DTIMER = 0x00000000;
  SDIO->DLEN = 0x00000000;
  SDIO->DCTRL = 0x00000000;
  SDIO->ICR = 0x00C007FF;
  SDIO->MASK = 0x00000000;
}
```

### SD_PowerON()

```c
SD_Error SD_PowerON(void)
{
  SD_Error errorstatus = SD_OK;
  uint32_t response = 0, count = 0, validvoltage = 0;
  uint32_t SDType = SD_STD_CAPACITY;
	
/********************************************************************************************************/
  /* 上电初始化 
   * 配置SDIO的外设
   * SDIOCLK = HCLK, SDIO_CK = HCLK/(2 + SDIO_INIT_CLK_DIV)   
   * 初始化（卡识别过程）时的时钟不能大于400KHz
   */
	/* HCLK = 72MHz, SDIOCLK = 72MHz, SDIO_CK = HCLK/(178 + 2) = 400 KHz ，使用这个分频因子要失能旁路时钟*/
  SDIO_InitStructure.SDIO_ClockDiv = SDIO_INIT_CLK_DIV;
	
  SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
	
	/* 不使用bypass模式，直接用HCLK进行分频得到SDIO_CK */
  SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;

	/* 空闲时不关闭时钟电源 */  
  SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;
	
	/* 初始化的时候暂时先把数据线配置成1根 */
  SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_1b;
	
	/* 失能硬件流控制 */
  SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
	
  SDIO_Init(&SDIO_InitStructure);

  /* 开启SDIO外设的电源 */
  SDIO_SetPowerState(SDIO_PowerState_ON);

  /* 使能 SDIO 时钟 */
  SDIO_ClockCmd(ENABLE);  
/********************************************************************************************************/   
  /* 下面发送一系列命令,开始卡识别流程
   * CMD0: GO_IDLE_STATE(复位所以SD卡进入空闲状态) 
   * 没有响应  ，没有参数
	 */
  SDIO_CmdInitStructure.SDIO_Argument = 0x0;//参数，因为命令固定是48位，所以要填充为0
  SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_GO_IDLE_STATE;//命令号
	
	/* 没有响应 */
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_No;
	
	/* 关闭等待中断 */
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
	
	/* 则CPSM在开始发送命令之前等待数据传输结束 */
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable; 
  SDIO_SendCommand(&SDIO_CmdInitStructure);	  		
	
	/* 检测是否正确接收到cmd0 */
  errorstatus = CmdError();
	
	/* 命令发送出错，返回 */
  if (errorstatus != SD_OK)	
  {
    /* CMD Response TimeOut (wait for CMDSENT flag) */
    return(errorstatus);
  }
/********************************************************************************************************/
  /* CMD8: SEND_IF_COND 判断卡的类型
   * Send CMD8 to verify SD card interface operating condition
	 *          
   * Argument: - [31:12]: Reserved (shall be set to '0')
   *           - [11:8] : Supply Voltage (VHS) 0x1 (Range: 2.7-3.6 V)
   *           - [7:0]  : Check Pattern (recommended 0xAA) 
   * CMD Response: R7 
	 */
	 /* 接收到命令sd会返回这个参数 */
  SDIO_CmdInitStructure.SDIO_Argument = SD_CHECK_PATTERN;
	
  SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SEND_IF_COND;	
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;	 
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;			 				
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);
   
  /*检查是否接收到命令*/
  errorstatus = CmdResp7Error(); 
	
	/* 有响应则card遵循sd协议2.0版本 */
  if (errorstatus == SD_OK)	  	
  {
		/* SD Card 2.0 ，先把它定义会sdsc类型的卡 */
    CardType = SDIO_STD_CAPACITY_SD_CARD_V2_0;
		
		/* 这个变量用作ACMD41的参数，用来询问是sdsc卡还是sdhc卡 */
    SDType = SD_HIGH_CAPACITY;	
  }
  else	/* 无响应，说明是1.x的或mmc的卡 */
  {
    /* 发命令 CMD55 */	  
    SDIO_CmdInitStructure.SDIO_Argument = 0x00;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);
    errorstatus = CmdResp1Error(SD_CMD_APP_CMD);
  }
	
  /* CMD55 		
   * 发送cmd55，用于检测是sd卡还是mmc卡，或是不支持的卡
	 * CMD 响应: R1
   */  
  SDIO_CmdInitStructure.SDIO_Argument = 0x00;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);
	
	/* 是否响应，没响应的是mmc或不支持的卡 */
  errorstatus = CmdResp1Error(SD_CMD_APP_CMD);	
/********************************************************************************************************/
  /* If errorstatus is Command TimeOut, it is a MMC card 
   * If errorstatus is SD_OK it is a SD card: SD card 2.0 (voltage range mismatch)
   * or SD card 1.x 
	 */
  if (errorstatus == SD_OK)	//响应了cmd55，是sd卡，可能为1.x,可能为2.0
  {
  	/*下面开始循环地发送sdio支持的电压范围，循环一定次数*/

    /* SD CARD
     * Send ACMD41 SD_APP_OP_COND with Argument 0x80100000 
		 */
    while ((!validvoltage) && (count < SD_MAX_VOLT_TRIAL))
    {	 
			/* 在发送ACMD命令前都要先向卡发送CMD55 
       * SEND CMD55 APP_CMD with RCA as 0 
			 */
      SDIO_CmdInitStructure.SDIO_Argument = 0x00;
      SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;	  
      SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
      SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
      SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
      SDIO_SendCommand(&SDIO_CmdInitStructure);

      errorstatus = CmdResp1Error(SD_CMD_APP_CMD);
			
      if (errorstatus != SD_OK)
      {
        return(errorstatus);
      }
			
			/* ACMD41
			 * 命令参数由支持的电压范围及HCS位组成，HCS位置一来区分卡是SDSC还是SDHC
			 * 0:SDSC
			 * 1:SDHC
       * 响应：R3,对应的是OCR寄存器			
			 */			
      SDIO_CmdInitStructure.SDIO_Argument = SD_VOLTAGE_WINDOW_SD | SDType;	  
      SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SD_APP_OP_COND;
      SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;  
      SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
      SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
      SDIO_SendCommand(&SDIO_CmdInitStructure);

      errorstatus = CmdResp3Error();
			
      if (errorstatus != SD_OK)
      {
        return(errorstatus); 
      }
			
			/* 若卡需求电压在SDIO的供电电压范围内，会自动上电并标志pwr_up位 
			 * 读取卡寄存器，卡状态
			 */
      response = SDIO_GetResponse(SDIO_RESP1);
			
			/* 读取卡的ocr寄存器的pwr_up位，看是否已工作在正常电压 */
      validvoltage = (((response >> 31) == 1) ? 1 : 0);	
      count++;			  /* 计算循环次数 */
    }
		
    if (count >= SD_MAX_VOLT_TRIAL)					 /* 循环检测超过一定次数还没上电 */
    {
      errorstatus = SD_INVALID_VOLTRANGE;	   /* SDIO不支持card的供电电压 */
      return(errorstatus);
    }
		
		/*检查卡返回信息中的HCS位*/
		/* 判断ocr中的ccs位 ，如果是sdsc卡则不执行下面的语句 */
    if (response &= SD_HIGH_CAPACITY)       
    {
      CardType = SDIO_HIGH_CAPACITY_SD_CARD; /* 把卡类型从初始化的sdsc型改为sdhc型 */
    }

  }/* else MMC Card */

  return(errorstatus);		
}

```

### SD_InitializeCards()

```c
SD_Error SD_InitializeCards(void)
{
  SD_Error errorstatus = SD_OK;
  uint16_t rca = 0x01;

  if (SDIO_GetPowerState() == SDIO_PowerState_OFF)
  {
    errorstatus = SD_REQUEST_NOT_APPLICABLE;
    return(errorstatus);
  }
	
	/* 判断卡的类型 不是wifi*/
  if (SDIO_SECURE_DIGITAL_IO_CARD != CardType)
  {
    /* Send CMD2 ALL_SEND_CID 
		 * 响应：R2，对应CID寄存器
		 */
    SDIO_CmdInitStructure.SDIO_Argument = 0x0;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_ALL_SEND_CID;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Long;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp2Error();

    if (SD_OK != errorstatus)
    {
      return(errorstatus);
    }
		
		/* 将返回的CID信息存储起来 */
    CID_Tab[0] = SDIO_GetResponse(SDIO_RESP1);
    CID_Tab[1] = SDIO_GetResponse(SDIO_RESP2);
    CID_Tab[2] = SDIO_GetResponse(SDIO_RESP3);
    CID_Tab[3] = SDIO_GetResponse(SDIO_RESP4);
  }
/********************************************************************************************************/
  if (   (SDIO_STD_CAPACITY_SD_CARD_V1_1 == CardType) 
		   ||(SDIO_STD_CAPACITY_SD_CARD_V2_0 == CardType) 
	     ||(SDIO_SECURE_DIGITAL_IO_COMBO_CARD == CardType)
       ||(SDIO_HIGH_CAPACITY_SD_CARD == CardType) )	 /* 使用的是2.0的卡 */
  {
    /* Send CMD3 SET_REL_ADDR with argument 0 
     * SD Card publishes its RCA.
     * 响应：R6，对应RCA寄存器		
		 */
    SDIO_CmdInitStructure.SDIO_Argument = 0x00;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_REL_ADDR;		
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;		
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);
		
		/* 把接收到的卡相对地址存起来 */
    errorstatus = CmdResp6Error(SD_CMD_SET_REL_ADDR, &rca);	

    if (SD_OK != errorstatus)
    {
      return(errorstatus);
    }
  }
/********************************************************************************************************/
  if (SDIO_SECURE_DIGITAL_IO_CARD != CardType)
  {
    RCA = rca;

    /* Send CMD9 SEND_CSD with argument as card's RCA 
		 * 响应:R2  对应寄存器CSD(Card-Specific Data)
		 */
    SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)(rca << 16);
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_CSD;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Long;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp2Error();

    if (SD_OK != errorstatus)
    {
      return(errorstatus);
    }

    CSD_Tab[0] = SDIO_GetResponse(SDIO_RESP1);
    CSD_Tab[1] = SDIO_GetResponse(SDIO_RESP2);
    CSD_Tab[2] = SDIO_GetResponse(SDIO_RESP3);
    CSD_Tab[3] = SDIO_GetResponse(SDIO_RESP4);
  }
/********************************************************************************************************/	
	/*全部卡初始化成功 */
  errorstatus = SD_OK; 

  return(errorstatus);
}
```

# 移植文件系统

和移植SPI_FLASH的文件系统一样，编写diskio.c文件

```c
/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
//#include "usbdisk.h"	/* Example: Header file of existing USB MSD control module */
//#include "atadrive.h"	/* Example: Header file of existing ATA harddisk control module */
//#include "sdcard.h"		/* Example: Header file of existing MMC/SDC contorl module */

/* Definitions of physical drive number for each drive */
//#define ATA		0	/* Example: Map ATA harddisk to physical drive 0 */
//#define MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
//#define USB		2	/* Example: Map USB MSD to physical drive 2 */

#include "./spi/bsp_spi_flash.h"
#include "./sdio/bsp_sdio_sd.h"
#include "string.h"
#define SD_CARD		0	/* Example: Map ATA harddisk to physical drive 0 */
#define SPI_FLASH		1	/* Example: Map MMC/SD card to physical drive 1 */
/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/
extern SD_CardInfo SDCardInfo;

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = STA_NOINIT;

	switch (pdrv) {
	case SD_CARD :
			stat &= ~STA_NOINIT;//直接返回初始化完成
			break;
	
	
	case SPI_FLASH :
		if(SPI_FLASH_ReadID() == sFLASH_ID)
		{
			stat = ~STA_NOINIT;
		}
		else
		{
			stat = STA_NOINIT;
		}
		break;
		
	default:
			stat = STA_NOINIT;

	}
	return stat;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = STA_NOINIT;
//	int result;

	switch (pdrv) {
	case SD_CARD :
				if(SD_Init() != SD_OK)
					stat = STA_NOINIT;
				else
					stat &= ~STA_NOINIT;
		break;



	case SPI_FLASH :
			SPI_FLASH_Init();//初始化
			SPI_Flash_WAKEUP();//唤醒
			stat = disk_status(pdrv);
			break;
	default:
			stat = STA_NOINIT;
	}
	return stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT status = RES_PARERR;
	SD_Error SD_state;
//	int result;

	switch (pdrv) {
	case SD_CARD :
		// translate the arguments here
//		result = ATA_disk_read(buff, sector, count);
			//四字节对齐，那就是最低两位都不能为1，因为4是100，例如5是0101，那就不是4字节对齐
			//所以只要低2位不为0，那就没有对齐，也就是&3不为0的话就表示没对齐
		  if((DWORD)buff&3)
			{
				DRESULT res = RES_OK;
				DWORD scratch[SDCardInfo.CardBlockSize / 4];

				while (count--) 
				{
					res = disk_read(SD_CARD,(void *)scratch, sector++, 1);

					if (res != RES_OK) 
					{
						break;
					}
					memcpy(buff, scratch, SDCardInfo.CardBlockSize);
					buff += SDCardInfo.CardBlockSize;
		    }
		    return res;
			}
				SD_state = SD_ReadMultiBlocks(buff,sector*SDCardInfo.CardBlockSize,SDCardInfo.CardBlockSize,count);
				if(SD_state==SD_OK)
				{
					/* Check if the Transfer is finished */
					SD_state=SD_WaitReadOperation();
					while(SD_GetStatus() != SD_TRANSFER_OK);
				}
				if(SD_state!=SD_OK)
					status = RES_PARERR;
				else
					status = RES_OK;	
				break;   

	case SPI_FLASH :
				//一个扇区是4096个字节，我们实现的spi读flash函数是一个字节一个字节的读
				//所以第n个扇区的地址就是sector*4096，要读取的字节数就是要读取的扇区数×4096个字节
				sector+=512;  	
				SPI_FLASH_BufferRead(buff,sector*4096,count*4096);

				status = RES_OK;

	
				
	default:
				status = RES_PARERR;
	}

	return status;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT status = RES_PARERR;
	SD_Error SD_state = SD_OK;

	if(!count)
		return RES_PARERR;
	
	switch (pdrv) {
	case SD_CARD :
			if((DWORD)buff&3)
			{
				DRESULT res = RES_OK;
				DWORD scratch[SDCardInfo.CardBlockSize / 4];

				while (count--) 
				{
					memcpy( scratch,buff,SDCardInfo.CardBlockSize);
					res = disk_write(SD_CARD,(void *)scratch, sector++, 1);
					if (res != RES_OK) 
					{
						break;
					}					
					buff += SDCardInfo.CardBlockSize;
		    }
		    return res;
			}		
		
			SD_state=SD_WriteMultiBlocks((uint8_t *)buff,sector*SDCardInfo.CardBlockSize,SDCardInfo.CardBlockSize,count);
			if(SD_state==SD_OK)
			{
				/* Check if the Transfer is finished */
				SD_state=SD_WaitWriteOperation();

				/* Wait until end of DMA transfer */
				while(SD_GetStatus() != SD_TRANSFER_OK);			
			}
			if(SD_state!=SD_OK)
				status = RES_PARERR;
		  else
			  status = RES_OK;	
		break;

	case SPI_FLASH :
		// translate the arguments here
		//一个扇区是4096个字节，我们实现的spi读flash函数是一个字节一个字节的读
	//所以第n个扇区的地址就是sector*4096，要写入的字节数就是要读取的扇区数×4096个字节
	//写入之前要先擦除
	//偏移2M个字节
	sector+=512;
	SPI_FLASH_SectorErase(sector*4096);	
	SPI_FLASH_BufferWrite((u8 *)buff,sector*4096,count*4096);

	status = RES_OK;

		default:
			status = RES_PARERR;
	}
	return status;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT status = RES_PARERR;

	switch (pdrv) {
	case SD_CARD :
			switch (cmd) 
			{
				// Get R/W sector size (WORD) 
				case GET_SECTOR_SIZE :    
					*(WORD * )buff = SDCardInfo.CardBlockSize;
				break;
				// Get erase block size in unit of sector (DWORD)
				case GET_BLOCK_SIZE :      
					*(DWORD * )buff = 1;//SDCardInfo.CardBlockSize;
				break;

				case GET_SECTOR_COUNT:
					*(DWORD * )buff = SDCardInfo.CardCapacity/SDCardInfo.CardBlockSize;
					break;
				case CTRL_SYNC :
				break;
			}
			status = RES_OK;
			break;
    
	case SPI_FLASH :
				switch(cmd)
				{
					case CTRL_SYNC:
							
						break;
					//返回扇区大小
					case GET_SECTOR_COUNT:
						//flash为8M字节，一个扇区4kb字节，所以一共8*1024/4 = 2048个
						//又因为板子的flash前2M个字节的空间被用作他用，所以文件系统只能利用后6M的空间
						//2*1024*1024/4096 = 512，2048-512 = 1536
						*(DWORD*)buff = 1536;
						break;
					
					//每个扇区的大小
					case GET_SECTOR_SIZE:
						*(WORD*)buff = 4096;
						break;
					
					//返回最小擦除块的大小(flash最小擦除单位是扇区)
					case GET_BLOCK_SIZE:
						*(DWORD*)buff = 1;
					
				}
			status = RES_OK;
			default:
			status = RES_PARERR;


	}

	return status;
}

__weak DWORD get_fattime(void) {
	/* 返回当前时间戳 */
	return	  ((DWORD)(2025 - 1980) << 25)	/* Year 2025 */
			| ((DWORD)1 << 21)				/* Month 1 */
			| ((DWORD)1 << 16)				/* Mday 1 */
			| ((DWORD)0 << 11)				/* Hour 0 */
			| ((DWORD)0 << 5)				  /* Min 0 */
			| ((DWORD)0 >> 1);				/* Sec 0 */
}
#endif

```

