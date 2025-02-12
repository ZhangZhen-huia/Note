#include <iostream>
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

int main() {
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
}