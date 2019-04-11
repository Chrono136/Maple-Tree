#pragma once
class UIMain : public nana::form
{
public:
	UIMain();
	~UIMain();

	void OnFormDestroy();

	void OnToggleConsoleClick(const nana::arg_click& ei);
	void OnDownloadTitleClick(const nana::arg_click& ei);
	void OnDecryptContentClick(const nana::arg_click& ei);
	void OnSelectLibraryClick(const nana::arg_click& ei);
	void OnUpdateContentClick(const nana::arg_click& ei);
	void OnDlcContentClick(const nana::arg_click& ei);

	void UpdateCoverArt(const arg_listbox&);

	bool IsVisible = false;

	nana::folderbox *curpicker;
	nana::listbox *librarylist;
	nana::progress *progressbar;
	nana::label *messagelabel;
	nana::picture *coverart;

	static void ProgressUpdateCallback(unsigned long min, unsigned long max, const char *data)
	{
		if (UIMain::mainForm && UIMain::mainForm->progressbar)
		{
			UIMain::mainForm->progressbar->amount(max);
			UIMain::mainForm->progressbar->value(min);
			UIMain::mainForm->messagelabel->caption(data);
		}
	}

	static int Init();

	static void HideConsole();

	static void ShowConsole();

	static UIMain* mainForm;
};

