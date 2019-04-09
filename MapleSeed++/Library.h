#pragma once
namespace MapleSeed
{
	class Library
	{
	public:
		Library(string baseDir);
		~Library();

		vector<MapleSeed::TitleInfo> _db;

		string BaseDirectory;

		static Library *ref;
	};
}
