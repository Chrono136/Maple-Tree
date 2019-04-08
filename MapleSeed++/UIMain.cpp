#include "stdafx.h"
#include "UIMain.h"


bool UIMain::IsVisible = false;

UIMain::UIMain() {}

UIMain::~UIMain() {}

void HideConsole()
{
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);
}

void ShowConsole()
{
	::ShowWindow(::GetConsoleWindow(), SW_SHOW);
}

bool IsConsoleVisible()
{
	return ::IsWindowVisible(::GetConsoleWindow()) != FALSE;
}

void on_toggle_cli_click(const nana::arg_click& ei)
{
	if (IsConsoleVisible()) {
		HideConsole();
	}
	else {
		ShowConsole();
	}
}

void on_form_destory()
{
	ShowConsole();
	UIMain::IsVisible = false;
	ThreadManager::stop_thread("GUIThread");
}

int UIMain::Init()
{
	const nana::size WINDOW_SIZE = { 1280, 720 };

	form fm;
	fm.events().destroy([&]() { on_form_destory(); });
	fm.caption("MapleSeed++ " + std::string(GEN_VERSION_STRING));
	fm.size(WINDOW_SIZE);
	API::track_window_size(fm, WINDOW_SIZE, false);
	API::track_window_size(fm, WINDOW_SIZE, true);
	fm.bgcolor(colors::white);
	fm.move(0, 5);

	//design button
	element::bground bground;
	button btn(fm, nana::rectangle(20, 20, 125, 30));
	btn.caption("Toggle CLI");
	btn.set_bground(bground);
	btn.events().click([&](const nana::arg_click& ei) { on_toggle_cli_click(ei); });

	fm.show();
	UIMain::IsVisible = true;

	nana::exec();
	return 0;
}
