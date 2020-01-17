#include "DocumentManager.h"
#include <iostream>
#include <string>

int main(int argc, char * argv[])
{
	setlocale(LC_ALL, "zh-cn.UTF-8");
	if (argc == 1)
	{
		//visual mode
		std::string command;
		std::string lib1, lib2;
		while (command != "exit")
		{
			std::cout << "DocumentManager>";
			std::cin >> command;
			if (command == "status")
			{
				std::cin >> lib1;
				DocumentLibrary lib_buffer(lib1);
			}
			else if (command == "sync")
			{
				std::cin >> lib1 >> lib2;
				DocumentLibrary lib1_buffer(lib1);
				DocumentLibrary lib2_buffer(lib2);
				lib1_buffer.Synchronous(lib2_buffer);
			}
			else if (command != "exit")
			{
				std::cout << "error:\tunkown command" << std::endl;
			}
		}
	}
	else
	{
		if (std::string(argv[1]) == "status")
		{
			if (argc == 3)
			{
				DocumentLibrary lib(argv[2]);
			}
			else
			{
				std::cout << "error:\terror arguments" << std::endl;
				std::abort();
			}
		}
		else if (std::string(argv[1]) == "sync")
		{
			if (argc == 4)
			{
				DocumentLibrary lib1(argv[2]);
				DocumentLibrary lib2(argv[3]);
				lib1.Synchronous(lib2);
			}
			else
			{
				std::cout << "error:\terror arguments" << std::endl;
				std::abort();
			}
		}
	}
	return 0;
}