#include "stdafx.h"
#include "Library.h"


Library *Library::ref;

Library::Library(string baseDir)
{
	Library::ref = this;

	if (!LoadBaseDirectory())
	{
		BaseDirectory = baseDir;
	}

	Load();

	WriteLineBold("Working Directory: '%s'", BaseDirectory.c_str());
}

Library::~Library()
{
}

void Library::Load()
{
	recursive_directory_iterator dir(BaseDirectory), end;
	while (dir != end)
	{
		if (dir->path().filename() == "meta.xml")
		{
			WriteLine("Loading: '%s'", dir->path().generic_string().c_str());
			string title_id(GetMetaXmlValue(dir->path().generic_string(), "title_id"));
			auto ti = TitleInfo::GetTitleInfo(title_id.c_str());
			_db.push_back(std::move(ti));
		}
		++dir;
	}
}

bool Library::LoadBaseDirectory()
{
	if (FileExists("config.xml"))
	{
		string base(GetMetaXmlValue("config.xml", "Base"));

		if (DirExists(base.c_str()))
		{
			BaseDirectory = base;

			return true;
		}
	}
	return false;
}

void Library::SaveBaseDirectory(string dir)
{
	if (DirExists(dir.c_str()))
	{
		SetConfigXmlValue("Base", BaseDirectory = dir);
	}
	else
	{
		WriteLineRed("Error saving confi. (%s)", dir.c_str());
	}
}

string Library::GetMetaXmlValue(string fp, string field)
{
	boost::property_tree::ptree pt1;
	boost::property_tree::read_xml(fp, pt1);
	boost::property_tree::ptree &menu = pt1.get_child("menu");
	boost::property_tree::ptree &title_id = menu.get_child(field);
	return title_id.get_value<std::string>();
}

void MapleSeed::Library::SetConfigXmlValue(string field, string value)
{
	boost::property_tree::ptree tree;
	string cstr("menu.");

	tree.put(cstr + field, value);

	boost::property_tree::write_xml("config.xml", tree);
}
