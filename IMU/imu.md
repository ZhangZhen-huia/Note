# 所涉及的名词

## 轴角/旋转向量

**百度解释**：轴角，刚体动力学术语，是晶体中结晶轴之间的夹角，一般以α、β、γ表示

英文名：axial angle；optic(-axial) angle

旋转的轴角表示用两个值参数化了旋转: **一个轴或直线**，和描述绕这个轴的旋转量的**一个角**。它也叫做旋转的指数坐标。有时也叫做**旋转向量**表示。

假如你站在地面上，选取重力的方向为负 *z* 方向。如果你左转，你将绕 *z* 轴旋转 90度，在轴角表示中，这将是

![image-20241020233815961](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202410202338726.png)

**r=[x,y,z,θ]**

单位向量n=[x,y,z]对应的是旋转轴，![\theta](https://i-blog.csdnimg.cn/blog_migrate/798a135f47a1ec3dc636082b66dc9938.gif)对应的是旋转角度。旋转向量与轴角相同，只是旋转向量用三个元素来描述旋转，它把![\theta](https://i-blog.csdnimg.cn/blog_migrate/798a135f47a1ec3dc636082b66dc9938.gif)角乘到了旋转轴上，如下：

​                                 ![](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202410202317533.png)

轴角最大的一个局限就是不能进行简单的插值，此外，轴角形式的旋转不能直接施于点或矢量

## 欧拉角

**百度解释**：用来 **唯一的** **确定定点转动**刚体位置的 **三个一组**的 **独立角参量**，由章动角θ、旋进角（即进动角）ψ和自转角φ组成，为欧拉首先提出而得名。简单的说是使用XYZ三个轴的旋转分量,来描述一个6自由度的旋转

![img](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202410202317046.gif)

​														（图1  该动图摘抄自维基百科）

通俗来说，就是你要去某个地方，别人给你指路：先前走100米然后右转然后再左转

听起来很清晰，更好理解，欧拉角就是这一种相对坐标



欧拉角一般具有两大类表示方式：

Proper Euler angles (z-x-z, x-y-x, y-z-y, z-y-z, x-z-x, y-x-y)

Tait–Bryan angles (x-y-z, y-z-x, z-x-y, x-z-y, z-y-x, y-x-z)

每个大类都使用了3个变量描述三次旋转过程中的旋转角度, 差别在于Proper Euler angles只涉及两个转轴.而Tait–Bryan angles涉及三个转轴

Tait–Bryan angles 也被称为Cardan angles, nautical angles, (heading, elevation, and bank),(yaw, pitch, and roll). 我们接触的比较多的是yaw(偏航), pitch(俯仰), roll(横滚).三个变量一般对应(车体,飞行器的)z,y,x三个坐标轴.



## 旋转矩阵

**向量旋转：**

先看二维旋转

![image-20241021165857502](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202410211659575.png)

根据三角函数关系，可以列出向量OP与OP'的坐标表示形式：

![image-20241021170022795](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202410211700821.png)

用矩阵表达为：

![image-20241021173222633](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202410211732660.png)



三维旋转可以类似的求出（其中y轴转错方向了，转过去就好）

<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202410211733896.jpg" alt="70793a198d0149d933ffa6625a37634d" style="zoom:50%;" />



![image-20241021173208140](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202410211732178.png)

​													（图2）

**坐标系旋转**：

坐标系旋转就是向量旋转的逆运动，故得到的矩阵就是向量旋转矩阵的**逆矩阵**

![ff9c9bcffd3b51d99bd161c3b69c94d0](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202410211742355.jpg)



然后为了表示一个物体绕三个轴的旋转，可以把三个轴的旋转矩阵相乘

![image-20241021174909764](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202410211749800.png)

​																（图3）

## 四元数

百度解释：

四元数，是简单的超复数。 [复数](https://baike.baidu.com/item/复数/254365?fromModule=lemma_inlink)是由[实数](https://baike.baidu.com/item/实数/296419?fromModule=lemma_inlink)加上[虚数单位](https://baike.baidu.com/item/虚数单位/6803458?fromModule=lemma_inlink) i 组成**，其中i²= -1**。 相似地，**四元数都是由实数加上三个虚数单位 i、j和k 组成**，而且它们有如下的关系： **i² = j² = k² = -1**， iº = jº = kº = 1 , 每个四元数都是 **1、i、j 和 k** 的线性组合，即是四元数一般可表示为**a + bi+ cj + dk**，其中**a、b、c 、d是实数**。

对于i、j和k本身的[几何](https://baike.baidu.com/item/几何/303227?fromModule=lemma_inlink)意义可以理解为一种旋转，其中i旋转代表Z轴与Y轴相交平面中Z轴正向向Y轴正向的旋转，j旋转代表X轴与Z轴相交平面中X轴正向向Z轴正向的旋转，k旋转代表Y轴与X轴相交平面中Y轴正向向X轴正向的旋转，-i、-j、-k分别代表i、j、k旋转的反向旋转。

参考文献：

1.CSDN：欧拉角（易理解）https://blog.csdn.net/ODDYOU/article/details/119976130  --->欧拉角部分内容和图片来源

2.知乎：[陀螺仪姿态解算+mahony滤波算法（公式推导及其代码） - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/654496867)-->旋转矩阵图片内容来源