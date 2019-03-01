#pragma once
class DownloadClient
{
public:
	DownloadClient(const char *url);
	~DownloadClient();

	char *dataBytes;
	long length;
	char *error;
	static void ev_handler(struct mg_connection *c, int ev, void *p);
};

