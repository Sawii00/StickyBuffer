#include <iostream>
#include "StickyBuffer.h"
#include <string>

void printBuffer(const StickyBuffer &b) {
	std::cout << b.to_string();
}

int main()
{
	StickyBuffer buffer(16, false);
	buffer.add<int>(1);
	buffer.add<int>(2);
	buffer.add<int>(3);
	buffer.add<char>(64);
	buffer.add<char>(65);
	buffer.add<char>(66);
	buffer.add<char>(67);

	printBuffer(buffer);

	system("pause");
}