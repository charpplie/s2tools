#include <iostream>
#include "appframework.h"

#include <string>
std::string test = App::GetAppInstallDir(570);

int main()
{
	std::cout << test;
	return 0;
}