# 所涉及的名词

## 轴角/旋转向量

**百度解释**：轴角，刚体动力学术语，是晶体中结晶轴之间的夹角，一般以α、β、γ表示

英文名：axial angle；optic(-axial) angle

旋转的轴角表示用两个值参数化了旋转: **一个轴或直线**，和描述绕这个轴的旋转量的**一个角**。它也叫做旋转的指数坐标。有时也叫做**旋转向量**表示。

假如你站在地面上，选取重力的方向为负 *z* 方向。如果你左转，你将绕 *z* 轴旋转 90度，在轴角表示中，这将是

![img](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202410202317723.svg+xml)

**r=[x,y,z,θ]**

单位向量n=[x,y,z]对应的是旋转轴，![\theta](https://i-blog.csdnimg.cn/blog_migrate/798a135f47a1ec3dc636082b66dc9938.gif)对应的是旋转角度。旋转向量与轴角相同，只是旋转向量用三个元素来描述旋转，它把![\theta](https://i-blog.csdnimg.cn/blog_migrate/798a135f47a1ec3dc636082b66dc9938.gif)角乘到了旋转轴上，如下：

​                                 ![](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202410202317533.png)

轴角最大的一个局限就是不能进行简单的插值，此外，轴角形式的旋转不能直接施于点或矢量

## 欧拉角

**百度解释**：用来 **唯一的** **确定定点转动**刚体位置的 **三个一组**的 **独立角参量**，由章动角θ、旋进角（即进动角）ψ和自转角φ组成，为欧拉首先提出而得名。

![img](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202410202317046.gif)

（图1  该动图摘抄自维基百科）

通俗来说，就是你要去某个地方，别人给你指路：先前走100米然后右转然后再左转

听起来很清晰，更好理解，欧拉角就是这一种相对坐标

参考文献：

1.CSDN：欧拉角（易理解）https://blog.csdn.net/ODDYOU/article/details/119976130  --->欧拉角部分内容和图片来源