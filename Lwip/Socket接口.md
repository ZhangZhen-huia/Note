# Socket接口简介

类似于文件操作的一种网络连接接口，通常将其称之为“套接字”。lwIP的Socket接口兼容BSD Socket接口，***但只实现完整Socket的部分功能***

![image-20250508082847932](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202505080828031.png)

## 结构体

```c
struct sockaddr {
  u8_t        sa_len;		//结构体大小
  sa_family_t sa_family;	//协议簇	IPV4还是IPV6	
  char        sa_data[14];	//IP地址和端口号信息等，这个数组把目标地址和端口号混合在了一起，不好区分
};
```

下面的结构体与上面的结构体等价，下面的是把sa_data[14]拆开来存储了

```c
struct sockaddr_in {
  u8_t            sin_len;		//结构体大小
  sa_family_t     sin_family;	//协议簇	IPV4还是IPV6	
  in_port_t       sin_port;		//端口号
  struct in_addr  sin_addr;		//IP地址
#define SIN_ZERO_LEN 8
  char            sin_zero[SIN_ZERO_LEN];	//不使用
```

![image-20250508083215402](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202505080832484.png)

![image-20250508083844427](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202505080838481.png)

![image-20250508084002375](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202505080840459.png)

# Socket连接UDP单播

- ```c
  #define LWIP_SOCKET                     1	//置1表示使用Socket封装
  ```

- ```c
  int errno;	//在sys_arch.c文件里定义这一个变量
  ```

## 初始化结构体

```c
 memset(&g_local_info, 0, sizeof(struct sockaddr_in)); /* 将服务器地址清空 */
    g_local_info.sin_len = sizeof(g_local_info);
    g_local_info.sin_family = AF_INET;                    /* IPv4地址 */
    g_local_info.sin_port = htons(LWIP_DEMO_PORT);        /* 设置端口号 8080端口*/
    g_local_info.sin_addr.s_addr = htons(INADDR_ANY);     /* 设置本地IP地址 0.0.0.0，也可以设置为192.168.1.30用户自设置的*/
```

使用8080端口的原因：

- 8080 是大于 1024 的端口号，属于**非特权端口**（无需管理员权限即可绑定）
- 在测试环境中，8080 端口常被开放用于调试（尤其是在企业或实验室网络中），开发者使用 8080 可避免额外配置防火墙规则

使用0.0.0.0作为本地地址：为了实现 **多网络接口监听** 和 **动态适配 IP 环境**

- 嵌入式设备可能有多个网络接口（如以太网、Wi-Fi、蜂窝网络等），绑定到 `0.0.0.0` 可以让服务同时接收来自任意接口的请求。

- 在动态 IP 分配（如 DHCP）环境中，设备的 IP 地址可能频繁变化。若代码硬编码绑定到某个具体 IP 地址（如 `192.168.1.100`），当 IP 变化时服务将无法接收数据

- 在调试阶段，开发者无需提前知道设备的具体 IP 地址（尤其在 DHCP 网络中），直接绑定 `0.0.0.0` 即可快速启动服务

- 嵌入式系统的典型场景

  - **IoT 设备**：智能家居设备通过 Wi-Fi 和蓝牙同时连接，绑定 `0.0.0.0` 可统一接收云端指令和本地控制信号

  - **工业网关**：需同时监听 LAN 和蜂窝网络的 Modbus/HTTP 请求，通过 `INADDR_ANY` 简化多协议支持

## 调用socket函数建立连接

```c
lwip_socket(int domain, int type, int protocol)
//协议簇	AF_INET：IPV4	AF_INET6：IPV6	AF_PACKET：原始数据，嵌入式较少使用
//套接字	即TCP/UDP/原始套接字
//协议类型		IPPROTO_TCP		IPPROTO_UDP		0(根据type自动适配)
//返回值为类似于标识符句柄的东西
```

## 调用bind函数建立绑定

用于将套接字与**特定**的本地IP地址和端口绑定

所以，在udp广播中，由于本地ip并不需要使用bind函数

```c
socklen_t 	g_sock_fd;   
bind(g_sock_fd, (struct sockaddr *)&g_local_info, sizeof(struct sockaddr_in));    //建立绑定
```

