#include "stdafx.h"
#include "DownloadClient.h"

struct ComInit
{
	HRESULT hr;
	ComInit() : hr(::CoInitialize(nullptr)) {}
	~ComInit() { if (SUCCEEDED(hr)) ::CoUninitialize(); }
};
static int s_exit_flag = 0;

DownloadClient::DownloadClient(const char *url)
{
	ComInit init;

	// use CComPtr so you don't have to manually call Release()
	CComPtr<IStream> pStream;

	wchar_t* wString = new wchar_t[4096];
	MultiByteToWideChar(CP_ACP, 0, url, -1, wString, 4096);

	// Open the HTTP request.
	HRESULT hr = URLOpenBlockingStreamW(nullptr, wString, &pStream, 0, nullptr);
	if (FAILED(hr))
	{
		std::cout << "ERROR: Could not connect. HRESULT: 0x" << std::hex << hr << std::dec << "\n";
		return;
	}

	string buffer;
	do
	{
		char* _dataBytes = new char[2048];
		hr = pStream->Read(_dataBytes, sizeof(_dataBytes), (ULONG*) &length);
		buffer.append(_dataBytes, length);
	} while (SUCCEEDED(hr) && hr != S_FALSE);

	if (FAILED(hr))
	{
		std::cout << "ERROR: Download failed. HRESULT: 0x" << std::hex << hr << std::dec << "\n";
		return;
	}
	else
	{
		length = buffer.length();
		dataBytes = new char[length];
		memcpy(dataBytes, buffer.c_str(), length);
	}
	
	/*
	struct mg_mgr mgr;
	struct mg_connection *nc;

	mg_mgr_init(&mgr, NULL);
	nc = mg_connect_http(&mgr, ev_handler, url, "User-Agent: wii libnup/1.1\r\n", NULL);
	nc->user_data = this;

	while (s_exit_flag == 0) {
		mg_mgr_poll(&mgr, 1000);
	}
	mg_mgr_free(&mgr);
	*/
}

DownloadClient::~DownloadClient()
{
}

void DownloadClient::ev_handler(struct mg_connection *nc, int ev, void *ev_data)
{
	DownloadClient *dc = (DownloadClient *) nc->user_data;
	struct http_message *hm = (struct http_message *) ev_data;

	auto rbuf = nc->recv_mbuf.buf;
	auto sbuf = nc->send_mbuf.buf;

	switch (ev) {
	case MG_EV_CONNECT:
		if (*(int *)ev_data != 0) {
			fprintf(stderr, "connect() failed: %s\n", dc->error = strerror(*(int *)ev_data));
			s_exit_flag = 1;
		}
		break;
	case MG_EV_HTTP_REPLY:
		nc->flags |= MG_F_CLOSE_IMMEDIATELY;

		if (dc != nullptr)
		{
			dc->length = hm->body.len;
			dc->dataBytes = new char[dc->length];
			memcpy(dc->dataBytes, hm->body.p, hm->body.len);
			dc->dataBytes[dc->length] = '\0';
		}
		else
		{
			Toolbelt::WriteLineRed(" Error: Unable to save downloaded data!");
		}

		putchar('\n');
		s_exit_flag = 1;
		break;
	case MG_EV_CLOSE:
		if (s_exit_flag == 0) {
			printf("Server closed connection\n");
			s_exit_flag = 1;
		}
		break;
	default:
		break;
	}
}
