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

	hRsrc = FindResource(NULL, MAKEINTRESOURCE(IDR_LANG1), L"lang");
	IDR = LoadResource(NULL, hRsrc);
	size = SizeofResource(NULL, hRsrc);
	reader.parse((const char*)LockResource(IDR), RvG::lang);
	FreeResource(IDR);


	// Set-up Pages

	x = new RvG::Window("name.launcher", 0);
	x->setTranslate("name.launcher", RvG::lang);
	
	SendMessage(*x, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(RvG::_hInstance, MAKEINTRESOURCE(IDI_ICON2)));
	SendMessage(*x, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(RvG::_hInstance, MAKEINTRESOURCE(IDI_ICON1)));
	
	pgbProgress = new RvG::ProgressBar(170, 0, 600, 25, x);
	
	btnLaunch = new RvG::Button("do.launch", 0, 0, 150, 150, x);
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

	btnMinecraftAdd = new RvG::Button("do.mcje.swi.download", 200, 0, 160, 100, pageMinecraft);
	btnMinecraftAdd->setTranslate("do.mcje.swi.download", RvG::lang);
	btnMinecraftAdd->bindCommand([](HWND win, HWND btn)->int {
		curPage->hide();
		if (hasPage2) {
			curPage2->hide();
			hasPage2 = 0;
		}
		hasPage2 = 1;
		curPage2 = pageMinecraftDownloads;
		curPage2->show();
		if (versionManifest.size() != 0) {
			lisMinecraftDownloads->show();
			labMinecraftDownloadsPrompt->hide();
		}
		else {
			labMinecraftDownloadsPrompt->show();
			lisMinecraftDownloads->hide();
		}
		return 0;
		});

	lisMinecraftVersion = new RvG::ListBox(0, 0, 200, 400, pageMinecraft, WS_VSCROLL);
	
	labMinecraftVersionPrompt = new RvG::Label("prompt.mcje.notvalid", 0, 0, 200, 100, pageMinecraft);
	labMinecraftVersionPrompt->setTranslate("prompt.mcje.notvalid", RvG::lang);

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

	btnLaunch->bindCommand([](HWND win, HWND btn)->int {
		if (lisMinecraftVersion == nullptr) {
			MessageBoxA(win, doTranslate("prompt.mcje.notvalid"), doTranslate("error"), MB_OK | MB_ICONERROR);
			return 0;
		}
		GetWindowTextA(*ediSettingsDir, newStr, 256);
		lisMinecraftVersion->getText(intMinecraftSel, baseStr);
		DWORD rec = 0;
		if (GetHandleInformation(pi.hProcess, &rec) != 0) {
			MessageBoxA(win, doTranslate("prompt.mcje.already"), doTranslate("error"), MB_OK | MB_ICONERROR);
		}
		else {
			thread thr([win]() {
				int n = launchInstance(baseStr, newStr, x);
				switch (n) {
				case 1: {
					MessageBoxA(win, doTranslate("prompt.mcje.error"), doTranslate("error"), MB_OK | MB_ICONERROR);
					break;
				}
				}
				});
			thr.detach();
		}
		return 0;
	});



	// Page Minecraft Downloads

	pageMinecraftDownloads = new RvG::Container(170, 25, 600, 400, x);
	
	btnMinecraftDownloads = new RvG::Button("do.mcje.download", 200, 0, 160, 100, pageMinecraftDownloads);
	btnMinecraftDownloads->setTranslate("do.mcje.download", RvG::lang);
	btnMinecraftDownloads->bindCommand(download);

	labMinecraftDownloadsPrompt = new RvG::Label("prompt.mcje.download.getting", 0, 0, 200, 100, pageMinecraftDownloads);
	labMinecraftDownloadsPrompt->setTranslate("prompt.mcje.download.getting", RvG::lang);
	
	lisMinecraftDownloads = new RvG::ListBox(0, 0, 200, 400, pageMinecraftDownloads, WS_VSCROLL);



	// Page Accounts

	pageAccounts = new RvG::Container(170, 25, 600, 400, x);
	
	lisAccountsList = new RvG::ListBox(0, 0, 200, 400, pageAccounts, WS_VSCROLL);
	
	labAccountsPrompt = new RvG::Label("prompt.accounts.no", 0, 0, 200, 400, pageAccounts);
	labAccountsPrompt->setTranslate("prompt.accounts.no", RvG::lang);

	btnAccountsRegister = new RvG::Button("do.accounts.add", 200, 0, 160, 100, pageAccounts);
	btnAccountsRegister->setTranslate("do.accounts.add", RvG::lang);
	btnAccountsRegister->bindCommand(addAcc);
	
	btnAccountsRemove = new RvG::Button("do.accounts.remove", 200, 100, 160, 100, pageAccounts);
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
	
	sz = 1024;
	RegGetValueA(hData, NULL, "MinecraftDirectory", RRF_RT_REG_SZ, NULL, tempA, &sz);
	
	RvG::ParentWidget* t = new RvG::Label("value.settings.dir", 3, 5, 200, 20, pageSettings);
	t -> setTranslate("value.settings.dir", RvG::lang);
	
	ediSettingsDir = new RvG::InputBox(tempA, 150, 0, 300, 25, pageSettings);
	
	t = new RvG::Label("value.settings.size", 3, 35, 200, 20, pageSettings);
	t -> setTranslate("value.settings.size", RvG::lang);
	
	sz = 4;
	RegGetValueA(hData, NULL, "WindowWidth", RRF_RT_REG_DWORD, NULL, &intSettingsWid, &sz);
	ediSettingsWid = new RvG::InputBox(_itoa(intSettingsWid, tempA, 10), 150, 30, 50, 25, pageSettings);
	new RvG::Label("x", 200, 35, 15, 20, pageSettings, SS_CENTER);
	RegGetValueA(hData, NULL, "WindowHeight", RRF_RT_REG_DWORD, NULL, &intSettingsHei, &sz);
	ediSettingsHei = new RvG::InputBox(_itoa(intSettingsHei, tempA, 10), 215, 30, 50, 25, pageSettings);



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
		char buf[4098];
		DWORD bytesRead;
		return 0;
		});



	// Page Languages

	pageLang = new RvG::Container(170, 25, 600, 400, x);
	lisLangList = new RvG::ListBox(0, 0, 200, 400, pageLang, WS_VSCROLL);
	for (Json::Value val : langs) {
		lisLangList->add(val["name"].asCString());
	}
	RegGetValueA(hData, NULL, "SelectedLang", RRF_RT_REG_DWORD, NULL, &intLangSel, &sz);
	lisLangList->setSelIndex(intLangSel);
	hRsrc = FindResource(NULL, MAKEINTRESOURCE(langs[intLangSel]["num"].asInt()), L"lang");
	IDR = LoadResource(NULL, hRsrc);
	size = SizeofResource(NULL, hRsrc);
	reader.parse((const char*)LockResource(IDR), RvG::lang);
	FreeResource(IDR);
	x->translate(RvG::lang);


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
		}
		return 0;
	});
	thr2.detach();



	// Hide pages

	curPage = pageMinecraft;
	pageMinecraftDownloads->hide();
	pageAccounts->hide();
	pageSettings->hide();
	pageMinecraftBE->hide();
	pageLang->hide();

	x->keepResponding();
	return 0;
}