一旦调用 `bind()` 成功，套接字的本地 IP 和端口将固定，除非显式调用 `close()` 关闭套接字并重新创建绑定。后续对结构体中本地 IP 的修改不会改变已绑定的套接字属性

## 调用recv函数进行接收

```c
int recv(int sockfd, void *buf, size_t len, int flags);
```

- **`sockfd`**：已建立连接的套接字描述符（TCP）或已绑定的套接字（UDP）。
- **`buf`**：接收数据的缓冲区指针。
- **`len`**：缓冲区的最大容量（字节数）。
- **`flags`**：控制接收行为的标志，常用值：
  - **`0`**：默认模式（阻塞接收）。
  - **`MSG_DONTWAIT`**：非阻塞模式（立即返回，无数据时返回 `-1`，`errno = EAGAIN`）。
  - **`MSG_PEEK`**：窥视数据（数据留在接收队列，下次 `recv()` 仍可读取）。

## 使用sys_thread_new函数创建发送线程

- lwIP 通过 `sys_thread_new` 抽象了底层操作系统的线程创建细节，使代码可移植到不同 RTOS（如 FreeRTOS、RT-Thread）或裸机环境

  - ```c
    sys_thread_t sys_thread_new(
        const char *name,         // 线程名称（调试用）
        void (*thread)(void *arg),// 线程入口函数
        void *arg,                // 传递给线程的参数
        int stacksize,            // 栈大小（单位：字节）
        int prio                  // 线程优先级
    );
    //返回值是线程句柄sys_thread_t 
    ```

    ```c
    /**
     * @brief       发送数据线程
     * @param       无
     * @retval      无
     */
    void lwip_data_send(void)
    {
        sys_thread_new("lwip_send_thread", lwip_send_thread, NULL, 512, LWIP_SEND_THREAD_PRIO );
    }
    //lwip_send_thread函数就是发送线程
    ```

    

## 使用sendto函数发送

- 发送之前要先设置远程IP地址

  - ```c
        g_local_info.sin_addr.s_addr = inet_addr(IP_ADDR);                /* 需要发送的远程IP地址 */
    ```

- ```c
  int sendto(
      int sockfd,                   // 套接字描述符（UDP 或 RAW）
      const void *buf,              // 待发送数据的缓冲区指针
      size_t len,                   // 待发送数据的长度（字节数）
      int flags,                    // 发送标志（通常设为 0）
      const struct sockaddr *dest_addr, // 目标地址结构体指针
      socklen_t addrlen             // 目标地址结构体长度
  );
  ```

- | 参数          | 说明                                                         |
  | :------------ | :----------------------------------------------------------- |
  | **sockfd**    | UDP 套接字描述符（需已绑定本地地址，若未绑定，系统会自动分配临时端口）。 |
  | **buf**       | 待发送数据的缓冲区（需确保数据在发送期间有效，避免悬空指针）。 |
  | **len**       | 数据长度（不应超过底层协议限制，如 UDP 最大载荷 65507 字节）。 |
  | **flags**     | 控制标志，常用值： - `0`：默认阻塞发送。 - `MSG_DONTWAIT`：非阻塞发送。 |
  | **dest_addr** | 目标地址结构体（`struct sockaddr_in` 或 `struct sockaddr_in6`），需包含 IP 和端口。 |
  | **addrlen**   | 目标地址结构体的实际大小（如 `sizeof(struct sockaddr_in)`）。 |

- 返回值为成功发送的字节数，或者返回-1表示发送失败

# UDP广播实验

首先需要两个Socket地址信息结构体，一个用来保存本地的网络信息g_local_info，一个用来指定广播的网络信息cast

g_local_info就是单片机的网络信息，cast就是广播的目标地址的网络信息，配置好后可以bind绑定一下本地的套接字，同时使用setsockopt函数设置允许udp广播

```c
opt = 1;
setsockopt(g_sock_fd,SOL_SOCKET,SO_BROADCAST,&opt,sizeof(opt));//配置套接字g_sock_fd的多种行为选项
```



## setsockopt()

```c
#include <sys/socket.h>

int setsockopt(
    int sockfd,         // 套接字描述符
    int level,          // 选项的协议层（如 SOL_SOCKET、IPPROTO_TCP）
    int optname,        // 选项名称（如 SO_REUSEADDR）
    const void *optval, // 指向选项值的指针
    socklen_t optlen    // 选项值的长度
);
//返回值：成功返回 0，失败返回 -1 并设置 errno。
```

