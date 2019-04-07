namespace MapleSeed
{
#pragma once

	class TitleInfo
	{
	public:
		TitleInfo(char* str, size_t len);
		TitleInfo();
		~TitleInfo();

		static TitleInfo * GetTitleInfo(const char * id);

		TitleInfo * ParseTitleInfo(const char * str, size_t len);

		static char * GenerateTMD(std::string working_dir, std::string _id);

		static char * GenerateTicket(std::string id);
		static char * GenerateTicket(TitleInfo * ti);

		void SetDirectory(std::string output_root);

		int DownloadContent();

		std::string workingDir;
		static std::string outputDir;

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
	};
}