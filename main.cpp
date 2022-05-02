#include "NetworkAdapterTest.h"

int main()
{
	setlocale(LC_ALL, "");

	NetAdapterInfo test;

	test.GetAdaptersAddrs();
	test.GetAdaptersInfo();
	test.PrintAdaptersInfo();

	return 0;
}