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

/*
Creates a ticket file used for decrypting content.
@param TitleInfo ti - Title info used to create a blank ticket
@return A populated ticket when valid TitleInfo is provided, otherwise returns a blank ticket
*/
MapleTicket MapleTicket::Create(TitleInfo * ti)
{
	if (ti && ti->id && ti->key)
	{
		MapleTicket ticket;
		char buf[TICKET_SIZE];

		//memcpy(&buf[0x1E6 * 2], ti->versions, 2);
		memcpy(&buf[0x2CC * 2], ti->id, 16);
		memcpy(&buf[0x1BF * 2], ti->key, 32);

		auto bytes = HexToBytes(buf);
		ticket.len = (int)bytes.size();
		memcpy(ticket.data, bytes.data(), bytes.size());

		ticket.info = ti;
		return ticket;
	}

	return MapleTicket();
}

/*
Creates a ticket file using the 'id' input.
@param id - Title ID
@return A populated ticket when valid TitleInfo is provided, otherwise returns a blank ticket
*/
MapleTicket MapleTicket::Create(std::string id)
{
	return Create(TitleInfo::GetTitleInfo(id.c_str()));
}
