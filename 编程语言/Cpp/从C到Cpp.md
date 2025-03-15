[TOC]

# C++类和对象

C++ 中的类（Class）可以看做C语言中结构体（Struct）的升级版。结构体是一种构造类型，可以包含若干成员变量，每个成员变量的类型可以不同；可以通过结构体来定义结构体变量，每个变量拥有相同的性质

C++ 中的类也是一种构造类型，但是进行了一些扩展，类的成员不但可以是变量，还可以是函数；通过类定义出来的变量也有特定的称呼，叫做**“对象”**。例如：

```c++
typedef class
{
    public:
    int year;
    int month;
    int day;
    void show()
    {
        cout << "年" << year << ",月:" << month << ",日:" << day << endl;
    }
}Data_t;
```

# C++源文件后缀

C语言源文件的后缀非常统一，在不同的编译器下都是`.c`。C++ 源文件的后缀则有些混乱，不同的编译器支持不同的后缀，下表是一个简单的汇总：

| 编译器 | Microsoft Visual C++ | GCC（GNU C++）       | Borland C++ | UNIX       |
| ------ | -------------------- | -------------------- | ----------- | ---------- |
| 后缀   | cpp、cxx、cc         | cpp、cxx、cc、c++、C | cpp         | C、cc、cxx |

# C++命名空间

为了解决合作开发时的命名冲突问题，[C++](https://c.biancheng.net/cplus/) 引入了**命名空间（Namespace）**的概念。请看下面的例子

```c++
namespace Li{  //小李的变量定义
    FILE* fp = NULL;
}
namespace Han{  //小韩的变量定义
    FILE* fp = NULL;
}

namespace 是C++中的关键字，用来定义一个命名空间：
namespace name{
    //variables, functions, classes
}
```

使用变量、函数时要指明它们所在的命名空间。以上面的 fp 变量为例，可以这样来使用：

```c++
Li::fp = fopen("one.txt", "r");  //使用小李定义的变量 fp
Han::fp = fopen("two.txt", "rb+");  //使用小韩定义的变量 fp
```

`:`是一个新符号，称为域解析操作符，在C++中用来指明要使用的命名空间

```c++
using关键字：声明命名空间
    using namespace std;//standard标准命名空间
	using std::fp;
表示using 声明以后的程序中如果出现了未指明命名空间的变量（函数，类，typedef、#define等等）都默认为std中的
```

# C++头文件和std命名空间

为了避免头文件重名，新版 C++ 库也对头文件的命名做了调整，去掉了后缀`.h`，所以老式 C++ 的`iostream.h`变成了`iostream`，`fstream.h`变成了`fstream`。而对于原来C语言的头文件，也采用同样的方法，但在每个名字前还要添加一个`c`字母，所以C语言的`stdio.h`变成了`cstdio`，`stdlib.h`变成了`cstdlib`。

旧的 C++ 头文件，如 iostream.h、fstream.h 等将会继续被支持，尽管它们不在官方标准中。这些头文件的内容不在命名空间 std 中。

新的 C++ 头文件，如 iostream、fstream 等包含的基本功能和对应的旧版头文件相似，但头文件的内容在命名空间 std 中。

标准C头文件如 stdio.h、stdlib.h 等继续被支持。头文件的内容不在 std 中。

具有C库功能的新C++头文件具有如 cstdio、cstdlib 这样的名字。它们提供的内容和相应的旧的C头文件相同，只是内容在 std 中。

对于不带`.h`的头文件，所有的符号都位于命名空间 std 中，使用时需要声明命名空间 std；对于带`.h`的头文件，没有使用任何命名空间，所有符号都位于全局作用域。这也是 C++ 标准所规定的。

# C++输入输出

在编写 C++ 程序时，如果需要使用输入输出时，则需要包含头文件`iostream`

cout 和 cin 都是 C++ 的**内置对象**，而**不是关键字**

使用 cout 进行输出时需要紧跟`<<`运算符

使用 cin 进行输入时需要紧跟`>>`运算符

二者都可以连续输出或者输入，输入运算符`>>`在读入下一个输入项前会忽略前一项后面的空格

# C++ new和delete

在c语言中用malloc()来动态分配内存，用free()来释放内存

```c++
int *p = (int*) malloc( sizeof(int) * 10 );  //分配10个int型的内存空间
free(p);  //释放内存
也就是说，使用 new 在堆上创建出来的对象是匿名的，没法直接使用，必须要用一个指针指向它，再借助指针来访问它的成员变量或成员函数。
```

在c++中又新增加了两个***关键字***new和delete

```c++
int *p = new int;  //分配1个int型的内存空间
delete p;  //释放内存
```

new 操作符会根据后面的数据类型来推断所需空间的大小

如果希望分配一组连续的数据，可以使用 new[]，释放也是用delete[]

```c++
int *p = new int[10];  //分配10个int型的内存空间delete[] p;
```

# C++内联函数

内联函数类似于宏定义，会把函数的本体直接移动到调用处，而省去了函数调用机制产生的时间，内联一般给到内容较为少的函数

指定内联函数的方法很简单，只需要在函数定义处增加 inline 关键字

# c++函数默认参数

```c++
int d = 100;

void showNumber(int a,int b =d + 2,int c = 0)
{
    cout << a << '\n' << b << '\n' << c <<'\n';
}

int main()
{
    //Data0.set_data(2025,1,22);
    showNumber(111);
    //Data0.show();
    return 0;
}

输出：
111
102
0
```

注意事项：

默认参数必须放到形参的最后，一旦有一个默认参数，则该参数之后的形参也必须有一个默认参数,即不能这么写

```c++
void showNumber(int a,int b =d + 2,int c)
{
    cout << a << '\n' << b << '\n' << c <<'\n';
}

```

# C++函数重载

C++ 允许多个函数拥有相同的名字，只要它们的***参数列表不同***就可以，这就是函数的重载（Function Overloading）

参数列表又叫参数签名，包括参数的类型、参数的个数和参数的顺序，只要有一个不同就叫做参数列表不同,而仅仅是参数名称或者返回类型不同则不叫重载。

C++代码在编译时会根据参数列表对函数进行重命名，例如`void Swap(int a, int b)`会被重命名为`_Swap_int_int`，`void Swap(float x, float y)`会被重命名为`_Swap_float_float`。当发生函数调用时，编译器会根据传入的实参去逐个匹配，以选择对应的函数，如果匹配失败，编译器就会报错，这叫做重载决议（Overload Resolution）