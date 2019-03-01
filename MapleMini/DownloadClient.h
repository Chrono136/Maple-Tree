#pragma once
class DownloadClient
{
public:
	DownloadClient(const char *url, const char* fileName = 0, bool toFile = 0);
	~DownloadClient();

	void DownloadData(const char *url, const char* fileName, bool toFile);

	char *dataBytes;
	u32 length;
	char *error;
};

