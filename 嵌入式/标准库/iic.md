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

