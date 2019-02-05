#pragma once
class DownloadClient
{
public:
	DownloadClient(const char *url);
	~DownloadClient();

	int downloadContent(void* tmd, char* outputDir, char* titleURL);

	char *dataBytes;
	size_t length;
	static void ev_handler(struct mg_connection *c, int ev, void *p);
};

