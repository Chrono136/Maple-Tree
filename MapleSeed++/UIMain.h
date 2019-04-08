#pragma once
class UIMain
{
public:
	UIMain();
	~UIMain();

	void OnFormDestroy();

	void OnToggleConsoleClick(const nana::arg_click& ei);

	void OnDownloadTitleClick(const nana::arg_click& ei);

	void OnDecryptContentClick(const nana::arg_click& ei);

	static int Init();

	static void HideConsole();

	static void ShowConsole();

	static bool IsVisible;

	nana::form frm = form(API::make_center(848, 480));

	nana::rectangle btn_def_sz = nana::rectangle(20, 20, 125, 30);

	nana::inputbox *titleIdInputbox;
};

