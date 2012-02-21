// kamea.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "file.h"
#include "opcodes.h"
#include "operations.h"
#include "errors.h"

using namespace Kamea;
using namespace std;

void test1(void)
{
	ifstream str1("D:\\Work\\source\\SEMIL\\KAMEA\\drawing.kam");
	if (!str1)
		throw runtime_error("open error");
	Program program = load(str1);
	ofstream str2("out.kam");
	if (!str2)
		throw runtime_error("open error");
	save(str2, program);
}

void test2(void)
{
	Program program;
	program.addCommand(auto_ptr<command>(new CCOMMENT("xxx")));
	{
		ofstream str2("out.kam");
		if (!str2)
			throw runtime_error("open error");
		save(str2, program);
	}

	{
		ifstream str1("out.kam");
		if (!str1)
			throw runtime_error("open error");
		program = load(str1);
	}
}

void test3(void)
{
	ifstream str1("c:\\semil\\files\\44.kam");
	if (!str1)
		throw runtime_error("open error");
	Program program = load(str1);
}

int _tmain(int argc, _TCHAR* argv[])
{
	using std::cout;
	using std::cin;

	try {
		test3();
	}
	catch (invalid_command_length &ex)
	{
		//MessageBox(NULL, ex.what(), NULL, 0);
		cout << ex.what();
	}
	catch (parse_error &ex)
	{
		MessageBox(NULL, ex.what(), NULL, 0);
	}
	catch (exception &ex)
	{
		//MessageBox(NULL, ex.what(), NULL, 0);
		cout << ex.what();
	}
	return 0;
}

