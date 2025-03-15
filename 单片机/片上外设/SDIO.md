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

- 存储单元：存储数据的部件，存储单元通过存储单元接口与卡控制单元进行数据传输
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