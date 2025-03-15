[TOC]

==使用 string 类需要包含头文件`<string>`==

# 定义string变量

```c++
#include <iostream>
#include<string>
using namespace std;

int main()
{
    string s1 = "c++";
    string s2(5,'w');
    string s3;
    string s4 = s1;
    cout <<s1<<'\n'<<s2<<'\n'<<s3<<'\n'<<s4<<'\n'<<endl;
  return 0;
}

输出：
c++
wwwww

c++

```

==在s1中，与C风格的字符串不同，string 的结尾没有结束标志'\0'。==

# 获取字符串长度

可以调用 string 类提供的 length() 函数

```c++
#include <iostream>
#include<string>
using namespace std;


int main()
{
    string s1 = "c++";
    int len = s1.length();
    cout << len;
  return 0;
}

输出：3
```

==由于 string 的末尾没有`'\0'`字符，所以 length() 返回的是字符串的真实长度，而不是长度 +1==

# 转换为C风格的字符串（涉及文件操作）

当使用c语言打开文件时的路径时，必须要使用C风格的字符串，为此需要转换为C语言的字符串

c_str()，该函数能够将 string 字符串转换为C风格的字符串，并返回该字符串的 const 指针（const char*）

```c++
#include <iostream>
#include <string>
#include <stdio.h>
using namespace std;

int main()
{
    int c;
    string path = "C:/Users/a1874/Desktop/Remote_Note.txt";
    FILE *fp = fopen(path.c_str(),"r");
    while(1)
   {
      c = fgetc(fp);
      if( feof(fp) )
      { 
          break ;
      }
      printf("%c", c);
   }
  return 0;
}

输出：git@github.com:ZhangZhen-huia/Note.git

```

# string字符串的输入输出

```c++
#include <iostream>
#include<string>
using namespace std;


int main()
{
    string s;
    cin >>s;
    cout<<s<<endl;
  return 0;
}

结果：
指针  指针宿舍
指针
```

输入的时候中间打了一个空格，而输入运算符`>>`默认会忽略空格，遇到空格就认为输入结束，所以最后输入的指针宿舍没有被存储到变量 s

# 访问字符串中的字符

```c++
#include <iostream>
#include<string>
using namespace std;


int main()
{
    string s = "3466493941";
    
    for(uint8_t i=0;i<s.length();i++)
    cout << s[i] << ' ';
    s[0] = '9';
    cout << '\n' << s <<endl;
  return 0;
}

结果：
3 4 6 6 4 9 3 9 4 1
9466493941
```

# 字符串的拼接

有了 string 类，我们可以使用`+`或`+=`运算符来直接拼接字符串，非常方便，再也不需要使用C语言中的 **strcat()、strcpy()、malloc()** 等函数来拼接字符串了，再也不用担心空间不够会溢出了

```c++
#include <iostream>
#include<string>
using namespace std;


int main()
{
    string s1 = "1a";
    string s2 = "2a";
    char *s3 =  "3a";
    char s4[] = "4a";
    char s5 = 'a';


    string s6 = s1+s2;
    string s7 = s1+s3;
    string s8 = s1+s4;
    string s9 = s1+s5;

    cout << s6 << endl << s7 << endl << s8 << endl << s9 <<endl;
    cout << (s1+=s2) << endl << (s2+=s3);
  return 0;
}

结果：
1a2a
1a3a
1a4a
1aa
1a2a3a
2a3a

```

用+来拼接字符串时，运算符的两边可以都是 string 字符串，也可以是一个 string 字符串和一个C风格的字符串，还可以是一个 string 字符串和一个字符数组，或者是一个 string 字符串和一个单独的字符

用+=的话两边的类型必须是一样的，比如s5就不能和*s3用+=连接,否则会报错

***注意：***cout的运算顺序是从右往左，所以先计算s2+=s3，将计算后的s2压栈，然后接着计算s1+=s2。输出顺序是从左往右，所以输出结果如上。

# 插入字符串

函数原型：`string& insert (size_t pos, const string& str);`

pos 表示要插入的位置，也就是下标,插入的位置是第pos个字符的后面；str 表示要插入的字符串，它可以是 string 字符串，也可以是C风格的字符串

- **0 ≤ pos ≤ std::string::size()**

```c++
#include <iostream>
#include<string>
using namespace std;


int main()
{
    string s1 = "aaaa";
    string s2 = "bbbb";

    s1.insert(0,"ccc");
    cout << s1 <<endl;
    s1.insert(0,s2);
    cout <<s1<<endl;

  return 0;
}
结果：
cccaaaa
bbbbcccaaaa
```

# 删除字符串

函数原型：`string& erase (size_t pos = 0, size_t len = npos);`

pos 表示要删除的子字符串的起始下标，len 表示要删除子字符串的长度。如果不指明 len 的话，那么直接删除从 pos 到字符串结束处的所有字符（此时 len = str.length - pos）

```c++
#include <iostream>
#include<string>
using namespace std;


int main()
{
    string s1 = "aaaa";
    string s2 = "bbbb";
    s1.insert(0,"ccc");
    cout << s1 <<endl;
    s1.erase(0,3);
    cout <<s1<<endl;
    s1.erase(0);
    cout <<s1<<endl;
  return 0;
}

结果：
cccaaaa
aaaa
（这一行字符串全部被删除，所以打印出来空白）
```

**待删除字符串最多只能删除到字符串结尾**，erase() 函数会从以下两个值中取出最小的一个作为待删除子字符串的长度：

- len 的值；
- 字符串长度减去 pos 的值

# 提取字符串

原型：`string substr (size_t pos = 0, size_t len = npos) const;`

pos 为要提取的子字符串的起始下标，len 为要提取的子字符串的长度。

```c++
#include <iostream>
#include <string>
using namespace std;
int main(){
    string s1 = "first second third";
    string s2;
    s2 = s1.substr(6, 6);
    cout<< s1 <<endl;
    cout<< s2 <<endl;
    return 0;
}
结果：
first second third
second
```

如果 pos 越界，会抛出异常；

如果 len 越界，会提取从 pos 到字符串结尾处的所有字符

# 查找字符串

find() 函数用于在 string 字符串中查找子字符串出现的位置，从第二个参数处开始查找

rfind() 函数，查找到第二个参数处

find_first_of() 函数用于查找子字符串和字符串共同具有的字符在字符串中首次出现的位置