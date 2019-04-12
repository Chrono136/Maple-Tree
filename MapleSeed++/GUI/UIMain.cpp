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
		MapleMain::DownloadContent(id.value());
		librarylist->at(0).append(TitleInfo::GetTitleInfo(id.value().c_str()));
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
		librarylist->clear();
		Library::ref->_db = std::move(*new vector<TitleInfo*>);

		for (nana::folderbox::path_type & p : paths)
		{
			messagelabel->caption("<center bold>" + Library::ref->BaseDirectory + "</>");

			Library::ref->SaveBaseDirectory(p.generic_string());
			Library::ref->Load();

			for (int i = 0; i < Library::ref->_db.size(); i++)
			{
				auto item = Library::ref->_db[i];
				librarylist->at(0).append(*item, true);
			}
		}
	}

	curpicker = nullptr;
}

void UIMain::OnUpdateContentClick(const nana::arg_click & ei)
{
	nana::inputbox input{ *this, "Input version", "Download Title Update" };

	inputbox::text vt("<bold>Title Version</>");
	input.min_width_entry_field(200);
	input.verify([&vt](window handle) { return true; });

	if (input.show_modal(vt))
	{
		for (auto item : librarylist->at(0))
		{
			if (item.selected())
			{
				auto & ti = item.value<TitleInfo>();
				string _id = string("0005000e") + string(ti.id).substr(8);
				MapleMain::DownloadContent(_id, vt.value());
			}
		}
	}
}

void UIMain::OnDlcContentClick(const nana::arg_click & ei)
{
	for (auto item : librarylist->at(0))
	{
		if (item.selected())
		{
			auto & itm = item.value<TitleInfo>();
			//MapleSeed::MapleMain::DownloadContent("0005000c" + string(itm.id).substr(8), "");
		}
	}
}

//Overload the operator<< for oresolver to resolve the person type
listbox::oresolver& operator<<(listbox::oresolver& ores, const TitleInfo& ti)
{
	return ores << ti.id << ti.name << ti.region;
}

void UIMain::UpdateCoverArt(const arg_listbox& ei)
{
	auto item = ei.item;
	if (!item.empty())
	{
		auto & itm = item.value<TitleInfo>();
		string cover = itm.GetCoverArt();

		nana::paint::image img_bg;
		if (img_bg.open(cover.c_str()))
		{
			auto image = new drawing{ *coverart };

			image->clear();
			image->draw([img_bg](nana::paint::graphics& graph) {img_bg.stretch(rectangle{ img_bg.size() }, graph, rectangle{ graph.size() }); });
			image->update();
		}
	}
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

#pragma region Buttons
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
		pool.push([&] {
			btn3.enabled(false);
			uim.OnSelectLibraryClick(ei);
			btn3.enabled(true);
		});
	});

	//design update content button
	nana::button btn4{ uim };
	btn4.caption("Download Update");
	btn4.events().click([&](const nana::arg_click& ei) {
		pool.push([&] {
			btn4.enabled(false);
			uim.OnUpdateContentClick(ei);
			btn4.enabled(true);
		});
	});

	//design update content button
	nana::button btn5{ uim };
	btn5.caption("Download DLC");
	btn5.events().click([&](const nana::arg_click& ei) {
		pool.push([&] {
			btn5.enabled(false);
			uim.OnDlcContentClick(ei);
			btn5.enabled(true);
		});
	});
#pragma endregion

	//design progress bar
	nana::progress pgbar{ uim };
	uim.progressbar = &pgbar;
	pgbar.fgcolor(colors::blue);
	
	//design cover art
	nana::picture pic{ uim };
	uim.coverart = &pic;
	uim.UpdateCoverArt(arg_listbox(NULL));

	//design title list
	nana::listbox list{ uim };
	list.events().selected([&](const arg_listbox& ei) {uim.UpdateCoverArt(ei); });
	uim.librarylist = &list;
	list.append_header("ID");
	list.append_header("Name");
	list.append_header("Region");
	for (int i = 0; i < Library::ref->_db.size(); i++)
		list.at(0).append(*Library::ref->_db[i], true);
	if (list.size_item(0) > 0)
		uim.UpdateCoverArt(arg_listbox(list.at(0).at(0)));

	//design directory label
	nana::label lbl{ uim };
	uim.messagelabel = &lbl;
	lbl.format(true);
	lbl.caption("<center bold>" + Library::ref->BaseDirectory + "</>");

	//set element placement
	nana::place plc_{ uim };
	plc_.div("vertical"
		"<weight=5% <><weight=90% gap=5 btns><>>"
		"<weight=85% arrange=[65%,35%] mid>"
		"<weight=5% pgbar>"
		"<weight=5% lbl>"
	);
	plc_["btns"] << btn0 << btn1 << btn2 << btn4 << btn3;
	plc_["mid"] << list << pic;
	plc_["pgbar"] << pgbar;
	plc_["lbl"] << lbl;
	plc_.collocate();

	uim.show();
	uim.IsVisible = true;

	nana::exec();
	return 0;
}
