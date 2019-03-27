#pragma once
class MapleTicket
{
public:
	MapleTicket();
	~MapleTicket();

	static MapleTicket Create(TitleInfo * ti);

	char* data;
	int len;

private:
	static const int TICKET_SIZE;
};

