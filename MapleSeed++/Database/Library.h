#pragma once
namespace MapleSeed
{
	class Library
	{
	public:
		Library(string baseDir);
		~Library();

		void Load();

		bool LoadBaseDirectory();

		void SaveBaseDirectory(string dir);

		string GetMetaXmlValue(string fp, string field);

		vector<MapleSeed::TitleInfo*> _db;

		string BaseDirectory;

		static Library *ref;

		static string GetBaseDirectory()
		{
			if (ref)
				return ref->BaseDirectory;
			else
				return "";
		}

	private:
		void SetConfigXmlValue(string field, string value);
	};
}
