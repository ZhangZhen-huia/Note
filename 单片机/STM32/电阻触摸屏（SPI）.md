# 触摸屏

## 电阻触摸屏

- 便宜，能适应恶劣环境，但是只支持单点触控，触摸时需要压力，使用久了容易磨损
- 野火的3.2寸屏幕是xpt2046芯片，SPI通信
- XPT2046，TSC2046，HR2046

![image-20250414213417666](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504142134726.png)

## 电容触摸屏

- 支持多点触控，检测精度高，通过与导电物体产生的电容效应来检测触摸动作
- 只能感应导电物体的触摸，湿度较大或屏幕表面有水时会影响检测效果
- 手机的屏幕都是电容屏
- GT9147，GT917S，GT911，GT1151，FT5426芯片

![image-20250414213456697](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504142134735.png)

# XPT2046芯片

![image-20250414214952951](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504142149008.png)

![image-20250414213841004](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504142138065.png)

- 左边连接到电阻屏，右边接stm32
- AUX：ADC辅助输入通道，可以不接
- PENIRQ：笔接触中断引脚，低电平表示有触摸

## 控制字

![image-20250414215238138](C:\Users\a1874\AppData\Roaming\Typora\typora-user-images\image-20250414215238138.png)

- Bit7：为1表示开始检测 
- Bit6-4：![image-20250414215614912](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504142156950.png)
- Bit3：选择12位精度ADC（比较慢，精度高）还是选择8位ADC精度（速度快，精度稍微低）
- Bit1-0：节电模式选择，为00的时候会在获取数据的时候才去产生电场![image-20250414215516984](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504142155029.png)
- 发送0x90检测X通道，发送0xD0检测Y通道

## 软件SPI（可以使用硬件的，这里是野火用了软件模拟的）

![image-20250414222509023](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504142225060.png)

### 发送命令

![image-20250414222847110](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504142228175.png)

```c
//发送命令
void XPT2046_SendCMD(uint8_t cmd)	
{
	uint8_t i;
	
	/*设置引脚的初始状态*/
	XPT2046_CS_ENABLE();//片选引脚设置为低电平
	XPT2046_CLK_LOW();//时钟为低电平
	XPT2046_MOSI_0();
	
	/*片选选中*/
	XPT2046_DelayUS(10);
	XPT2046_CS_DISABLE();
	
	/*产生8个时钟，发送数据*/
	for(i=0;i<8;i++)
	{
		if( (cmd & (0x80>>i)) == 0 )
		{
			XPT2046_MOSI_0();
		}
		else
		{
			XPT2046_MOSI_1();		
		}		
		
		XPT2046_CLK_HIGH();		
		XPT2046_DelayUS(5);
			
		XPT2046_CLK_LOW();
		XPT2046_DelayUS(5);	
	}	
	
//	/*片选取消*/
//	XPT2046_CS_ENABLE();
	
}
```

### 读取命令

```c
//读取数据
uint16_t XPT2046_ReceiveData(void)	
{
	uint8_t i;
	
	uint16_t receive_temp=0;
	
	
		XPT2046_CLK_HIGH();		
		XPT2046_DelayUS(5);
		XPT2046_CLK_LOW();
		XPT2046_DelayUS(5);	
	
	/*产生12个时钟，读取数据*/
	for(i=0;i < 12;i++)
	{		
		receive_temp = receive_temp << 1;

		XPT2046_CLK_HIGH();		
		XPT2046_DelayUS(5);
		
		
		if( XPT2046_MISO() == Bit_SET)
		{
			receive_temp |= 0x01;
		}
		else
		{
			receive_temp |= 0x00;		
		}				
			
		XPT2046_CLK_LOW();
		XPT2046_DelayUS(5);	

	}	
	
	/*片选取消*/
	XPT2046_CS_ENABLE();
	
	return receive_temp;
	
}
```

