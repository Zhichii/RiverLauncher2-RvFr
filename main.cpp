#include <river\defines.h>
#include <river\launch.h>
#include <river\data.h>
#include <river\javaDownload.h>
#include <river\accounts.h>
#include "resource.h"
#include <SDKDDKVer.h>
#include <Commctrl.h>
#include <thread>
#include <river\ui.h>

int btn1_i = 0;

int main() {
	RVG_START;
	initData();
	mkdir("RvL");
	HRSRC hRsrc = FindResource(NULL, MAKEINTRESOURCE(IDR_JAVACLASS1), L"javaclass");
	HGLOBAL IDR = LoadResource(NULL, hRsrc);
	DWORD size = SizeofResource(NULL, hRsrc);
	FILE* javaClass = fopen("RvL\\getJavaMainVersion.class", "wb");
	fwrite(LockResource(IDR), sizeof(char), size, javaClass);
	fclose(javaClass);
	FreeResource(IDR);

	x = new RvG::Window(L"RiverLauncher2", 0);
	SendMessage(*x, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(RvG::_hInstance, MAKEINTRESOURCE(IDI_ICON2)));
	SendMessage(*x, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(RvG::_hInstance, MAKEINTRESOURCE(IDI_ICON1)));
	btnLaunch = new RvG::Button(loadString(btn_launch), 0, 0, 125, 125, x);
	swiMinecraft = new RvG::Button(loadString(swi_mcje), 0, 150, 100, 50, x);
	swiAccounts = new RvG::Button(loadString(swi_accounts), 0, 200, 100, 50, x);
	swiSettings = new RvG::Button(loadString(swi_settings), 0, 250, 100, 50, x);
	swiMods = new RvG::Button(loadString(swi_mods), 0, 300, 100, 50, x);
	swiMinecraftBE = new RvG::Button(loadString(swi_mcbe), 0, 350, 100, 50, x);



	// Page Minecraft

	pageMinecraft = new RvG::Container(170, 25, 600, 400, x);
	btnMinecraftEnd = new RvG::Button(loadString(btn_minecraft_end), 0, 0, 161, 100, pageMinecraft);
	labMinecraftLog = new RvG::Label(L"The log will show here", 0, 100, 600, 400, pageMinecraft, WS_BORDER);
	struct _stat fileStat;
	char temp[1026];
	sz = 1024;
	RegGetValueA(hData, NULL, "MinecraftDirectory", RRF_RT_REG_SZ, NULL, temp, &sz);
	strcat(temp, "\\versions");
	lisMinecraftVersion = new RvG::ListBox(161, 0, 161, 100, pageMinecraft);
	labMinecraftVersionPrompt = new RvG::Label(L"Please set a valid Minecraft directory! ", 161, 0, 161, 100, pageMinecraft);
	if ((_stat(temp, &fileStat) == 0) && (fileStat.st_mode & _S_IFDIR)) {
			for (auto& v : std::filesystem::directory_iterator::directory_iterator(temp)) {
			std::string fileName = v.path().filename().string();
			const char* A = fileName.c_str();
			wchar_t W[1026] = {};
			MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, A, strlen(A), W, 1024);
			lisMinecraftVersion->add(W);
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
			MessageBox(win, L"Please set a valid Minecraft directory! ", L"Error", MB_OK | MB_ICONERROR);
			return 0;
		}
		GetWindowTextA(*ediSettingsDir, newStr, 256);
		lisMinecraftVersion->getText(intMinecraftSel, baseStr);
		DWORD rec;
		if (GetHandleInformation(pi.hProcess, &rec)) {
			MessageBox(win, L"One process is already running! ", L"Error", MB_OK | MB_ICONERROR);
		}
		else if (launchInstance(baseStr, newStr, labMinecraftLog, x) == 1) {
			MessageBox(win, L"Uncorrect Parameters! Check the folder and the instance exists! ", L"Error", MB_OK | MB_ICONERROR);
		}
		return 0;
	});
	btnMinecraftEnd->bindCommand([](HWND win, HWND btn)->int {
		if (TerminateProcess(pi.hProcess, 0)) {
			MessageBox(win, L"The process was ended successfully! ", L"Prompt", MB_OK | MB_ICONINFORMATION);
			CloseHandle(hRead);
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
		}
		else {
			MessageBox(win, L"Unable to end the process! ", L"Error", MB_OK | MB_ICONERROR);
		}
		return 0;
	});

	// Page Accounts

	pageAccounts = new RvG::Container(170, 25, 600, 400, x);
	lisAccountsList = new RvG::ListBox(161, 0, 161, 100, pageAccounts);
	labAccountsPrompt = new RvG::Label(L"Please add a new account! ", 161, 0, 161, 100, pageAccounts);
	btnAccountsRegister = new RvG::Button(L"Add account", 0, 0, 161, 100, pageAccounts);
	btnAccountsRemove = new RvG::Button(L"Remove account", 322, 0, 161, 100, pageAccounts);
	btnAccountsRegister->bindCommand(addAcc);
	btnAccountsRemove->bindCommand(remAcc);
	char tempA[1026] = {};
	wchar_t tempW[1026];
	initAccounts();
	if (accounts.size() != 0) {
		for (Json::Value i : accounts) {
			strcpy(tempA, i["userName"].asCString());
			tempW[strlen(tempA)] = 0;
			MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, tempA, strlen(tempA), tempW, 1024);
			lisAccountsList->add(tempW);
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
	
	sz = 1024;
	RegGetValue(hData, NULL, L"MinecraftDirectory", RRF_RT_REG_SZ, NULL, tempW, &sz);
	pageSettings = new RvG::Container(170, 25, 600, 400, x);
	new RvG::Label(L"Minecraft Directory", 3, 5, 200, 20, pageSettings);
	ediSettingsDir = new RvG::InputBox(tempW, 150, 0, 300, 25, pageSettings);
	new RvG::Label(L"Minecraft Window Size", 3, 35, 200, 20, pageSettings);
	sz = 4;
	RegGetValueA(hData, NULL, "WindowWidth", RRF_RT_REG_DWORD, NULL, &intSettingsWid, &sz);
	ediSettingsWid = new RvG::InputBox(_itow(intSettingsWid, tempW, 10), 150, 30, 50, 25, pageSettings);
	new RvG::Label(L"x", 200, 35, 15, 20, pageSettings, SS_CENTER);
	RegGetValueA(hData, NULL, "WindowHeight", RRF_RT_REG_DWORD, NULL, &intSettingsHei, &sz);
	ediSettingsHei = new RvG::InputBox(_itow(intSettingsHei, tempW, 10), 215, 30, 50, 25, pageSettings);


	// Page Minecraft BE
	pageMinecraftBE = new RvG::Container(170, 25, 600, 400, x);
	btnMinecraftBELaunch = new RvG::Button(L"Launch Bedrock", 0, 0, 161, 100, pageMinecraftBE);
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
	btnMinecraftBEPreviewLaunch = new RvG::Button(L"Launch Preview", 161, 0, 161, 100, pageMinecraftBE);
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



	swiMinecraft->bindCommand([](HWND win, HWND btn)->int {
		curPage->hide();
		curPage = pageMinecraft;
		curPage->show();
		struct _stat fileStat;
		GetWindowTextA(*ediSettingsDir, baseStr, 256);
		strcat(baseStr, "\\versions");
		if ((_stat(baseStr, &fileStat) == 0) && (fileStat.st_mode & _S_IFDIR)) {
			SendMessage(*lisMinecraftVersion, LB_RESETCONTENT, 0, 0);
			for (auto& v : std::filesystem::directory_iterator::directory_iterator(baseStr)) {
				std::string fileName = v.path().filename().string();
				const char* A = fileName.c_str();
				wchar_t W[1026] = {};
				MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, A, strlen(A), W, 1024);
				lisMinecraftVersion->add(W);
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
		curPage->show();
		char tempA[1026] = {};
		wchar_t tempW[1026] = {};
		sz = 1024;
		initAccounts();
		if (accounts.size() != 0) {
			SendMessage(*lisAccountsList, LB_RESETCONTENT, 0, 0);
			for (Json::Value i : accounts) {
				strcpy(tempA, i["userName"].asCString());
				MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, tempA, strlen(tempA), tempW, 1024);
				tempW[strlen(tempA)] = 0;
				lisAccountsList->add(tempW);
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
		curPage->show();
		return 0;
	});
	swiMinecraftBE->bindCommand([](HWND win, HWND btn)->int {
		curPage->hide();
		curPage = pageMinecraftBE;
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
				if (intAccountsSel < 0 || intAccountsSel >= accounts.size()) intAccountsSel = accounts.size() - 1;
				RegSetKeyValueA(hData, NULL, "SelectedAccount", REG_DWORD, &intAccountsSel, 4);
			}
			if (!IsWindowVisible(*x)) break;
			Sleep(50);
		}
		return 0;
	});
	thr.detach();

	curPage = pageMinecraft;
	pageAccounts->hide();
	pageSettings->hide();
	pageMinecraftBE->hide();
	thread thr2([]() {
		try {
			Response resp = Get("https://launchermeta.mojang.com/mc/game/version_manifest_v2.json");
			Json::Value versions;
			reader.parse(resp.GetText(), versions);
			char temp[64] = {};
			sz = 64;
			RegGetValueA(hData, NULL, "LatestKnown", RRF_RT_REG_SZ, NULL, temp, &sz);
			writeLog("Resp", temp);
			if (temp[0] == 0 || strcmp(temp, versions["latest"]["snapshot"].asCString())) {
				strcpyf(temp, "New version released: %s", versions["latest"]["snapshot"].asCString());
				sz = strlen(versions["latest"]["snapshot"].asCString())+1;
				RegSetKeyValueA(hData, NULL, "LatestKnown", REG_SZ, versions["latest"]["snapshot"].asCString(), sz);
				MessageBoxA(*x, temp, "Prompt", MB_OK | MB_ICONINFORMATION);
			}
		}
		catch (const char* msg){
			writeLog("Requesting", msg);
		}
		return 0;
	});
	thr2.detach();

	x->keepResponding();
	return 0;
}