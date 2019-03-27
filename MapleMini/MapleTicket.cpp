#include "stdafx.h"
#include "MapleTicket.h"


const int MapleTicket::TICKET_SIZE = 1500;

MapleTicket::MapleTicket()
{
	data = new char[TICKET_SIZE];
	len = TICKET_SIZE;
}


MapleTicket::~MapleTicket()
{
}

MapleTicket MapleTicket::Create(TitleInfo * ti)
{
	if (ti->id)
	{
		MapleTicket ticket;
		char buf[TICKET_SIZE];
		memset(buf, 1, TICKET_SIZE);
		
		memcpy(&buf[0x1BF * 2], ti->key, 32);
		memcpy(&buf[0x1E6 * 2], ti->versions, 2);
		memcpy(&buf[0x2CC * 2], ti->id, 16);

		auto bytes = HexToBytes(buf);
		ticket.len = (int)bytes.size();
		memcpy(ticket.data, bytes.data(), bytes.size());

		return ticket;
	}

	return MapleTicket();
}
