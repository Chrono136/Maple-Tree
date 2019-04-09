#pragma once
class UIMain : public nana::form
{
public:
	UIMain();
	~UIMain();

	static int Init();

	static void HideConsole();

	static void ShowConsole();

	static nana::form* mainForm;

	void OnFormDestroy();

	void OnToggleConsoleClick(const nana::arg_click& ei);

	void OnDownloadTitleClick(const nana::arg_click& ei);

	void OnDecryptContentClick(const nana::arg_click& ei);

	bool IsVisible = false;

	nana::rectangle btn_def_sz = nana::rectangle(20, 20, 125, 30);

	nana::inputbox *titleIdInputbox;
};

