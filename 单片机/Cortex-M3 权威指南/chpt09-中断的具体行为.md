# 中断／异常的响应序列 

当一个开始响应一个中断时，CM3体内将会进行一下三件事

- 入栈：把8个寄存器的值压入栈中
- 取向量：从向量表中找出对应的服务程序入口地址
- 选择堆栈指针MSP/PSP，更新堆栈指针SP，更新连接寄存器LR，更新程序计数器PC

## 入栈

保护现场：依次把xPSR, PC, LR, R12以及 R3‐R0由硬件自动压入适当的堆栈中，如果当响应异常时，当前的代码正在使用PSP，则压入 PSP，即使用线程堆栈；否则压入MSP，使用主堆栈。一旦进入了服务例程，就将一直使用 主堆栈

> 假设入栈开始时，SP的值为N，则则在入栈后，入栈顺序以及入栈后堆栈中的内容
>
> ![image-20250403152227633](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504031522672.png)
>
> ![image-20250403152345025](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504031523059.png)

R4-R11在中断服务例程很庞大的时候也会被压入栈中

## 取向量 

当数据总线（系统总线）正在为入栈而忙碌的时候，指令总线（I-Code总线）开始从向量表中找出正确的异常向量，然后在服务程序的入口处预取指，使得入栈与取指这两个操作可以同时进行

## 更新寄存器

在入栈和取向量的工作都完毕之后，执行服务例程之前，还要更新一系列的寄存器

- SP堆栈指针：在入栈中会把堆栈指针（PSP或MSP）更新到新的位置
- PSR：IPSR位段（地处PSR的最低部分）会被更新为新响应的异常编号
- PC：在向量取出完毕后，PC将指向服务例程的入口地址
- LR：自动更新为EXC_RETURN

# 异常返回

从形式上看，有3种途径可以触发异常 返回序列，不管使用哪一种，都需要用到先前储的LR的值

![image-20250405145855860](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504051459949.png)

在启动了中断返回序列后，就会进行出栈和更新NVIC寄存器的操作

- 出栈：先前压入栈中的寄存器在这里恢复。内部的出栈顺序与入栈时的相对应，堆栈指 针的值也改回去。
- 更新NVIC寄存器：伴随着异常的返回，它的活动位也被硬件清除。对于外部中断，倘若中断输入再次被置为有效，悬起位也将再次置位，新一次的中断响应序列也可随之再次开始

# 咬尾中断

就是第二个中断咬到第一个中断的尾巴上面，紧紧的跟着上一个中断，两个中断一共只需要入栈，出栈一次就够了

![image-20250405150500618](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504051505664.png)

# 晚到（的高优先级）异常

低优先级的异常刚刚开始响应，处于入栈的阶段，尚未执行其服务例程时，如果此时收到了高优先级异常的请求，则本次入栈就成了为高优先级中断所做的了，入栈后就会直接执行高优先级异常的服务程序

![image-20250405150729913](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504051507955.png)

# 异常返回值 

> ![image-20250405150839275](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504051508315.png)

> 合法的EXC_RETURN值共3个
>
> ![image-20250405150903768](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504051509802.png)
