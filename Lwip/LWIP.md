# LWIP简介

- lwip是一个小型的开源的TCP/IP协议栈——阉割版的TCP/IP协议栈
- 作用: 云台接入, 无线网关, 远程模块, 工控控制器, 嵌入式NAT无线路由器, 网络摄像头
  - LWIP本身没有实现NAT协议，但是他是路由器必备的协议，我们可以添加

![image-20250417205230766](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504172052837.png)

![image-20250417205456512](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504172054570.png)

![image-20250417205938120](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504172100687.png)

# MAC简介

- STM32自带有10/100Mbit/s的以太网MAC内核，它提供地址及媒体访问的控制方式
- 支持外部PHY接口实现***10/100Mbit/s***数据传输速率
- ***支持全双工和半双工操作***
- 支持通过MDIO接口配置和管理PHY设备
- 数据链路层提供寻址机构、数据帧的构建、数据差错检查、传送控制、向网络层提供标准的数据接口等功能。数据链路层的芯片称之为MAC控制器

> 从硬件的角度来分析，以太网的电路接口一般由CPU、MAC(Media Access Control)控制器和物理层接口(physical Layer PHY)组成
>
> ![image-20250417210905369](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504172109420.png)
>
> 对于上述三部分，并不一定都是独立的芯片，主要有以下几种情况
>
> - CPU内部集成了MAC和PHY，难度较高
> - CPU内部集成MAC,PHY采用独立芯片(主流方案)
> - CPU不集成MAC和PHY，MAC和PHY采用独立芯片或者集成芯片(高端采用)
> - MDIO总线接口，主要是完成CPU对于PHY芯片的寄存器配置
> - MII总线接口，主要是完成数据收发相关的业务
> - MII叫做介质独立接口，以太网MAC通过该接口发出数据帧经过PHY后传输到其他网络节点上，同时其他网络节点的数据先经过PHY后再由MAC接收；
> - SMI叫做是串行管理接口，以太网MAC通过该接口可以访问PHY的寄存器，通过对这些寄存器操作可对PHY进行控制和管理

# ST的ETH框架

![image-20250417211754978](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504172117050.png)

# SMI站管理接口

允许应用程序通过时钟线和数据线访问任意PHY寄存器，***最多支持32个PHY访问***

- MDC（可以类比于IIC的SCL）：周期时钟引脚，最大频率为2.5Mhz，由stm32内部实现
- MDIO（可以类比于IIC的SDA）：数据输入/输出比特流

![image-20250417212152112](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504172121143.png)

## 帧格式

![image-20250417212629110](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504172126163.png)

![image-20250417213004917](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504172130971.png)

![image-20250417212849525](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504172128578.png)

![image-20250417212836769](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504172128811.png)

# MII和RMII

- 用于MAC与外接的PHY互联，支持10/100Mbit/s数据传输模式
- 主要作用就是把以太网帧转发到PHY设备当中，即MAC内核于PHY之间的桥梁

![image-20250417213451276](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504172134322.png)

## 二者比较

![image-20250417213812179](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504172138231.png)

### 引脚

![image-20250417215004223](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504172150286.png)

### 时钟

![image-20250417215253966](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504172152025.png)

### 收发位宽

![image-20250417221820870](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504172218926.png)

# PHY芯片简介

![image-20250418181937862](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504181819947.png)

从硬件上来说，一般的PHY芯片为模数混合电路，负责接收电、光这类模拟信号，经过解调和A/D转换后通过MII/RMII接口将信号交给MAC内核处理

## LAN8720A与YT8512C

它们都是低功耗的10/100M以太网PHY层芯片，支持MII以及RMII接口与以太网MAC层通信。支持Auto MDIX自动翻转功能，无需更换网线即可将连接更改为直连或交叉连接。

![image-20250418182212221](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504181822277.png)

## RMII模式选择

LAN8720A和YT8512C在RMII介质接口下延伸出两个连接方式

![image-20250418183250959](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504181832014.png)

### LAN8720A

***这两种模式是由PHY的2号引脚电平决定，为0：Out模式（REF_CLK输出时钟），为1：In模式（中断）***

![image-20250418183638673](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504181836707.png)

![image-20250418183428886](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504181834942.png)

![image-20250418183446336](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504181834405.png)

### YT8512C

![image-20250418183804013](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504181838081.png)

## PHY寄存器

![image-20250418184348054](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504181843113.png)

- BCR：设置PHY的双工模式和网速
- BSR：获取PHY的双工模式和网速

## 特殊功能寄存器

![image-20250418184856738](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504181848783.png)

![image-20250418185022014](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504181850086.png)

![image-20250418185031706](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504181850776.png)