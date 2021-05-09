#include "objects.h"
#include "objects.cpp"

void test_simple_generic_object() {
	std::cout << "running test_simple_generic_test...\n";
	Ad_Object *obj = new Ad_Object();
	std::cout << &obj << " " << obj << "\n";
	delete obj;
}