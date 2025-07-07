# ICMP协议

ICMP协议是一个网络层协议。 一个新搭建好的网络，往往需要先进行一个简单的测试，来验证网络是否畅通；但是IP协议并不提供可靠传输。如果丢包了，IP协议并不能通知传输层是否丢包以及丢包的原因

为什么需要ICMP协议

- IP 协议本身不提供差错报告和差错控制机制来保证数据报递交的有效性，例如：数据报在网络中被丢弃了，源主机更希望等到该数据报递交过程中的异常信息。
- IP 协议不能进行主机管理与查询机制，例如：不知道对方主机或者路由器的活跃，对于不活跃的主机和路由器就没有必要发送数据报。即ping，ping不通就不发数据报

ICMP协议有两种报文

- 差错报文
- 查询报文：ping

# ICMP协议类型与结构

- 差错报告报文：目的不可达、源站抑制、重定向、超时、参数错误
  - lwip实现了目的不可达和超时的差错报文
    - 超时报文：第一个是TTL（生存时间），另一个是重装的时候的超时等待时间
- 询问报文：回送请求/回答（ping）、时间戳请求/回答
  - lwip实现了回送请求/回答

![image-20250429153943537](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202504291539588.png)

## ICMP差错报文

![image-20250502211905535](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202505022119652.png)

## ICMP查询报文

![image-20250502212208895](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202505022122957.png)

# 源码

## ICMP报文数据结构

![image-20250505182214593](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202505051822707.png)

## 差错报文

- 目的不可达：在传递过程中出错，不能到达目标主机，或到达目标主机后无法传递至上层协议，通过icmp_dest_unreach函数调用
- 超时差错：IP首部的“TTL”字段记录着该数据报的生命值，该数据报每被转发一次，TTL值减1。直到为0，丢弃该报，通过icmp_time_exceeded函数调用
- **这两种差错报文都是调用**icmp_send_response函数发送

### 原理

就是把丢弃的数据包IP首部和数据区域（前8字节）拷贝到差错报文当中

![image-20250505183652837](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202505051836924.png)

## 查询报文

请求报文发送，应答报文回复。应答包是在请求包的基础上修改得来

![image-20250505183726494](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/img/202505051837554.png)