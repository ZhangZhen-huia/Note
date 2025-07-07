# 简介

- 地址解析协议，即ARP（Address Resolution Protocol），是根据IP地址获取物理地址（MAC地址）的一个TCP/IP协议。
- ARP协议就是根据目标IP地址以广播方式获取相应的MAC地址，并将MAC地址存入ARP缓存表

![image-20250427164427898](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504271644932.png)

广播形式的目标MAC地址：0xff,0xff,0xff,0xff,0xff,0xff

![image-20250427175600480](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504271756506.png)

# MAC地址与IP地址作用

- IP地址：用于网络寻址，它作用空间是广域网
  - 可变，解决数据在外网（互联网）的传输问题
  - 类似于告诉你在哪个小区
- MAC地址：用于链路层寻址，它作用空间是局域网
  - 不可变，不重复的，唯一的
  - 类似于告诉你在那栋楼，哪个房间

- 若两个主机的IP地址在同一网段（普通意义上的同一网段就是指IP地址前三段相同，最后一段不同），那么就可以直接通过MAC地址进行通信，即MAC地址是作用于局域网
  - ![image-20250427165257842](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504271652875.png)
- 若两个主机的IP不在同一网段，那么就需要先把信息传输到网关（同一网段下的路由器种），然后通过IP地址进行寻址，找到相应的网段，再传输到主机中，即IP地址是作用于广域网
  - ![image-20250427165305275](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504271653308.png)

# ARP协议原理

![image-20250428102834137](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504281028207.png)

## 发送ARP请求包原理

![image-20250427174833972](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504271748998.png)

![image-20250428100434575](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504281004665.png)

### 缓存表内容结构体

```c
struct etharp_entry {
  /** Pointer to a single pending outgoing packet on this ARP entry. */
  struct pbuf *q;				//挂起数据，检查有没有目标MAC地址，没有就挂起数据包
  ip4_addr_t ipaddr;			//目标的地址
  struct netif *netif;			//当前使用的网卡
  struct eth_addr ethaddr;		//MAC地址
  u16_t ctime;				   //保存的时间，如当前表项没有作用，那就会定时删除
  u8_t state;				   //状态
};
```

- ARP缓存表首先要判断IP地址是否有一个对应的MAC地址（表项处于ETHARP_STATE_EMPTY状态）
- 如果没有，就会创建一个表项，里面存放了目标的IP地址，但是没有映射MAC地址（处于ETHARP_STATE_PENDING状态），然后主机发送一个ARP请求包到对方主机中
- 对方主机发送过来一个应答包（处于ETHARP_STATE_STABLE状态）

```c
/** ARP states */
enum etharp_state {
  ETHARP_STATE_EMPTY = 0,			//ARP缓存表处于初始化的状态
  ETHARP_STATE_PENDING,				//只记录IP地址, 还没有获取MAC地址
  ETHARP_STATE_STABLE,				//应答前, 数据包被挂起了, 应答以后更新以及发送数据
  ETHARP_STATE_STABLE_REREQUESTING_1,//发送ARP应答包状态1
  ETHARP_STATE_STABLE_REREQUESTING_2 //发送ARP应答包状态1
};
```

- 单播模式下，`etharp_output()`函数会去遍历缓存表，根据数据包的一个IP地址去查找对应的表项，找到了就直接调用`etharp_output_to_arp_index()`函数发送，没有找到就调用`etharp_query()`函数
  - `etharp_output_to_arp_index()`函数最后也会调用`ethernet_output()`函数发送
  - `etharp_query()`函数会创建一个ARP表项（如果没有找到IP地址对应的表项），然后发送ARP请求包，通过`etharp_request()`---->`etharp_request_dst()`------>`etharp_raw()`----->`ethernet_output()`函数发送
- 广播或者组播会直接调用`ethernet_output()`函数发送

## 接收ARP应答包原理

![image-20250428102505917](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504281025994.png)

## ARP报文协议

![image-20250428101902355](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504281019418.png)

