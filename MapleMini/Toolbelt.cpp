#include "stdafx.h"
#include "Toolbelt.h"


std::string Toolbelt::getUserInput()
{
	std::string input = "";

	getline(std::cin, input);

	return input;
}

//TODO: probably a memory leak, don't care(yet)
char* Toolbelt::StringToCharArray(std::string str)
{
	char *cstr = new char[str.length() + 1];
	strcpy(cstr, str.c_str());

	return cstr;
	delete[] cstr;
}

char * Toolbelt::DownloadBytes(char *)
{
	return nullptr;
}

void Toolbelt::WriteLine(const char * text)
{
	cout << rang::style::bold << text << rang::style::reset << endl;
}

void Toolbelt::WriteLineRed(const char * text)
{
	cout << rang::fg::red << text << rang::style::reset << endl;
}

Toolbelt::Toolbelt()
{
}


Toolbelt::~Toolbelt()
{
}
