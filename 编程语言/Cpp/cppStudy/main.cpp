#include <iostream>
#include <stdio.h>
using namespace std;

// 基类
class Base {
public:
    virtual void show() {
        cout << "Base::show()" << endl;
    }
};

// 派生类
class Derived : public Base {
public:
    void show() override {
        cout << "Derived::show()" << endl;
    }
    void specialFunction() {
        cout << "Derived::specialFunction()" << endl;
    }
};

/* int main() {
    Derived D1;
    Base& ptr = D1;
    Base b1;
    ptr.show();  // 输出：Derived::show()

    // 强制类型转换
    Derived& dPtr = dynamic_cast<Derived&>(ptr);

    dPtr.specialFunction();  // 输出：Derived::specialFunction()
    ptr = b1;
    ptr.show();

    return 0;
} */



#define N 2

struct stu{
    char name[10];
    int num;
    int age;
    float score;
} boya[N], boyb[N], *pa, *pb;

int main(){
    FILE *fp;
    int i;
    pa=boya;
    pb=boyb;
    if( (fp=fopen("D:\\demo.txt","wt+")) == NULL ){
        puts("Fail to open file!");
        exit(0);
    }

    //从键盘读入数据，保存到boya
    printf("Input data:\n");
    for(i=0; i<N; i++,pa++){
        scanf("%s %d %d %f", pa->name, &pa->num, &pa->age, &pa->score);   
    }
    //pa = boya;
    //将boya中的数据写入到文件
    for(i=0; i<N; i++,pa++){
        fprintf(fp,"%s %d %d %f\n", pa->name, pa->num, pa->age, pa->score);   
    }
    //重置文件指针
    rewind(fp);
    //从文件中读取数据，保存到boyb
    for(i=0; i<N; i++,pb++){
        fscanf(fp, "%s %d %d %f\n", pb->name, &pb->num, &pb->age, &pb->score);
    }
    pb=boyb;
    //将boyb中的数据输出到显示器
    for(i=0; i<N; i++,pb++){
        printf("%s  %d  %d  %f\n", pb->name, pb->num, pb->age, pb->score);
    }

    fclose(fp);
    return 0;
}