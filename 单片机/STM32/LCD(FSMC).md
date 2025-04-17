# 显示器简介

显示器属于计算机的IO设备

## LCD

液晶显示器，功耗比较小，缺点是需要背光片，在强光环境下显示效果很差

## LED显示器

单个像素点内包含红绿蓝三色 LED 灯，显示原理类似我们实验板上的 LED 彩灯，通过控制红绿蓝颜色的强度进行混色，实现全彩颜色输出，多个像素点构成一个屏幕，因为是自发光，所以在强光下也能清晰显示

## OLED 显示器

不需要背光源

# 显存

——专门用于存储显示数据的存储器，则被称为显存

一般会使用 SRAM 或 SDRAM 性质的存储器

显存一般至少要能存储液晶屏的一帧显示数据，如RGB888的分辨率为800×480的屏幕，一帧的数据大小为3×800×480 = 1152000字节

# 基本参数

- 像素：显示器的像素指它成像最小的点
- 分辨率：分辨率 800x480表示该显示器的每一行有 800 个像素点，每一列有 480 个像素点，也可理 解为有 800 列，480 行
- 色彩深度：指显示器的每个像素点能表示多少种颜色，一般用bit来表示，如16bit的RGB565，24bit的RGB888
- 显示器尺寸：英寸，这个长度是指屏幕对 角线的长度
- 点距：两个相邻像素点之间的距离，分辨率于点距成反比



# 野火的屏幕

stm32f1x系列因为性能不足，所以需要屏幕自带控制芯片，所以它只能驱动自带控制器的屏幕，可以理解为电脑的外置显卡

![image-20250320101357984](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503201014104.png)

## 液晶面板的控制信号（控制芯片与液晶屏的通信）

![image-20250320101502446](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503201015474.png)

- RGB信号线：8位，所以支持RGB888，一共 24 位数据线，可表示的颜色为 2 24种
- CLK：液晶屏与外部使用同步通讯方式，以 CLK 信号作为同步时钟，在同步时钟的驱动下， 每个时钟传输一个像素点数据
- HSYNC：每传输完成液晶屏的**一行**像素数据时，HSYNC 会发生电平跳变，如分辨率是800×480的屏幕，传输完一帧需要跳变480次
- VSYNC：，每传 输完成一帧像素数据时，VSYNC 会发生电平跳变，如液晶屏以 60 帧/秒的 速率运行时，VSYNC 每秒钟电平会跳变 60 次
- DE：当DE为高时，RBG信号线才有效

## 液晶数据传输时序（了解）

传输一帧图像数据的时序，中间省略了多行及多个像素点

![image-20250320102048470](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503201020529.png)

![image-20250320102123765](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503201021803.png)

![image-20250320102140455](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503201021493.png)

> 在这些时间参数控制的区域，数据使能信号线“DE”都为低电平，RGB 数据线的信 号无效，当“DE”为高电平时，表示的数据有效，传输的数据会直接影响液晶屏的显示区域

## ILI9341控制芯片

该芯片的核心就是中间的GRAM（也就是显存），GRAM 中每个存 储单元都对应着液晶面板的一个像素点，所以可以用FSMC来和他通信

左边的引脚与MCU通信

