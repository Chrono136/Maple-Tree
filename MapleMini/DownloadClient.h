#pragma once
class DownloadClient
{
public:
	DownloadClient(const char *url, const char* fileName = 0, unsigned long filesize = 0, bool toFile = 0, bool resume = 0);
	DownloadClient() {}
	~DownloadClient();

	//url-> the remote location of the file to obtain
	//fn-> the local file name to save remote file as
	void DownloadFile(const char *url, const char*fn);

	void DownloadData(const char *url, const char* fileName, unsigned long filesize, bool toFile, bool resume);

	char *buf;
	u32 len;
	char *err;
};

