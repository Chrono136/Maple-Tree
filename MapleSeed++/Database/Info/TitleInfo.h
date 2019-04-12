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

		static char * GenerateTMD(std::string _librarypath, std::string _id, string version = "");
		static char * GenerateTicket(std::string id);
		static char * GenerateTicket(TitleInfo * ti);

		TitleInfo * ParseTitleInfo(const char * str, size_t len);

		string SetLibraryPath(string path);
		string GetLibraryPath();
		string GetMetaXmlFile();
		string GetProductCode();
		string GetCoverArt();
		void UpdateContentType();

		int Download(string version = "");
		void Decrypt();

		MapleSeed::ContentType _type;

		const char* uid;
		const char* id;
		const char* key;
		const char* name;
		const char* region;
		const char* versions;
		const char* hasdlc;
		const char* haspatch;
		const char* contenttype;
		const char* cdn;
		const char* pcode;
		const char* ccode;
		const char* iloc;
		const char* notes;

	private:
		string LibraryPath = "";
	};
}