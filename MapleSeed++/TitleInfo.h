namespace MapleSeed
{
#pragma once

	class TitleInfo
	{
	public:
		TitleInfo(const char * id);
		TitleInfo(char* str, size_t len);
		TitleInfo();
		~TitleInfo();

		static TitleInfo * GetTitleInfo(const char * id);

		static char * GenerateTMD(std::string working_dir, std::string _id);
		static char * GenerateTicket(std::string id);
		static char * GenerateTicket(TitleInfo * ti);

		TitleInfo * ParseTitleInfo(const char * str, size_t len);

		string SetLibraryPath(string path);
		string GetLibraryPath();
		string GetMetaXmlFile();
		string GetProductCode();
		string GetCoverArt();

		int Download();
		void Decrypt();

		char* uid;
		char* id;
		char* key;
		char* name;
		char* region;
		char* versions;
		char* hasdlc;
		char* haspatch;
		char* contenttype;
		char* cdn;
		char* pcode;
		char* ccode;
		char* iloc;
		char* notes;

	private:
		string LibraryPath = "";
	};
}