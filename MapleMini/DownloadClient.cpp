#include "stdafx.h"
#include "DownloadClient.h"


static int s_exit_flag = 0;
static const char *s_show_headers_opt = "--show-headers";

DownloadClient::DownloadClient(const char *url)
{
	struct mg_mgr mgr;
	struct mg_connection *nc;

	mg_mgr_init(&mgr, NULL);
	nc = mg_connect_http(&mgr, ev_handler, url, NULL, NULL);
	nc->user_data = this;

	while (s_exit_flag == 0) {
		mg_mgr_poll(&mgr, 1000);
	}
	mg_mgr_free(&mgr);
}

DownloadClient::~DownloadClient()
{
}

void DownloadClient::ev_handler(struct mg_connection *nc, int ev, void *ev_data)
{
	DownloadClient *dc = (DownloadClient *) nc->user_data;
	struct http_message *hm = (struct http_message *) ev_data;

	switch (ev) {
	case MG_EV_CONNECT:
		if (*(int *)ev_data != 0) {
			fprintf(stderr, "connect() failed: %s\n", strerror(*(int *)ev_data));
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
