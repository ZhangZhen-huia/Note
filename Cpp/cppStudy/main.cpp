#include <iostream>
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

int main() {
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
}