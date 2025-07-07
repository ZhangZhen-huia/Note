# TCP协议

三次握手，四次挥手，数据传输，在不可靠的信道上完成可靠的传输

## 握手

![image-20250506154210617](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202505061542691.png)

## 挥手

![image-20250506154222850](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202505061542927.png)

## 数据传输

- 采用TCP协议进行数据传输，是不会造成IP分片的。若数据过大，只会在传输层进行数据分包。
- 采用UDP协议进行数据传输，若数据过大，只会在网络层进行数据分片，不能再传输层分包。
- TCP/IP 协议栈中定义了MSS, 为的是提高网络的性能，lwIP定义的MSS大小是1460（MTU - IP首部 - TCP首部）

问题：丢包问题和乱序问题

解决：两个协议

- 停止等待协议：一问一答，发送方发送一个数据，然后接收方接收到之后应答，接收方接收到应答后再发送下一个数据，一问一答。信道利用率和效率低
- 滑动窗口协议：确认之前多个数据发送。信道利用率和效率高

### 滑动窗口协议

设置一个窗口大小，假设为4，发送方一次发送4个数据，接收方准确的接收到了第一个数据之后会返回一个应答，发送方接收到之后会把窗口后移一个数据，然后继续发送

![image-20250506154737904](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202505061547974.png)

若是发生了丢包，由于每次发送的窗口数据都有三个会被再次发送一下即重传机制，所以每一个数据都有机会被自动重新发送一次

![image-20250506154854504](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202505061548571.png)

## 报文结构

![image-20250506172153858](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202505061721930.png)

```c
PACK_STRUCT_BEGIN 
struct tcp_hdr 
{ 
    PACK_STRUCT_FIELD(u16_t src); 
    PACK_STRUCT_FIELD(u16_t dest); 
    PACK_STRUCT_FIELD(u32_t seqno); 
    PACK_STRUCT_FIELD(u32_t ackno); 
    PACK_STRUCT_FIELD(u16_t  _hdrlen_rsvd_flags); 
    PACK_STRUCT_FIELD(u16_t wnd); 
    PACK_STRUCT_FIELD(u16_t chksum); 
    PACK_STRUCT_FIELD(u16_t urgp); 
} PACK_STRUCT_STRUCT; 
PACK_STRUCT_END
```

所以一共是2+2+4+4+2+2+2+2 = 20字节

# UDP协议

![image-20250506154910413](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202505061549490.png)

## 报文结构

![image-20250506154935910](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202505061549949.png)

```c
PACK_STRUCT_BEGIN 
struct udp_hdr 
{ 
	PACK_STRUCT_FIELD(u16_t src); 		/* 源端口号 */ 
	PACK_STRUCT_FIELD(u16_t dest); 	/* 目的端口号 */ 
	PACK_STRUCT_FIELD(u16_t len); 		/* UDP长度 */ 
    PACK_STRUCT_FIELD(u16_t chksum); 	/* 校验和 */ 
} PACK_STRUCT_STRUCT; 
PACK_STRUCT_END
```

所以UDP首部是2+2+2+2 = 8个字节，TCP首部是20个字节的，IP首部也是20字节

## UDP数据递交流程

- 在应用层：申请一个pbuf，带有54字节的首部（PBUF_TRANSPORT）
- 递交给传输层：偏移payload指针8个字节，然后添加UDP首部
- 递交给网络层：偏移payload指针添加IP首部（20字节）

![image-20250506160859309](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202505061608358.png)

## UDP控制块

```c
struct udp_pcb 
{ 
    IP_PCB; 							/* 通用IP控制块 */ 
    struct udp_pcb *next; 			/* 下一节点的指针，用于构成控制块链表 */ 
    u8_t flags; 						/* 控制块状态 */ 
    u16_t local_port, remote_port; 	/* 本地端口号、远程端口号 */ 
    
    /* 下面这两个若是使用RAW接口的话需要用户自己编写 ，使用NETCONN或者SOCKET则由lwip内核编写好了*/
    udp_recv_fn recv; 				/* 处理网络接收数据的回调 */ 
    void *recv_arg; 					/* 用户自定义参数，接收回调入参 */ 
};

```

![image-20250506161503418](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202505061615464.png)

### 控制块原理

网络层在接收到了一个数据包，通过查找IP首部里面的一个上层协议的字段，若是UDP协议，则会偏移payload指针到UDP首部，然后这个pbuf就是递交给`udp_input()`函数处理，此时pbuf由网络层递交给了传输层

![image-20250506162828022](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202505061628069.png)

***遍历udp_pcbs链表，检查其中的本地端口号与报文首部中的目的端口号是否匹配，并将数据包递交给匹配成功的控制块处理***

![image-20250506163157397](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202505061631454.png)

## RAW接口

**RAW/Callback API** **是** **lwIP** **的一大特色， 在没有操作系统支持的裸机环境中，只能使用这种** **API** **进行开发，同时这种** **API** **也可以用在操作系统环境中**

- 内存少，效率高，兼容强，切换少
- 可读性差，逻辑复杂，易用性差

![image-20250506164840171](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202505061648230.png)

```c
void udp_recv(struct udp_pcb *pcb, udp_recv_fn recv, void *recv_arg)
{
  LWIP_ASSERT_CORE_LOCKED();

  LWIP_ERROR("udp_recv: invalid pcb", pcb != NULL, return);

  /* 赋值结构体 */
  pcb->recv = recv;
  pcb->recv_arg = recv_arg;
}
```

![image-20250506171159050](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202505061712234.png)
