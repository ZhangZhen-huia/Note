# SPI-多主机通信(轮流做主机而非同时多个主机)

## SPI介绍

![image-20240818123728081](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408181237133.png)

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408181237798.png" alt="image-20240818123753737" style="zoom:60%;" />

## SPI框图

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408181241206.png" alt="image-20240818124109135" style="zoom:50%;" />

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

### 起始条件和终止条件

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408181327053.png" alt="image-20240818132736002" style="zoom:50%;" />

### 交换一个字节--模式0

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408181328300.png" alt="image-20240818132828246" style="zoom:50%;" />

### 交换一个字节--模式1

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202408181328220.png" alt="image-20240818132806157" style="zoom:50%;" />
