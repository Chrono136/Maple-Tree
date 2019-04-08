namespace MapleSeed
{
#pragma once
	class MapleTicket
	{
	public:
		MapleTicket();
		~MapleTicket();

		static MapleTicket Create(TitleInfo * ti);
		static MapleTicket Create(std::string id);

		char* data;
		int len;

		TitleInfo * info;

	private:
		static const int TICKET_SIZE;
	};
}