- 如每个像素点的位数是 6、16 还是 18 位
- 可配置使用 SPI 接口、 8080 接口还是 RGB 接口与 MCU 进行通讯（IM[3:0]来选择控制模式），当前屏幕使用8080时序
-  LED 控制器(LED Controller），用来控制液晶屏中的 LED 背光源

右边的是和LCD进行通信

![image-20250320103146439](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503201031485.png)

## 8080接口

![image-20250320104348831](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503201043879.png)

![image-20250320104406659](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503201044705.png)

- 由图可知没有引出CLK引脚，所以使用异步通信
- 读数据：为了还原屏幕，比如获取截屏，鼠标移动之后被原本遮挡图像的还原

### 时序

#### 写

![image-20250320104842983](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503201048027.png)

#### 读

![image-20250320105127791](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503201051837.png)

#### 常用命令

![image-20250320105201093](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503201052121.png)

# 实际操作（FSMC）

**使用模式B**，之前控制SRAM的时候使用的是模式A

8080时序也可以使用普通的IO来模拟时序，缺点就是效率太低

## FSMC

——灵活的静态存储控制器

可以用于驱动包括 SRAM、 NOR FLASH 以及 NAND FLSAH 类型的存储器，不能驱动如 SDRAM 这种动态的存储器（FMC可以驱动动态的）

> 由于 FSMC 外设可以用于控制扩展的外部存储器，而 MCU 对液晶屏的操作实际上就 是把显示数据写入到显存中，与控制存储器非常类似，且 8080 接口的通讯时序完全可以使 用 FSMC 外设产生，因而非常适合使用 FSMC 控制液晶屏

![image-20250320110205839](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503201102890.png)

![image-20250320110305010](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503201103054.png)

没有引出CLK引脚，所以屏幕采用异步通信，就不需要CLK信号

A：地址总线，我们需要一根来和LCD进行通信

在控制 LCD 时，使用的是类似异步、地址与数据线独立的 NOR FLASH 控制方式，所以 CLK、NWAIT、NADV 引脚并没有使用到

![image-20250320110702075](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503201107111.png)

NE片选选择

## 读时序

![image-20250320110757865](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503201107916.png)

## 写时序

![image-20250320110810922](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503201108975.png)

## 时序对比

![image-20250414142935047](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504141429181.png)

![image-20250320120227140](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503201202192.png)

所以我们需要把A[25:0]模拟D/CX

![](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504141435254.png)

所以通过地址线也可以输出高低电平

***注意：还需要注意地址对齐问题，NOR FLASH是16位的，所以根据规则，把HADDR<<1位***

*![image-20250320122630121](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503201226183.png)*



![image-20250320123659990](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503201237037.png)

PE1：复位引脚，配置成普通推挽输出

BL（BK）：背光引脚，普通推挽输出

NE1：片选块

RS：表示D/CX，用于数据/命令选择，A16引脚

## 程序

- 把液晶屏当成一个NORFLASH来控制，即模式B

- 初始化GPIO，出了复位和背光引脚为推挽输出，其他都为FSMC的复用推挽输出

- 初始化FSMC，FSMC_NORSRAMInitTypeDef 和 FSMC_NORSRAMTimingInitTypeDef结构体，和SRAM差不多，需要改bank，存储器类型，模式，还有各种时间，注意要失能Bank

  - 地址建立时间和数据保持时间![image-20250414144704489](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504141447559.png)

  - 具体的事时间长短要看ili9341的芯片手册
    - ![image-20250414144803558](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504141448647.png)

    - ![image-20250414144816763](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504141448843.png)

- 算D/CX，用A16引脚模拟，然后用指针的形式去访问地址，来产生命令

  - ```c
    //FSMC_Bank1_NORSRAM用于LCD命令操作的地址
    #define      FSMC_Addr_ILI9341_CMD         ( ( uint32_t ) 0x60000000 )	//0x60000000 & (~(0<<(16+1)))	加上地址对齐
    
    //FSMC_Bank1_NORSRAM用于LCD数据操作的地址      
    #define      FSMC_Addr_ILI9341_DATA        ( ( uint32_t ) 0x60020000 )	//0x60000000 | (1<<(16+1)) 加上地址对齐
    ```

  ```c
  * ( __IO uint16_t * ) ( FSMC_Addr_ILI9341_CMD ) = usCmd;
  ```

  注意：这里并不是把地址截断了，而是在stm32中，指针就是32位宽的，这里uint16_t*只是声明指针指向的数据类型为 16 位。而截断地址需显式操作 

```c
uint16_t* ptr = (uint16_t*)(uint16_t)0x60000000;
```

## 命令

### 2Ah——设置x坐标![image-20250322111703585](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503221117631.png)

```c
	/* column address control set 设置x轴起始坐标和结束坐标*/
	/* 从0x0000 到 0x00EF 即0-239一共240，正好等于屏幕的一行为240像素点*/
	ILI9341_Write_Cmd ( CMD_SetCoordinateX ); 
	ILI9341_Write_Data ( 0x00 );
	ILI9341_Write_Data ( 0x00 );
	ILI9341_Write_Data ( 0x00 );
	ILI9341_Write_Data ( 0xEF );
```

### 2Bh——设置y坐标

![image-20250322112013812](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503221120860.png)

```c
	/* page address control set */
	/* 0-319 一共320，对应屏幕一列为320像素点 */
	DEBUG_DELAY ();
	ILI9341_Write_Cmd ( CMD_SetCoordinateY ); 
	ILI9341_Write_Data ( 0x00 );
	ILI9341_Write_Data ( 0x00 );
	ILI9341_Write_Data ( 0x01 );
	ILI9341_Write_Data ( 0x3F );
```

3Ah——设置像素

![image-20250322112338766](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503221123816.png)

```c
	/*  Pixel Format Set (3Ah)  */
	DEBUG_DELAY ();
	ILI9341_Write_Cmd ( 0x3a ); 
	ILI9341_Write_Data ( 0x55 );//0101 0101
```

### 36h——扫描方向

# 字符编码

> 由于计算机只能识别 0 和 1，文字也只能以 0 和 1 的形式在计算机里存储，所以我们 需要对文字进行编码才能让计算机处理，编码的过程就是规定特定的 01 数字串来表示特定 的文字，最简单的字符编码例子是 ASCII 码

## ASCII码

### 控制字符——没有图案，即打印出来是空白

![image-20250401123218545](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504011232684.png)

![image-20250401123237677](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504011232741.png)

### 字符与数字——有图案的

![image-20250401123245850](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504011232901.png)

![image-20250401123251654](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504011232698.png)

## 中文编码

### GB2312

把ASCII码的127号之后的扩展字符集直接丢弃，小于 127 的编码按原来 ASCII 标准解释字符。当 2 个大于 127 的字符连在一起 时，就表示 1 个汉字，第 1 个字节使用 (0xA1-0xFE) 编码，第 2 个字节使用(0xA1-0xFE)编 码，这样的编码组合起来可以表示了 7000 多个符号，其中包含 6763 个汉字

# 字模

字模是图形数据，而图形在计算机中是由一个个像素点组成的，所以字模实质是 一个个像素点数据

使用字模软件直接生成