### level

用于指定选项所属的协议层或功能模块，决定了可设置的选项类型

- **SOL_SOCKET**（通用选项值）常用选项
  - SO_REUSEADDR：允许地址和端口重用
  - SO_RCVTIMEO`/`SO_SNDTIMEO：设置接收/发送超时
  - SO_KEEPALIVE：启用 TCP 保活机制
  - SO_BROADCAST：允许 UDP 广播
- **IPPROTO_TCP**常用选项，配置 TCP 协议特有行为
  - TCP_NODELAY：禁用 Nagle 算法（减少小数据包延迟）。
  - TCP_KEEPIDLE：设置保活探测的空闲时间。
  - TCP_KEEPINTVL：设置保活探测间隔。
  - TCP_KEEPCNT：设置保活探测最大重试次数
- **IPPROTO_IP**（IPV4）常用选项
  - `IP_TTL`：设置数据包的生存时间（TTL）。
  - `IP_MULTICAST_IF`：指定组播的出口网络接口。
  - `IP_ADD_MEMBERSHIP`：加入 IPv4 组播组。
- **IPPROTO_IPV6**（IPV6）常用选项
  - `IPV6_V6ONLY`：限制套接字仅处理 IPv6 流量
  - `IPV6_MULTICAST_HOPS`：设置组播数据包的跳数限制
  - `IPV6_ADD_MEMBERSHIP`：加入 IPv6 组播组
- **IPPROTO_UDP**常用选项
  - `UDP_CORK`（Linux 特有）：合并多个小数据包发送
  - `UDP_SEGMENT`：控制数据包分段（需系统支持）
- **其他协议层**
  - SOL_RAW：用于原始套接字（Raw Socket）的选项。
  - SOL_PACKET（Linux 特有）：配置链路层数据包捕获。
  - 协议族特定选项：如 `SOL_BLUETOOTH`（蓝牙协议）。

### 选项值

![image-20250510202444728](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202505102025307.png)

```c
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <stdint.h>
#include <stdio.h>
#include "lwip/sockets.h"
#include "malloc.h"
#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/api.h"
#include "lwip_demo.h"


#define IP_ADDR   "192.168.1.111"	//定义电脑的IP地址
#define LOCAL_ADDR   "192.168.1.30"	//定义电脑的IP地址
#define LWIP_DEMO_RX_BUFSIZE         200    /* 最大接收数据长度 */
#define LWIP_DEMO_PORT               8080   /* 连接的本地端口号 */
#define LWIP_SEND_THREAD_PRIO       ( tskIDLE_PRIORITY + 3 ) /* 发送数据线程优先级 */


//接收数据缓冲区
uint8_t g_lwip_demo_recvbuf[LWIP_DEMO_RX_BUFSIZE];
//发送的数据内容
char g_lwip_demo_sendbuf[] = "Hello Mr.Zhang！\r\n";
//数据发送标志位
uint8_t g_lwip_send_flag;

// 定义Socket地址信息结构体
struct sockaddr_in 	g_local_info,cast;              
int opt;
//定义一个Socket接口
socklen_t g_sock_fd;      

extern QueueHandle_t g_display_queue;//显示消息队列句柄
void lwip_send_thread(void *pvParameters);



/**
 * @brief       发送数据线程
 * @param       无
 * @retval      无
 */
void lwip_data_send(void)
{
    sys_thread_new("lwip_send_thread", lwip_send_thread, NULL, 512, LWIP_SEND_THREAD_PRIO );
}


void lwip_demo(void)
{
	
	BaseType_t lwip_err;
	 
	memset(&g_local_info, 0, sizeof(struct sockaddr_in)); // 将服务器地址清空
	g_local_info.sin_family = AF_INET;										//选择IPV4地址
	g_local_info.sin_len = sizeof(g_local_info);		
	g_local_info.sin_port = htons(22222);				//设置本地端口号
	g_local_info.sin_addr.s_addr = inet_addr("192.168.1.30"); 		//设置本地IP地址 
	opt = 1;
	
	g_sock_fd = socket(AF_INET,SOCK_DGRAM,0);							//配置协议簇，套接字类型，协议类型，给0是自动适配
	setsockopt(g_sock_fd,SOL_SOCKET,SO_BROADCAST,&opt,sizeof(opt));//配置套接字g_sock_fd的多种行为选项

  bind(g_sock_fd, (struct sockaddr *)&g_local_info, sizeof(struct sockaddr_in));    //建立绑定
	lwip_data_send();
	while(1)
	{
		memset(g_lwip_demo_recvbuf, 0, sizeof(g_lwip_demo_recvbuf));//接收之前先清空接收缓冲区
    recv(g_sock_fd, (void *)g_lwip_demo_recvbuf, sizeof(g_lwip_demo_recvbuf), 0);//该接收函数是阻塞的
		lwip_err = xQueueSend(g_display_queue,g_lwip_demo_recvbuf,0);
		
	}
	
}	


/**
 * @brief       发送数据线程函数
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void lwip_send_thread(void *pvParameters)
{
    pvParameters = pvParameters;
    
//    g_local_info.sin_addr.s_addr = inet_addr(IP_ADDR);                /* 发送的远程IP地址 */
	cast.sin_family = 2;
		cast.sin_addr.s_addr = inet_addr("255.255.255.255");
		cast.sin_port = htons(8080);	
    while (1)
    {
        if ((g_lwip_send_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA)     /* 有数据要发送 */
        {
           sendto(g_sock_fd,                                         /* scoket */
                  (char *)g_lwip_demo_sendbuf,                        /* 发送的数据 */
                  sizeof(g_lwip_demo_sendbuf), 0,                     /* 发送的数据大小 */
                  (struct sockaddr *)&cast,                   /* 接收端地址信息 */ 
                  sizeof(cast));                              /* 接收端地址信息大小 */

            g_lwip_send_flag &= ~LWIP_SEND_DATA;
        }
        
        vTaskDelay(100);
   }
}
```

