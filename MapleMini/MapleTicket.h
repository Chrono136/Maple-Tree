#pragma once
class MapleTicket
{
public:
	MapleTicket();
	~MapleTicket();

	static MapleTicket Create(TitleInfo * ti);

	int length;
	char* ticket;

private:
	static const int TK = 0x140;


};

