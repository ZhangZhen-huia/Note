# ARM汇编

## 基本语法

> ![image-20250309150841665](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503091508690.png)

- 标号：可选，如有必须顶格写，作用是让汇编器来计算程序转移的地址

- 操作码：前面必须有至少一个空白符，通常使用一个“Tab”

- 操作数：可以有若干个，第一个操作数是要操作的地址寄存器，立即数必须由#开头

- 注释：以分号” ；“ 开头

- EQU：来定义常数，常数定义必须顶格写

- > ![](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503091516504.png)

- 手工汇编：如果汇编器不能识别某些特殊指令的助记符，你就要“手工汇编”——查出该指令的确 切二进制机器码，然后使用 DCI 编译器指示字。例如，BKPT 指令的机器码是 0xBE00

- ![image-20250309152016873](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503091520898.png)

- DCB：定义一串字节常数，—允许以字符串的形式表达

- DCD ：定义一串 32 位整数

- > ![image-20250309152111871](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503091521904.png)

## 后缀使用

> ![image-20250309152147954](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503091521985.png)

> 在 Thumb‐2 指令集中，有些操作既可以由 16 位指令完成，也可以由 32 位指令完成。 例如，R0=R0+1 这样的操作，16 位的与 32 位的指令都提供了助记符为“ADD”的指令。在 UAL 下，你可以让汇编器决定用哪个，也可以手工指定是用 16 位的还是 32 位的
>
> ![image-20250309152354031](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503091523058.png)

- .W(Wide)后缀指定 32 位指令，如果没有给出后缀，汇编器会先试着用 16 位指令以缩小 代码体积，如果不行再使用 32 位指令

## 指令集

边框加粗的是从 ARMv6T2 才支持的指令。 

双线边框的是从 Cortex‐M3 才支持的指令（v7 的其它款式不一定支持）

![image-20250309154818093](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503091549359.png)

### 16位指令

#### 16 位数据操作指令 

![image-20250309153847704](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503091539989.png)

#### 16 位转移指令

![image-20250309154010009](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503091540040.png)

#### 16 位存储器数据传送指令 

![image-20250309154030827](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503091540864.png)

#### 其它 16 位指令

![image-20250309154045755](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503091540787.png)

### 32位指令

#### 32 位数据操作指令

![image-20250309154118993](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503091541040.png)

![image-20250309154203138](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503091542191.png)

![image-20250309154211321](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503091542361.png)

#### 32 位存储器数据传送指令

![image-20250309154219202](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503091542240.png)

#### 32 位转移指令

![image-20250309154230729](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503091542759.png)

#### 其它 32 位指令 

![image-20250309154238901](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503091542944.png)

![image-20250309154247313](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503091542338.png)

# ARM 汇编代码中通用的语法

## 数据传送 

- 两个寄存器间传送数据 
- 寄存器与存储器间传送数据 
- 寄存器与特殊功能寄存器间传送数据 
- 把一个立即数加载到寄存器

```c++
MOV R8, R3
//把 R3 的数据传送给 R8
```

## 常用的存储器访问指令

![image-20250309155401883](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503091554928.png)

![image-20250309155409428](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503091554466.png)

## 常用的多重存储器访问方式

![image-20250309155428941](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503091554991.png)

> 加粗的是符合 CM3 堆栈操作的 LDM/STM 使用方式。并且，如果 Rd 是 R13（即 SP），则与 POP/PUSH 指令等效。(LDMIA‐>POP, STMDB ‐> PUSH)
>
> ![image-20250310120736678](C:\Users\a1874\AppData\Roaming\Typora\typora-user-images\image-20250310120736678.png)

！表示自增或者自减

- I：Increment，自增字（4字节）

- D：Decrement，自减字（4字节） 

- B：Before，之前

- A：After，之后

- > ![image-20250310121302935](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503101213971.png)

关于第二个操作，AI解释如下：

![image-20250310121643837](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503101216883.png)

## 预索引

！：还可以用于更新寄存器的标志, `LDR.W   R0,     [R1,   #20]!`, 先执行R1 = R1 + 20, 之后把R1位置的值放到R0(预索引)

![image-20250310125002310](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503101250615.png)

## 后索引

先把寄存器地址里面的值取出来以后再进行更新寄存器

```c
STR.W R0, [R1], #-12 ;后索引
```

该指令是把 R0 的值存储到地址 R1 处的。在存储完毕后， R1 = R1+(‐12)

![image-20250310125317192](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202503101253249.png)