#include "stdafx.h"
#include "UIMain.h"


UIMain* UIMain::mainForm;

UIMain::UIMain() :
	form(API::make_center(848, 480))
{
	mainForm = this;
}

UIMain::~UIMain()
{
	mainForm = nullptr;
}

void UIMain::HideConsole()
{
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);
}

void UIMain::ShowConsole()
{
	::ShowWindow(::GetConsoleWindow(), SW_SHOW);
}

bool IsConsoleVisible()
{
	return ::IsWindowVisible(::GetConsoleWindow()) != FALSE;
}

void UIMain::OnFormDestroy()
{
	ShowConsole();
	UIMain::IsVisible = false;
	ThreadManager::StopThread("GUIThread");
}

void UIMain::OnToggleConsoleClick(const nana::arg_click& ei)
{
	if (IsConsoleVisible()) {
		HideConsole();
	}
	else {
		ShowConsole();
	}
}

void UIMain::OnDownloadTitleClick(const nana::arg_click& ei)
{
	nana::inputbox id_inputbox_{ *this, "Input id of the desired title", "Download Title" };

	inputbox::text id("<bold>Title ID</>");
	id_inputbox_.min_width_entry_field(200);
	id_inputbox_.verify([&id](window handle)
	{
		if (id.value().empty() || id.value().length() != 16)
		{
			msgbox mb(handle, "Invalid title id");
			mb << L"Title ID should be at least 16 characters";
			mb.show();
			return false; //verification failed
		}
		return true; //verification passed
	});

	if (id_inputbox_.show_modal(id))
	{
		auto ti = TitleInfo::GetTitleInfo(id.value().c_str());
		ti->Download();
		ti->Decrypt();
		librarylist->at(0).append({ ti->id, ti->name, ti->region });
	}
}

void UIMain::OnDecryptContentClick(const nana::arg_click& ei)
{
	if (curpicker) return;

	nana::folderbox picker;
	curpicker = &picker;

	picker.title("WiiU Games Content Decryptor");
	//picker.allow_multi_select(true);

	auto paths = picker.show();
	if (!paths.empty())
	{
		for (nana::folderbox::path_type & p : paths)
		{
			MapleMain::DecryptContent(p.generic_string(), Library::GetBaseDirectory());
		}
	}

	curpicker = nullptr;
}

void UIMain::OnSelectLibraryClick(const nana::arg_click& ei)
{
	if (curpicker) return;

	nana::folderbox picker;
	curpicker = &picker;

	picker.title("WiiU Games Directory");

	auto paths = picker.show();
	if (!paths.empty())
	{
		for (nana::folderbox::path_type & p : paths)
		{
			messagelabel->caption("<center bold>" + Library::ref->BaseDirectory + "</>");

			Library::ref->SaveBaseDirectory(p.generic_string());
			Library::ref->Load();

			librarylist->clear();

			for (int i = 0; i < Library::ref->_db.size(); i++)
			{
				auto item = Library::ref->_db[i];
				librarylist->at(0).append({ item->id, item->name, item->region });
			}
		}
	}

	curpicker = nullptr;
}

void UIMain::UpdateCoverArt(string code)
{
	auto temp = std::filesystem::temp_directory_path().generic_string() + string(code + ".bmp");

	if (!FileExists(temp))
	{
		string url = string("https://art.gametdb.com/wiiu/coverM/US/") + code + string(".jpg");

		DownloadClient().DownloadFile(url.c_str(), temp.c_str());
	}

	nana::paint::image img_bg;
	img_bg.open(temp.c_str());
	drawing{ *coverart }.draw([img_bg](nana::paint::graphics& graph) {
		img_bg.stretch(rectangle{ img_bg.size() }, graph, rectangle{ graph.size() });
	});
}

int UIMain::Init()
{
	UIMain uim;
	nana::threads::pool pool;

	//design main form
	uim.events().destroy([&]() { uim.OnFormDestroy(); });
	uim.caption("MapleSeed++ " + std::string(GEN_VERSION_STRING));
	uim.bgcolor(colors::white);

	//lock window size
	API::track_window_size(uim, uim.size(), false);
	//API::track_window_size(uim, uim.size(), true);

	//set icon from resource
	wstring app_path(4096, '\0');
	app_path.resize(GetModuleFileNameW(0, &app_path.front(), (DWORD)app_path.size()));
	uim.icon(paint::image(app_path));

	//design toggle console button
	nana::button btn0{ uim };
	btn0.caption("Toggle CLI");
	btn0.events().click([&](const nana::arg_click& ei) { uim.OnToggleConsoleClick(ei); });

	//design download button
	nana::button btn1{ uim };
	btn1.caption("Download Title");
	btn1.events().click([&](const nana::arg_click& ei) { 
		pool.push([&] {
			btn1.enabled(false);
			uim.OnDownloadTitleClick(ei);
			btn1.enabled(true);
		});
	});

	//design decrypt button
	nana::button btn2{ uim };
	btn2.caption("Decrypt Content");
	btn2.events().click([&](const nana::arg_click& ei) { 
		pool.push([&] {
			btn2.enabled(false);
			uim.OnDecryptContentClick(ei);
			btn2.enabled(true);
		});
	});

	//design update library button
	nana::button btn3{ uim };
	btn3.caption("Select Library");
	btn3.events().click([&](const nana::arg_click& ei) {
		btn3.enabled(false);
		uim.OnSelectLibraryClick(ei);
		btn3.enabled(true);
	});

	//design progress bar
	nana::progress pgbar{ uim };
	uim.progressbar = &pgbar;
	pgbar.fgcolor(colors::blue);
	
	//design cover art
	nana::picture pic{ uim };
	uim.coverart = &pic;
	auto temp = std::filesystem::temp_directory_path().generic_string() + string("msca.bmp");
	if (!FileExists(temp))
		DownloadClient().DownloadFile("http://pixxy.in/default.bmp", temp.c_str());
	nana::paint::image img_bg;
	img_bg.open(temp.c_str());
	drawing{ pic }.draw([img_bg](nana::paint::graphics& graph) {
		img_bg.stretch(rectangle{ img_bg.size() }, graph, rectangle{ graph.size() });
	});

	//design title list
	nana::listbox list{ uim };
	//list.events().selected([&] {uim.UpdateCoverArt("AQUE01"); });
	uim.librarylist = &list;
	list.append_header("ID");
	list.append_header("Name");
	list.append_header("Region");
	for (int i = 0; i < Library::ref->_db.size(); i++)
	{
		auto item = Library::ref->_db[i];
		list.at(0).append({ item->id, item->name, item->region });
	}

	//design directory label
	nana::label lbl{ uim };
	uim.messagelabel = &lbl;
	lbl.format(true);
	lbl.caption("<center bold>" + Library::ref->BaseDirectory + "</>");

	//set element placement
	nana::place plc_{ uim };
	plc_.div("vertical"
		"<weight=5% <><weight=500 gap=5 btns><>>"
		"<weight=85% arrange=[60%,40%] mid>"
		"<weight=5% pgbar>"
		"<weight=5% lbl>"
	);
	plc_["btns"] << btn0 << btn1 << btn2 << btn3;
	plc_["mid"] << list << pic;
	plc_["pgbar"] << pgbar;
	plc_["lbl"] << lbl;
	plc_.collocate();

	uim.show();
	uim.IsVisible = true;

	nana::exec();
	return 0;
}
