#include <iostream>
#include <stdio.h>
using namespace std;

// ����
class Base {
public:
    virtual void show() {
        cout << "Base::show()" << endl;
    }
};

// ������
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
    ptr.show();  // �����Derived::show()

    // ǿ������ת��
    Derived& dPtr = dynamic_cast<Derived&>(ptr);

    dPtr.specialFunction();  // �����Derived::specialFunction()
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

    //�Ӽ��̶������ݣ����浽boya
    printf("Input data:\n");
    for(i=0; i<N; i++,pa++){
        scanf("%s %d %d %f", pa->name, &pa->num, &pa->age, &pa->score);   
    }
    //pa = boya;
    //��boya�е�����д�뵽�ļ�
    for(i=0; i<N; i++,pa++){
        fprintf(fp,"%s %d %d %f\n", pa->name, pa->num, pa->age, pa->score);   
    }
    //�����ļ�ָ��
    rewind(fp);
    //���ļ��ж�ȡ���ݣ����浽boyb
    for(i=0; i<N; i++,pb++){
        fscanf(fp, "%s %d %d %f\n", pb->name, &pb->num, &pb->age, &pb->score);
    }
    pb=boyb;
    //��boyb�е������������ʾ��
    for(i=0; i<N; i++,pb++){
        printf("%s  %d  %d  %f\n", pb->name, pb->num, pb->age, pb->score);
    }

    fclose(fp);
    return 0;
}