#pragma once
class DownloadClient
{
public:
	DownloadClient(const char *url, const char* fileName = 0, unsigned long filesize = 0, bool toFile = 0, bool resume = 0);
	DownloadClient() {}
	~DownloadClient();

	void DownloadData(const char *url, const char* fileName, unsigned long filesize, bool toFile, bool resume);

	char *dataBytes;
	u32 length;
	char *error;
};

