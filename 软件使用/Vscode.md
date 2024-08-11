# Vscode运行C程序的一些问题

## 一、F5调试功能错误-修改launch.json文件


![Vscode.md202408112333002](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/软件使用/Vscode.md202408112346689.png)

这个错误告诉我们这个文件不存在，也就是这个**program**的位置是错的



<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/软件使用/Vscode.md202408112346471.png" alt="Vscode.md202408112335661" style="zoom:50%;" />

实际上我们的文件是放在了
<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/软件使用/Vscode.md202408112347777.png" alt="Vscode.md202408112335567" style="zoom:50%;" />



**main.exe**文件在这里，那我们就把这里的位置修改一下就行了
<img src="https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/软件使用/Vscode.md202408112347369.png" alt="Vscode.md202408112336643" style="zoom:50%;" />

同时在末尾加上这一串东西

**<u>${fileBasenameNoExtension}.exe</u>**



## 二、运行C语言终端闪退

下载这个扩展

![Vscode.md202408112337507](https://raw.githubusercontent.com/ZhangZhen-huia/Note/main/软件使用/Vscode.md202408112347297.png)



然后按**F6**执行就可以了

