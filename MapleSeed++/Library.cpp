#include "stdafx.h"
#include "Library.h"


Library *Library::ref;

Library::Library(string baseDir)
{
	Library::ref = this;
	BaseDirectory = baseDir;
	BaseDirectory = "D:\\Emulation\\Nintendo Wii U\\roms";

	recursive_directory_iterator dir(BaseDirectory), end;
	while (dir != end)
	{
		auto fn = dir->path().filename();
		if (fn == "meta.xml")
		{
			WriteLine("Loading: '%s'", dir->path().generic_string().c_str());


		}

		++dir;
	}

	WriteLineBold("Working Directory: '%s'", BaseDirectory.c_str());
}

Library::~Library()
{
}
