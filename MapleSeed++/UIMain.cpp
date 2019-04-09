#include "stdafx.h"
#include "UIMain.h"


nana::form* UIMain::mainForm;

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
	ThreadManager::stop_thread("GUIThread");
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
		MapleMain::DownloadContent("", id.value());
	}
}

void UIMain::OnDecryptContentClick(const nana::arg_click& ei)
{

}

int UIMain::Init()
{
	UIMain uim;

	//lock window size
	API::track_window_size(uim, uim.size(), false);
	API::track_window_size(uim, uim.size(), true);

	//set icon from resource
	wstring app_path(4096, '\0');
	app_path.resize(GetModuleFileNameW(0, &app_path.front(), (DWORD)app_path.size()));
	uim.icon(paint::image(app_path));

	//design main form
	uim.events().destroy([&]() { uim.OnFormDestroy(); });
	uim.caption("MapleSeed++ " + std::string(GEN_VERSION_STRING));
	uim.bgcolor(colors::white);

	//design toggle console button
	nana::button btn0{ uim, uim.btn_def_sz };
	btn0.caption("Toggle CLI");
	btn0.events().click([&](const nana::arg_click& ei) { uim.OnToggleConsoleClick(ei); });

	//design download button
	nana::button btn1{ uim, uim.btn_def_sz };
	btn1.caption("Download Title");
	btn1.events().click([&](const nana::arg_click& ei) { uim.OnDownloadTitleClick(ei); });

	//design decrypt button
	nana::button btn2{ uim, uim.btn_def_sz };
	btn2.caption("Decrypt Content");
	btn2.events().click([&](const nana::arg_click& ei) { uim.OnDecryptContentClick(ei); });

	//design progress bar
	nana::progress pgbar{ uim };
	

	//set element placement
	nana::place plc(uim);
	plc.div("<vert abc>");
	plc.field("abc") << btn0 << btn1 << btn2;
	plc.collocate();

	uim.show();
	uim.IsVisible = true;

	nana::exec();
	return 0;
}
