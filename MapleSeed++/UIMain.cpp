#include "stdafx.h"
#include "UIMain.h"


bool UIMain::IsVisible = false;

UIMain::UIMain() 
{
	//lock window size
	API::track_window_size(frm, frm.size(), false);
	API::track_window_size(frm, frm.size(), true);
}

UIMain::~UIMain()
{
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
	if (!titleIdInputbox)
	{
		titleIdInputbox = new inputbox(frm, "Input id of the desired title", "Download Title");

		inputbox::text id("<bold>Title ID</>");
		titleIdInputbox->min_width_entry_field(200);
		titleIdInputbox->verify([&id](window handle)
		{
			if (id.value().empty())
			{
				msgbox mb(handle, "Invalid input");
				mb << L"Title ID should not be empty";
				mb.show();
				return false; //verification failed
			}
			return true; //verification passed
		});

		if (titleIdInputbox->show_modal(id))
		{
			MapleMain::DownloadContent("", id.value());
		}

		titleIdInputbox = nullptr;
	}
}

void UIMain::OnDecryptContentClick(const nana::arg_click& ei)
{

}

int UIMain::Init()
{
	UIMain ui;

	//set icon from resource
	wstring app_path(4096, '\0');
	app_path.resize(GetModuleFileNameW(0, &app_path.front(), (DWORD)app_path.size()));
	ui.frm.icon(paint::image(app_path));

	//design main form
	ui.frm.events().destroy([&]() { ui.OnFormDestroy(); });
	ui.frm.caption("MapleSeed++ " + std::string(GEN_VERSION_STRING));
	ui.frm.bgcolor(colors::white);

	//design toggle console button
	button btn0(ui.frm, ui.btn_def_sz);
	btn0.caption("Toggle CLI");
	btn0.events().click([&](const nana::arg_click& ei) { ui.OnToggleConsoleClick(ei); });

	//design download button
	button btn1(ui.frm, ui.btn_def_sz);
	btn1.caption("Download Title");
	btn1.events().click([&](const nana::arg_click& ei) { ui.OnDownloadTitleClick(ei); });

	//design decrypt button
	button btn2(ui.frm, ui.btn_def_sz);
	btn2.caption("Decrypt Content");
	btn2.events().click([&](const nana::arg_click& ei) { ui.OnDecryptContentClick(ei); });

	//set element placement
	nana::place plc(ui.frm);
	plc.div("<vert abc>");
	plc.field("abc") << btn0 << btn1 << btn2;
	plc.collocate();

	ui.frm.show();
	UIMain::IsVisible = true;

	nana::exec();
	return 0;
}
