# MQTT简介

基于客户端服务端架构的**发布订阅**模式的消息传输协议

类似于微博的机制，发布者发布的信息会被所有订阅者收到，没有订阅的用户收不到消息

服务器可以创建了多个主题A，B，C，D，每个主题都不一样，客户端订阅哪个主题，才能接收到这个主题的消息

- 发布/订阅消息模式，提供一对多的消息发布
- MQTT 是基于 TCP 连接进行数据推送的
- 支持 QoS 等级。根据消息的重要性不同设置不同的服务等级
- 小型传输，开销很小，协议交换最小化，以降低网络流量
- 使用 will 遗嘱机制来通知客户端异常断线
- 基于主题发布/订阅消息，对负载内容屏蔽的消息传输

## 版本

MQTT5 是在 MQTT3.1.1 的基础上进行了升级，因此 MQTT5 是完全兼容 MQTT3.1.1 的

## MQTT两个端

- 服务端（broker）：是发布者，是MQTT 信息传输的枢纽，负责数据传递和客户端管理，确保客户端之间通讯顺畅
  - 客户端管理：当客户端连接到MQTT的服务器后，服务器通过客户端的心跳来检测客户端是否在线，若是某一个开发板心跳为，那么服务端会使用遗嘱机制来通知其他客户端当前客户端已经断开连接

- 客户端（client）：是订阅者也是发布者，且订阅和发布都是围绕着所选主题来进行的
  - “发布” :向服务器发布信息；“订阅”：从服务器收取信息

![image-20250608095011555](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202506080950668.png)

## MQTT特性

各个客户端相互独立，依然可以实现信息交流，客户端和服务端处于同一通信网络，且MQTT客户端在发送和接收信息的时候不需要时间同步

## QoS服务质量

**在无线网络中，中断和干扰很频繁，MQTT在这里帮助避免信息丢失及其服务质量水平**

报文在不可靠的信道上可能存在丢失，最直接的解决方法就是重发，QoS也可以看成重发机制了

这些级别在发布时使用，服务质量就是在表示报文要发送几次

- QoS0：最多发布一次，类似于UDP协议，我发送了一个消息，但是不管你接没接收到，我都不在发送了
- QoS1：至少发布一次，即会检测接收端是否返回了一个ack包，否则就重发
- QoS2：发布一次，接收端要发送两个ack包给发送端
  - ![image-20250608100447318](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202506081004368.png)

# MQTT协议报文结构

**固定报头+可变报头+有效负载**

- 固定报头**至少两个字节以上**，必选的，存在于所有MQTT的数据包中，表示数据包类型和数据包的分组类标识
- 可变报头是可选的，存在于部分MQTT数据包中，是否选择以及内容取决于固定报头中的数据包的类型
- 有效负载是可选的，存在于部分MQTT数据包中，表示客户端收到的具体内容，取决于固定报头中的数据包的类型

## 固定报头

![image-20250608101348332](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202506081013371.png)

第2个字节及之后的字节(至多4个字节)是剩余数据的长度，即可变报头+有效负载等等

> 报文类型
>
> ![image-20250608101503219](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202506081015293.png)

> 报文类型标志位
>
> ![image-20250608101821299](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202506081018379.png)

## 可变报头

> ![image-20250608102050057](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202506081020137.png)

## 有效负载

> ![image-20250608102106278](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202506081021388.png)

# MQTT协议图示了解

## 客户端与代理服务器建立连接

> ![image-20250608102757154](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202506081027211.png)

## 客户端向代理服务器订阅

> ![](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202506081028369.png)

## 客户端向代理服务器发布主题

> ![image-20250608102912444](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202506081029501.png)

# OneNET服务器

官方参考：[OneNET - 中国移动物联网开放平台](https://open.iot.10086.cn/doc/v5/develop/detail/251)

![image-20250608144417461](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202506081444508.png)

![image-20250608144430465](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202506081444511.png)

![image-20250608144512035](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202506081445104.png)

![image-20250608144603812](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202506081446855.png)

这里创建好的一个产品就相当于MQTT的一个服务端