# UDP组播/多播实验

组播（Multicast）是计算机网络中一种**“一对多”或“多对多”**的数据传输方式。它允许数据从一个或多个发送者同时高效地传输到**一组特定的接收者**（称为“组播组”），而不是向所有设备广播或逐个单播

只有加入特定组播组的设备才会接收数据，网络负载低

| 传输方式 | 目标对象           | 网络负载                 | 典型场景                     |
| :------- | :----------------- | :----------------------- | :--------------------------- |
| **单播** | 一对一（如私聊）   | 高（每份数据单独传输）   | 网页浏览、文件下载           |
| **广播** | 一对所有（全网段） | 极高（所有设备强制接收） | 局域网地址发现（如ARP）      |
| **组播** | 一对多（特定组）   | 低（数据仅复制必要次数） | 视频会议、IPTV、股票行情推送 |

## 修改代码

hal库版本不同，放置的位置也不一样，但是要修改的代码是一样的

### bsp_ethernet.c

ETH_MACDMAConfig()函数

```c
macinit.ReceiveAll = ETH_RECEIVEAll_DISABLE;
macinit.MulticastFramesFilter = ETH_MULTICASTFRAMESFILTER_PERFECT;

//修改为
macinit.ReceiveAll = ETH_RECEIVEALL_ENABLE;	//使能全部接收
macinit.MulticastFramesFilter = ETH_MULTICASTFRAMESFILTER_NONE;  //不过滤组播的帧	
```

### lwipopts.h

```c
/* ---------- IGMP 选项 ---------- */
#define LWIP_IGMP                       1
//如果没有就自己定义一下
```

### ethernetif.c

```c
//low_level_init函数，添加标志位	NETIF_FLAG_IGMP

netif->flags = NETIF_FLAG_BROADCAST|NETIF_FLAG_ETHARP|NETIF_FLAG_LINK_UP|NETIF_FLAG_IGMP;   /*广播 ARP协议 链接检测*/

```

## 实验代码

### ip_mreq结构体

```c
#include <netinet/in.h>

struct ip_mreq {
    struct in_addr imr_multiaddr;  // 组播组的 IPv4 地址，指定要加入或离开的组播组的 IPv4 地址，必须是 D 类地址（范围 224.0.0.0 到 239.255.255.255），224.0.0.1（本地网络所有组播设备）
    struct in_addr imr_interface;  // 指定本地网络接口的 IPv4 地址，用于绑定组播数据的收发
};
```

