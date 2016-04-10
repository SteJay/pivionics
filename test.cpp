#include <iostream>
using namespace std;

class BaseClass {
	public:
	void MyMethod(void) { cout << "BaseClass MyMethod called." << endl; }
	virtual void MyVirtual(void) { cout << "BaseClass MyVirtual called." << endl; }
	void CallBoth(void) {
		this->MyMethod();
		this->MyVirtual();
	}
};

class Derived: public BaseClass {
	public:
	void MyMethod(void) { cout << "Derived MyMethod called." << endl; }
	void MyVirtual(void) { cout << "Derived MyVirtual called." << endl; }
};


int main(int argc, char* argv[] ) {
	BaseClass* base=new BaseClass;
	Derived* der=new Derived;
	cout << "Calling methods on base class:" << endl;
	base->CallBoth();
	cout << "Calling methods on derived class:" << endl;
	der->CallBoth();
	cout << "Finally, try calling the derived class through a pointer of the base class type:" << endl;
	BaseClass* b = der;
	b->CallBoth();
	delete base;
	delete der;
	return 0;
}
