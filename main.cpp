#include <river\defines.h>
#include <river\launch.h>
#include <river\data.h>
#include <river\download.h>
#include <river\accounts.h>
#include <river\ui.h>
#include <SDKDDKVer.h>
#include <thread>

int btn1_i = 0;

int main() {

	RVG_START;
	initData();
	mkdir("RvL");

	HRSRC hRsrc = FindResourceA(NULL, MAKEINTRESOURCEA(IDR_JAVACLASS1), "javaclass");
	HGLOBAL IDR = LoadResource(NULL, hRsrc);
	DWORD size = SizeofResource(NULL, hRsrc);
	FILE* javaClass = fopen("RvL\\getJavaMainVersion.class", "wb");
	fwrite(LockResource(IDR), sizeof(char), size, javaClass);
	fclose(javaClass);
	FreeResource(IDR);

	hRsrc = FindResource(NULL, MAKEINTRESOURCE(IDR_LANG_CHINESE), L"lang");
	IDR = LoadResource(NULL, hRsrc);
	size = SizeofResource(NULL, hRsrc);
	reader.parse((const char*)LockResource(IDR), RvG::lang);
	FreeResource(IDR);


	RegGetValueA(hData, NULL, "SelectedLang", RRF_RT_REG_DWORD, NULL, &intLangSel, &sz);
	hRsrc = FindResource(NULL, MAKEINTRESOURCE(langs[intLangSel]["num"].asInt()), L"lang");
	IDR = LoadResource(NULL, hRsrc);
	size = SizeofResource(NULL, hRsrc);
	reader.parse((const char*)LockResource(IDR), RvG::lang);
	FreeResource(IDR);


	hRsrc = FindResource(NULL, MAKEINTRESOURCE(IDR_FONT1), L"font");
	IDR = LoadResource(NULL, hRsrc);
	size = SizeofResource(NULL, hRsrc);
	DWORD nfi;
	hFontHandle = AddFontMemResourceEx(LockResource(IDR), size, 0, &nfi);
	hFont = CreateFontA(14, 0, 0, 0, FW_REGULAR, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, "Unifont");
	FreeResource(IDR);


	// Set-up Pages

	x = new RvG::Window("name.launcher", 0);
	x->setFont(hFont);
	x->setTranslate("name.launcher", RvG::lang);
	x->onClose([](HWND win, HWND btn) -> int {
		DeleteObject(hFont);
		CloseHandle(hWrite);
		CloseHandle(hRead);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		return 0;
		});
	
	SendMessage(*x, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(RvG::_hInstance, MAKEINTRESOURCE(IDI_ICON2)));
	SendMessage(*x, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(RvG::_hInstance, MAKEINTRESOURCE(IDI_ICON1)));
	
	pgbProgress = new RvG::ProgressBar(170, 0, 600, 25, x);
	
	btnLaunch = new RvG::Button("do.launch", 0, 0, 150, 150, x);
	btnLaunch->bindCommand([](HWND win, HWND btn)->int {
		if (lisMinecraftVersion == nullptr) {
			MessageBoxA(win, doTranslate("prompt.mcje.notvalid"), doTranslate("error", 1), MB_OK | MB_ICONERROR);
			return 0;
		}
		GetWindowTextA(*ediSettingsDir, newStr, 256);
		lisMinecraftVersion->getText(intMinecraftSel, baseStr);
		DWORD rec = 0;
		if (GetHandleInformation(pi.hProcess, &rec) != 0) {
			MessageBoxA(win, doTranslate("prompt.mcje.already"), doTranslate("error", 1), MB_OK | MB_ICONERROR);
		}
		else {
			thread thr([win]() {
				int n = launchInstance(baseStr, newStr, x);
				switch (n) {
				case 1: {
					MessageBoxA(win, doTranslate("prompt.mcje.error"), doTranslate("error", 1), MB_OK | MB_ICONERROR);
					break;
				}
				}
				});
			thr.detach();
		}
		return 0;
		});
	btnLaunch->setTranslate("do.launch", RvG::lang);

	swiMinecraft = new RvG::Button("swi.mcje", 0, 175, 125, 50, x);
	swiMinecraft->setTranslate("swi.mcje", RvG::lang);
	
	swiAccounts = new RvG::Button("swi.accounts", 0, 225, 125, 50, x);
	swiAccounts->setTranslate("swi.accounts", RvG::lang);
	
	swiSettings = new RvG::Button("swi.settings", 0, 275, 125, 50, x);
	swiSettings->setTranslate("swi.settings", RvG::lang);
	
	swiMods = new RvG::Button("swi.mods", 0, 325, 125, 50, x);
	swiMods->setTranslate("swi.mods", RvG::lang);

	swiMinecraftBE = new RvG::Button("swi.mcbe", 0, 375, 125, 50, x);
	swiMinecraftBE->setTranslate("swi.mcbe", RvG::lang);
	
	swiLang = new RvG::Button("swi.lang", 0, 425, 125, 50, x);
	swiLang->setTranslate("swi.lang", RvG::lang);




	// Page Minecraft

	pageMinecraft = new RvG::Container(170, 25, 600, 400, x);

	lisMinecraftVersion = new RvG::ListBox(0, 0, 300, 400, pageMinecraft, WS_VSCROLL);
	labMinecraftVersionPrompt = new RvG::Label("prompt.mcje.notvalid", 0, 0, 300, 400, pageMinecraft);
	labMinecraftVersionPrompt->setTranslate("prompt.mcje.notvalid", RvG::lang);

	btnMinecraftAdd = new RvG::Button("do.mcje.add", 300, 0, 160, 100, pageMinecraft);
	btnMinecraftAdd->setTranslate("unfinished", RvG::lang);
	btnMinecraftAdd->bindCommand([](HWND win, HWND btn)->int {
		curPage->hide();
		if (hasPage2) {
			curPage2->hide();
			hasPage2 = 0;
		}
		hasPage2 = 1;
		curPage2 = pageMinecraftDownloads;
		curPage2->show();
		if (versionManifest.isArray()) {
			if (versionManifest.size() != 0) {
				lisMinecraftDownloads->show();
				labMinecraftDownloadsPrompt->hide();
			}
			else {
				labMinecraftDownloadsPrompt->show();
				lisMinecraftDownloads->hide();
			}
		}
		else {
			labMinecraftDownloadsPrompt->setTranslate("prompt.mcje.download.unable.manifest", RvG::lang);
			labMinecraftDownloadsPrompt->show();
			lisMinecraftDownloads->hide();
		}
		return 0;
		});

	btnMinecraftRemove = new RvG::Button("do.mcje.remove", 300, 100, 160, 100, pageMinecraft);
	btnMinecraftRemove->setTranslate("do.mcje.remove", RvG::lang);
	btnMinecraftRemove->bindCommand([](HWND win, HWND btn) {
		thread thr([]() {
			int ind = lisMinecraftVersion->getSelIndex();
			if (ind == -1) return 0;
			RvG::Window* dialog = new RvG::Window(doTranslate("dialog"), 1, CW_USEDEFAULT, CW_USEDEFAULT, 750, 250);
			dialog->setFont(hFont);
			char temp[256] = {};
			char t2[256];
			lisMinecraftVersion->getText(ind, t2);
			strcpyf(temp, doTranslate("prompt.mcje.remove"), t2);
			new RvG::Label(temp, 10, 10, 512, 20, dialog);
			RvG::Button* btnDialogOK = new RvG::Button(doTranslate("sel.yes"), 516, 157, 100, 40, dialog);
			RvG::Button* btnDialogCan = new RvG::Button(doTranslate("sel.no"), 622, 157, 100, 40, dialog);
			btnDialogCan->bindCommand([](HWND win, HWND btn) {
				DestroyWindow(win);
				return 0;
				});
			btnDialogOK->bindCommand([](HWND win, HWND btn)->int {
				char temp[1026];
				char t2[256];
				lisMinecraftVersion->getText(lisMinecraftVersion->getSelIndex(), t2);
				struct _stat fileStat;
				
				SendMessageA(*lisMinecraftVersion, LB_RESETCONTENT, 0, 0);
				if ((_stat(temp, &fileStat) == 0) && (fileStat.st_mode & _S_IFDIR)) {
					sz = 1024;
					RegGetValueA(hData, NULL, "MinecraftDirectory", RRF_RT_REG_SZ, NULL, temp, &sz);
					strcatf(temp, "\\versions\\%s", t2);
					filesystem::remove_all(temp);

					sz = 1024;
					RegGetValueA(hData, NULL, "MinecraftDirectory", RRF_RT_REG_SZ, NULL, temp, &sz);
					strcat(temp, "\\versions");
					int sz2 = 0;
					for (auto& v : std::filesystem::directory_iterator::directory_iterator(temp)) {
						std::string fileName = v.path().filename().string();
						const char* A = fileName.c_str();
						lisMinecraftVersion->add(A);
						sz2++;
					}
					int t = intMinecraftSel;
					if (t >= sz2) t = sz2 - 1;
					if (t < 0) t = 0;
					lisMinecraftVersion->setSelIndex(t);
					intMinecraftSel = t;
					labMinecraftVersionPrompt->hide();
				}
				else {
					lisMinecraftVersion->hide();
				}

				DestroyWindow(win);
				return 0;
				});
			SetFocus(*dialog);
			dialog->keepResponding();
			});
		thr.detach();
		return 0;
		});

	btnMinecraftOpen = new RvG::Button("do.open", 300, 200, 160, 100, pageMinecraft);
	btnMinecraftOpen->setTranslate("do.open", RvG::lang);
	btnMinecraftOpen->bindCommand([](HWND win, HWND btn)->int {
		struct _stat fileStat;
		char temp[514];
		char t2[514];
		sz = 512;
		RegGetValueA(hData, NULL, "MinecraftDirectory", RRF_RT_REG_SZ, NULL, temp, &sz);
		strcat(temp, "\\versions");

		if ((_stat(temp, &fileStat) == 0) && (fileStat.st_mode & _S_IFDIR)) {

			int ind = lisMinecraftVersion->getSelIndex();
			if (ind != -1) lisMinecraftVersion->getText(ind, t2);
			strcatf(temp, "\\%s", t2);

			HANDLE hRead, hWrite;
			STARTUPINFOA si;
			PROCESS_INFORMATION pi;
			SECURITY_ATTRIBUTES sa;
			sa.nLength = sizeof(SECURITY_ATTRIBUTES);
			sa.lpSecurityDescriptor = NULL;
			sa.bInheritHandle = TRUE;

			if (!CreatePipe(&hRead, &hWrite, &sa, 0)) {
				DWORD ret = GetLastError();
				return ret ? ret : -1;
			}

			ZeroMemory(&si, sizeof(STARTUPINFO));

			si.cb = sizeof(STARTUPINFO);
			GetStartupInfoA(&si);
			si.hStdError = hWrite;
			si.hStdOutput = hWrite;
			si.wShowWindow = SW_HIDE;
			si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
			char tmpS[1026];
			strcpyf(tmpS, "cmd /c explorer %s", temp);
			if (!CreateProcessA(NULL, tmpS, NULL, NULL, TRUE, NULL,
				NULL, NULL, &si, &pi)) {
				DWORD ret = GetLastError();
				CloseHandle(hRead);
				CloseHandle(hWrite);
				return ret ? ret : -1;
			}

			CloseHandle(hWrite);
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
			char buf[4098];
			DWORD bytesRead;
		}

		return 0;
		});

	struct _stat fileStat;
	char temp[1026];
	sz = 1024;
	RegGetValueA(hData, NULL, "MinecraftDirectory", RRF_RT_REG_SZ, NULL, temp, &sz);
	strcat(temp, "\\versions");

	if ((_stat(temp, &fileStat) == 0) && (fileStat.st_mode & _S_IFDIR)) {
		for (auto& v : std::filesystem::directory_iterator::directory_iterator(temp)) {
			std::string fileName = v.path().filename().string();
			const char* A = fileName.c_str();
			lisMinecraftVersion->add(A);
		}
		int t = 0; sz = 4;
		RegGetValueA(hData, NULL, "SelectedLaunch", RRF_RT_REG_DWORD, NULL, &t, &sz);
		lisMinecraftVersion->setSelIndex(t);
		labMinecraftVersionPrompt->hide();
	}
	else {
		lisMinecraftVersion->hide();
	}




	// Page Minecraft Downloads

	pageMinecraftDownloads = new RvG::Container(170, 25, 600, 400, x);

	lisMinecraftDownloads = new RvG::ListBox(0, 0, 300, 400, pageMinecraftDownloads, WS_VSCROLL);
	labMinecraftDownloadsPrompt = new RvG::Label("prompt.mcje.download.getting", 0, 0, 300, 400, pageMinecraftDownloads);
	labMinecraftDownloadsPrompt->setTranslate("prompt.mcje.download.getting", RvG::lang);
	
	btnMinecraftDownloads = new RvG::Button("do.mcje.download", 300, 0, 160, 100, pageMinecraftDownloads);
	btnMinecraftDownloads->setTranslate("do.mcje.download", RvG::lang);
	btnMinecraftDownloads->bindCommand(download);



	// Page Accounts

	pageAccounts = new RvG::Container(170, 25, 600, 400, x);
	
	lisAccountsList = new RvG::ListBox(0, 0, 300, 400, pageAccounts, WS_VSCROLL);
	labAccountsPrompt = new RvG::Label("prompt.accounts.no", 0, 0, 300, 400, pageAccounts);
	labAccountsPrompt->setTranslate("prompt.accounts.no", RvG::lang);

	btnAccountsRegister = new RvG::Button("do.accounts.add", 300, 0, 160, 100, pageAccounts);
	btnAccountsRegister->setTranslate("do.accounts.add", RvG::lang);
	btnAccountsRegister->bindCommand(addAcc);
	
	btnAccountsRemove = new RvG::Button("do.accounts.remove", 300, 100, 160, 100, pageAccounts);
	btnAccountsRemove->setTranslate("do.accounts.remove", RvG::lang);
	btnAccountsRemove->bindCommand(remAcc);
	
	char tempA[1026] = {};
	initAccounts();
	if (accounts.size() != 0){
		for (Json::Value i : accounts) {
			strcpy(tempA, i["userName"].asCString());
			if (i["userType"].asInt() == 0) strcat(tempA, doTranslate("type.accounts.legacy"));
			if (i["userType"].asInt() == 1) strcat(tempA, doTranslate("type.accounts.mojang"));
			lisAccountsList->add(tempA);
		}
		lisAccountsList->setSelIndex(intAccountsSel);
		lisAccountsList->show();
		labAccountsPrompt->hide();
	}
	else {
		labAccountsPrompt->show();
		lisAccountsList->hide();
	}



	// Page Settings
	
	pageSettings = new RvG::Container(170, 25, 600, 400, x);

	new RvG::Label("x", 200, 35, 15, 20, pageSettings, SS_CENTER);
	RvG::ParentWidget* t;
	t = new RvG::Label("value.settings.dir", 3, 5, 125, 20, pageSettings);
	t -> setTranslate("value.settings.dir", RvG::lang);
	t = new RvG::Label("value.settings.size", 3, 35, 200, 20, pageSettings);
	t -> setTranslate("value.settings.size", RvG::lang);

	sz = 1024;
	RegGetValueA(hData, NULL, "MinecraftDirectory", RRF_RT_REG_SZ, NULL, tempA, &sz);
	ediSettingsDir = new RvG::InputBox(tempA, 150, 0, 300, 25, pageSettings);
	
	sz = 4;
	RegGetValueA(hData, NULL, "WindowWidth", RRF_RT_REG_DWORD, NULL, &intSettingsWid, &sz);
	ediSettingsWid = new RvG::InputBox(_itoa(intSettingsWid, tempA, 10), 150, 30, 50, 25, pageSettings);

	RegGetValueA(hData, NULL, "WindowHeight", RRF_RT_REG_DWORD, NULL, &intSettingsHei, &sz);
	ediSettingsHei = new RvG::InputBox(_itoa(intSettingsHei, tempA, 10), 215, 30, 50, 25, pageSettings);



	// Page Mods

	pageMods = new RvG::Container(170, 25, 600, 400, x);

	lisMods = new RvG::ListBox(0, 0, 300, 400, pageMods, WS_VSCROLL);
	labModsPrompt = new RvG::Label("prompt.mods.no", 0, 0, 300, 400, pageMods, WS_VSCROLL);
	labModsPrompt->setTranslate("prompt.mods.no", RvG::lang);

	btnModsSwitch = new RvG::Button("do.mods.switch", 300, 0, 160, 100, pageMods);
	btnModsSwitch->setTranslate("do.mods.switch", RvG::lang);
	btnModsSwitch->bindCommand([](HWND win, HWND btn)->int {
		int ind = lisMods->getSelIndex();
		if (ind == -1) return 0;
		int i = 0;
		sz = 1024;
		char temp[1026];
		RegGetValueA(hData, NULL, "MinecraftDirectory", RRF_RT_REG_SZ, NULL, temp, &sz);
		strcat(temp, "\\mods");
		for (auto& v : std::filesystem::directory_iterator::directory_iterator(temp)) {
			if (v.is_directory()) continue;
			if (i != ind) {
				i++;
				continue;
			}
			std::string fileName = v.path().filename().string();
			char A[512] = "";
			strcpyf(A, "%s\\%s", temp, fileName.c_str());
			if (fileName.ends_with(".disabled")) {
				A[strlen(A)-9] = 0;
			}
			else {
				strcat(A, ".disabled");
			}
			char B[512] = "";
			strcpyf(B, "%s\\%s", temp, fileName.c_str());
			rename(B, A);
			break;
		}

		struct _stat fileStat;
		sz = 1024;
		RegGetValueA(hData, NULL, "MinecraftDirectory", RRF_RT_REG_SZ, NULL, temp, &sz);
		strcat(temp, "\\mods");

		SendMessage(*lisMods, LB_RESETCONTENT, 0, 0);
		if ((_stat(temp, &fileStat) == 0) && (fileStat.st_mode & _S_IFDIR)) {
			for (auto& v : std::filesystem::directory_iterator::directory_iterator(temp)) {
				if (v.is_directory()) continue;
				std::string fileName = v.path().filename().string();
				char A[512] = "";
				if (fileName.ends_with(".disabled")) {
					strcpy(A, doTranslate("type.mods.disabled"));
				}
				else {
					strcpy(A, doTranslate("type.mods.enabled"));
				}
				const int tempInt = strlen(A);
				strcat(A, fileName.c_str());
				int n = find(A + tempInt, ".disabled");
				if (n != -1) A[n + tempInt] = 0;
				lisMods->add(A);
			}
			labModsPrompt->hide();
		}
		else {
			lisMods->hide();
		}
		lisMods->setSelIndex(ind);

		return 0;
		});

	btnModsEnable = new RvG::Button("do.mods.enable", 300, 100, 160, 100, pageMods);
	btnModsEnable->setTranslate("do.mods.enable", RvG::lang);
	btnModsEnable->bindCommand([](HWND win, HWND btn)->int {
		sz = 1024;
		char temp[1026];

		struct _stat fileStat;
		sz = 1024;
		RegGetValueA(hData, NULL, "MinecraftDirectory", RRF_RT_REG_SZ, NULL, temp, &sz);
		strcat(temp, "\\mods");

		if ((_stat(temp, &fileStat) == 0) && (fileStat.st_mode & _S_IFDIR)) {
			for (auto& v : std::filesystem::directory_iterator::directory_iterator(temp)) {
				if (v.is_directory()) continue;
				std::string fileName = v.path().filename().string();
				char A[512] = "";
				strcpyf(A, "%s\\%s", temp, fileName.c_str());
				if (fileName.ends_with(".disabled")) {
					A[strlen(A) - 9] = 0;
				}
				char B[512] = "";
				strcpyf(B, "%s\\%s", temp, fileName.c_str());
				rename(B, A);
			}
			SendMessage(*lisMods, LB_RESETCONTENT, 0, 0);
			for (auto& v : std::filesystem::directory_iterator::directory_iterator(temp)) {
				if (v.is_directory()) continue;
				std::string fileName = v.path().filename().string();
				char A[512] = "";
				if (fileName.ends_with(".disabled")) {
					strcpy(A, doTranslate("type.mods.disabled"));
				}
				else {
					strcpy(A, doTranslate("type.mods.enabled"));
				}
				const int tempInt = strlen(A);
				strcat(A, fileName.c_str());
				int n = find(A + tempInt, ".disabled");
				if (n != -1) A[n + tempInt] = 0;
				lisMods->add(A);
			}
			labModsPrompt->hide();
		}
		else {
			lisMods->hide();
		}

		return 0;
		});

	btnModsDisable = new RvG::Button("do.mods.disable", 300, 200, 160, 100, pageMods);
	btnModsDisable->setTranslate("do.mods.disable", RvG::lang);
	btnModsDisable->bindCommand([](HWND win, HWND btn)->int {
		sz = 1024;
		char temp[1026];

		struct _stat fileStat;
		sz = 1024;
		RegGetValueA(hData, NULL, "MinecraftDirectory", RRF_RT_REG_SZ, NULL, temp, &sz);
		strcat(temp, "\\mods");

		if ((_stat(temp, &fileStat) == 0) && (fileStat.st_mode & _S_IFDIR)) {
			for (auto& v : std::filesystem::directory_iterator::directory_iterator(temp)) {
				if (v.is_directory()) continue;
				std::string fileName = v.path().filename().string();
				char A[512] = "";
				strcpyf(A, "%s\\%s", temp, fileName.c_str());
				if (!fileName.ends_with(".disabled")) {
					strcat(A, ".disabled");
				}
				char B[512] = "";
				strcpyf(B, "%s\\%s", temp, fileName.c_str());
				rename(B, A);
			}
			SendMessage(*lisMods, LB_RESETCONTENT, 0, 0);
			for (auto& v : std::filesystem::directory_iterator::directory_iterator(temp)) {
				if (v.is_directory()) continue;
				std::string fileName = v.path().filename().string();
				char A[512] = "";
				if (fileName.ends_with(".disabled")) {
					strcpy(A, doTranslate("type.mods.disabled"));
				}
				else {
					strcpy(A, doTranslate("type.mods.enabled"));
				}
				const int tempInt = strlen(A);
				strcat(A, fileName.c_str());
				int n = find(A + tempInt, ".disabled");
				if (n != -1) A[n + tempInt] = 0;
				lisMods->add(A);
			}
			labModsPrompt->hide();
		}
		else {
			lisMods->hide();
		}

		return 0;
		});

	btnModsOpen = new RvG::Button("do.open", 300, 300, 160, 100, pageMods);
	btnModsOpen->setTranslate("do.open", RvG::lang);
	btnModsOpen->bindCommand([](HWND win, HWND btn)->int {
		struct _stat fileStat;
		char temp[514];
		sz = 512;
		RegGetValueA(hData, NULL, "MinecraftDirectory", RRF_RT_REG_SZ, NULL, temp, &sz);
		strcat(temp, "\\mods");

		if ((_stat(temp, &fileStat) == 0) && (fileStat.st_mode & _S_IFDIR)) {

			HANDLE hRead, hWrite;
			STARTUPINFOA si;
			PROCESS_INFORMATION pi;
			SECURITY_ATTRIBUTES sa;
			sa.nLength = sizeof(SECURITY_ATTRIBUTES);
			sa.lpSecurityDescriptor = NULL;
			sa.bInheritHandle = TRUE;

			if (!CreatePipe(&hRead, &hWrite, &sa, 0)) {
				DWORD ret = GetLastError();
				return ret ? ret : -1;
			}

			ZeroMemory(&si, sizeof(STARTUPINFO));

			si.cb = sizeof(STARTUPINFO);
			GetStartupInfoA(&si);
			si.hStdError = hWrite;
			si.hStdOutput = hWrite;
			si.wShowWindow = SW_HIDE;
			si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
			char tmpS[1026];
			strcpyf(tmpS, "cmd /c explorer %s", temp);
			if (!CreateProcessA(NULL, tmpS, NULL, NULL, TRUE, NULL,
				NULL, NULL, &si, &pi)) {
				DWORD ret = GetLastError();
				CloseHandle(hRead);
				CloseHandle(hWrite);
				return ret ? ret : -1;
			}

			CloseHandle(hWrite);
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
			char buf[4098];
			DWORD bytesRead;
		}

		return 0;
		});

	sz = 1024;
	RegGetValueA(hData, NULL, "MinecraftDirectory", RRF_RT_REG_SZ, NULL, temp, &sz);
	strcat(temp, "\\mods");

	if ((_stat(temp, &fileStat) == 0) && (fileStat.st_mode & _S_IFDIR)) {
		for (auto& v : std::filesystem::directory_iterator::directory_iterator(temp)) {
			if (v.is_directory()) continue;
			std::string fileName = v.path().filename().string();
			char A[512] = "";
			if (fileName.ends_with(".disabled")) {
				strcpy(A, doTranslate("type.mods.disabled"));
			}
			else {
				strcpy(A, doTranslate("type.mods.enabled"));
			}
			const int tempInt = strlen(A);
			strcat(A, fileName.c_str());
			int n = find(A + tempInt, ".disabled");
			if (n != -1) A[n + tempInt] = 0;
			lisMods->add(A);
		}
		labModsPrompt->hide();
	}
	else {
		lisMods->hide();
	}



	// Page Minecraft BE

	pageMinecraftBE = new RvG::Container(170, 25, 600, 400, x);
	
	btnMinecraftBELaunch = new RvG::Button("do.mcbe.launch", 0, 0, 160, 100, pageMinecraftBE);
	btnMinecraftBELaunch->setTranslate("do.mcbe.launch", RvG::lang);
	btnMinecraftBELaunch->bindCommand([](HWND win, HWND btn)->int {
		HANDLE hRead, hWrite;
		STARTUPINFOA si;
		PROCESS_INFORMATION pi;
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;

		if (!CreatePipe(&hRead, &hWrite, &sa, 0)) {
			DWORD ret = GetLastError();
			return ret ? ret : -1;
		}

		ZeroMemory(&si, sizeof(STARTUPINFO));

		si.cb = sizeof(STARTUPINFO);
		GetStartupInfoA(&si);
		si.hStdError = hWrite;
		si.hStdOutput = hWrite;
		si.wShowWindow = SW_HIDE;
		si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
		char tmpS[256];
		strcpy(tmpS, "cmd /c start shell:appsfolder\\Microsoft.MinecraftUWP_8wekyb3d8bbwe!app");
		if (!CreateProcessA(NULL, tmpS, NULL, NULL, TRUE, NULL,
			NULL, NULL, &si, &pi)) {
			DWORD ret = GetLastError();
			CloseHandle(hRead);
			CloseHandle(hWrite);
			return ret ? ret : -1;
		}

		CloseHandle(hWrite);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		char buf[4098];
		DWORD bytesRead; 
		return 0;
	});
	
	btnMinecraftBEPreviewLaunch = new RvG::Button("do.mcbe.launchpreview", 160, 0, 160, 100, pageMinecraftBE);
	btnMinecraftBEPreviewLaunch->setTranslate("do.mcbe.launchpreview", RvG::lang);
	btnMinecraftBEPreviewLaunch->bindCommand([](HWND win, HWND btn)->int {
		HANDLE hRead, hWrite;
		STARTUPINFOA si;
		PROCESS_INFORMATION pi;
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;

		if (!CreatePipe(&hRead, &hWrite, &sa, 0)) {
			DWORD ret = GetLastError();
			return ret ? ret : -1;
		}

		ZeroMemory(&si, sizeof(STARTUPINFO));

		si.cb = sizeof(STARTUPINFO);
		GetStartupInfoA(&si);
		si.hStdError = hWrite;
		si.hStdOutput = hWrite;
		si.wShowWindow = SW_HIDE;
		si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
		char tmpS[256];
		strcpy(tmpS, "cmd /c start shell:appsfolder\\Microsoft.MinecraftWindowsBeta_8wekyb3d8bbwe!app");
		if (!CreateProcessA(NULL, tmpS, NULL, NULL, TRUE, NULL,
			NULL, NULL, &si, &pi)) {
			DWORD ret = GetLastError();
			CloseHandle(hRead);
			CloseHandle(hWrite);
			return ret ? ret : -1;
		}

		CloseHandle(hWrite);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		char buf[4098];
		DWORD bytesRead;
		return 0;
		});



	// Page Languages

	pageLang = new RvG::Container(170, 25, 600, 400, x);

	lisLangList = new RvG::ListBox(0, 0, 300, 400, pageLang, WS_VSCROLL);

	for (Json::Value val : langs) {
		lisLangList->add(val["name"].asCString());
	}
	lisLangList->setSelIndex(intLangSel);


	// Switch Buttons

	swiMinecraft->bindCommand([](HWND win, HWND btn)->int {
		curPage->hide();
		curPage = pageMinecraft;
		if (hasPage2) {
			curPage2->hide();
			hasPage2 = 0;
		}
		curPage->show();
		struct _stat fileStat;
		GetWindowTextA(*ediSettingsDir, baseStr, 256);
		strcat(baseStr, "\\versions");
		if ((_stat(baseStr, &fileStat) == 0) && (fileStat.st_mode & _S_IFDIR)) {
			SendMessage(*lisMinecraftVersion, LB_RESETCONTENT, 0, 0);
			for (auto& v : std::filesystem::directory_iterator::directory_iterator(baseStr)) {
				std::string fileName = v.path().filename().string();
				const char* A = fileName.c_str();
				lisMinecraftVersion->add(A);
			}
			int t; sz = 4;
			RegGetValueA(hData, NULL, "SelectedLaunch", RRF_RT_REG_DWORD, NULL, &t, &sz);
			lisMinecraftVersion->setSelIndex(t);
			lisMinecraftVersion->show();
			labMinecraftVersionPrompt->hide();
		}
		else {
			labMinecraftVersionPrompt->show();
			lisMinecraftVersion->hide();
		}
		return 0;
	});
	swiAccounts->bindCommand([](HWND win, HWND btn)->int {
		curPage->hide();
		curPage = pageAccounts;
		if (hasPage2) {
			curPage2->hide();
			hasPage2 = 0;
		}
		curPage->show();
		char tempA[1026] = {};
		sz = 1024;
		initAccounts();
		if (accounts.size() != 0) {
			SendMessage(*lisAccountsList, LB_RESETCONTENT, 0, 0);
			for (Json::Value i : accounts) {
				strcpy(tempA, i["userName"].asCString());
				if (i["userType"].asInt() == 0) strcat(tempA, doTranslate("type.accounts.legacy"));
				if (i["userType"].asInt() == 1) strcat(tempA, doTranslate("type.accounts.mojang"));
				lisAccountsList->add(tempA);
			}
			lisAccountsList->setSelIndex(intAccountsSel);
			labAccountsPrompt->show();
			labAccountsPrompt->hide();
		}
		else {
			lisAccountsList->hide();
		}
		return 0;
	});
	swiSettings->bindCommand([](HWND win, HWND btn)->int {
		curPage->hide();
		curPage = pageSettings;
		if (hasPage2) {
			curPage2->hide();
			hasPage2 = 0;
		}
		curPage->show();
		return 0;
		});
	swiMods->bindCommand([](HWND win, HWND btn)->int {
		curPage->hide();
		curPage = pageMods;
		if (hasPage2) {
			curPage2->hide();
			hasPage2 = 0;
		}
		curPage->show();

		SendMessage(*lisMods, LB_RESETCONTENT, 0, 0);
		struct _stat fileStat;
		char temp[1026];
		sz = 1024;
		RegGetValueA(hData, NULL, "MinecraftDirectory", RRF_RT_REG_SZ, NULL, temp, &sz);
		strcat(temp, "\\mods");

		if ((_stat(temp, &fileStat) == 0) && (fileStat.st_mode & _S_IFDIR)) {
			for (auto& v : std::filesystem::directory_iterator::directory_iterator(temp)) {
				if (v.is_directory()) continue;
				std::string fileName = v.path().filename().string();
				char A[512] = "";
				if (fileName.ends_with(".disabled")) {
					strcpy(A, doTranslate("type.mods.disabled"));
				}
				else {
					strcpy(A, doTranslate("type.mods.enabled"));
				}
				const int tempInt = strlen(A);
				strcat(A, fileName.c_str());
				int n = find(A + tempInt, ".disabled");
				if (n != -1) A[n + tempInt] = 0;
				lisMods->add(A);
			}
			labModsPrompt->hide();
		}
		else {
			lisMods->hide();
		}
		return 0;
		});
	swiMinecraftBE->bindCommand([](HWND win, HWND btn)->int {
		curPage->hide();
		curPage = pageMinecraftBE;
		if (hasPage2) {
			curPage2->hide();
			hasPage2 = 0;
		}
		curPage->show();
		return 0;
	});
	swiLang->bindCommand([](HWND win, HWND btn)->int {
		curPage->hide();
		curPage = pageLang;
		if (hasPage2) {
			curPage2->hide();
			hasPage2 = 0;
		}
		curPage->show();
		return 0;
		});
	thread thr([]()->int {
		while (1) {
			if (curPage == pageMinecraft && IsWindowVisible(*x)) {
				intMinecraftSel = lisMinecraftVersion->getSelIndex();
				RegSetKeyValueA(hData, NULL, "SelectedLaunch", REG_DWORD, &intMinecraftSel, 4);
			}
			if (curPage == pageSettings && IsWindowVisible(*x)) {
				GetWindowTextA(*ediSettingsDir, baseStr, 256);
				RegSetKeyValueA(hData, NULL, "MinecraftDirectory", REG_SZ, baseStr, strlen(baseStr) + 1);
				GetWindowTextA(*ediSettingsWid, baseStr, 256);
				intSettingsWid = atoi(baseStr);
				RegSetKeyValueA(hData, NULL, "WindowWidth", REG_DWORD, &intSettingsWid, 4);
				GetWindowTextA(*ediSettingsHei, baseStr, 256);
				intSettingsHei = atoi(baseStr);
				RegSetKeyValueA(hData, NULL, "WindowHeight", REG_DWORD, &intSettingsHei, 4);
			}
			if (curPage == pageAccounts && IsWindowVisible(*x)) {
				intAccountsSel = lisAccountsList->getSelIndex();
				if (intAccountsSel < 0 || intAccountsSel >= accounts.size()) {
					intAccountsSel = 0;
					lisAccountsList->setSelIndex(intLangSel);
				}
				RegSetKeyValueA(hData, NULL, "SelectedAccount", REG_DWORD, &intAccountsSel, 4);
			}
			if (curPage == pageLang && IsWindowVisible(*x)) {
				if (intLangSel != lisLangList->getSelIndex()) {
					intLangSel = lisLangList->getSelIndex();
					if (intLangSel < 0 || intLangSel >= langs.size()) {
						intLangSel = 0;
						lisLangList->setSelIndex(intLangSel);
					}
					RegSetKeyValueA(hData, NULL, "SelectedLang", REG_DWORD, &intLangSel, 4);
					HRSRC hRsrc = FindResource(NULL, MAKEINTRESOURCE(langs[intLangSel]["num"].asInt()), L"lang");
					HGLOBAL IDR = LoadResource(NULL, hRsrc);
					DWORD size = SizeofResource(NULL, hRsrc);
					reader.parse((const char*)LockResource(IDR), RvG::lang);
					FreeResource(IDR);
					x->translate(RvG::lang);
				}
			}
			if (!IsWindowVisible(*x)) break;
			Sleep(50);
		}
		return 0;
	});
	thr.detach();
	thread thr2([]() {
		try {
			Response resp = Get("https://launchermeta.mojang.com/mc/game/version_manifest_v2.json");
			reader.parse(resp.GetText(), versionManifest);
			if (IsWindowVisible(*pageMinecraftDownloads)) lisMinecraftDownloads->show();
			for (Json::Value i : versionManifest["versions"]) {
				lisMinecraftDownloads->add(i["id"].asCString());
			}
			labMinecraftDownloadsPrompt->hide();
			char temp[64] = {};
			sz = 64;
			RegGetValueA(hData, NULL, "LatestKnown", RRF_RT_REG_SZ, NULL, temp, &sz);
			writeLog("Resp", temp);
			if (temp[0] == 0 || strcmp(temp, versionManifest["latest"]["snapshot"].asCString())) {
				strcpyf(temp, doTranslate("prompt.mcje.newversion"), versionManifest["latest"]["snapshot"].asCString());
				sz = strlen(versionManifest["latest"]["snapshot"].asCString())+1;
				RegSetKeyValueA(hData, NULL, "LatestKnown", REG_SZ, versionManifest["latest"]["snapshot"].asCString(), sz);
				MessageBoxA(*x, temp, doTranslate("prompt"), MB_OK | MB_ICONINFORMATION);
			}
		}
		catch (const char* msg){
			writeLog("Requesting", msg);
			versionManifest = "unable to get";
		}
		return 0;
	});
	thr2.detach();



	// Hide pages

	curPage = pageMinecraft;
	pageMinecraftDownloads->hide();
	pageAccounts->hide();
	pageSettings->hide();
	pageMods->hide();
	pageMinecraftBE->hide();
	pageLang->hide();

	x->keepResponding();

	for (pair<string, HINTERNET> hConnectPair : hosts) {
		InternetCloseHandle(hConnectPair.second);
	}
	InternetCloseHandle(hInternet);
	return 0;
}