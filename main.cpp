#include "H:\RiverGuiFrame\RiverGuiFrame\rvg.h"
#include <river\defines.h>
#include <river\launch.h>
#include <river\data.h>
#include "resource.h"
#include <SDKDDKVer.h>
#include <Commctrl.h>
#include <thread>
using namespace std;
using namespace requests;

int btn1_i = 0;

RvG::Window* x;
RvG::Container* curPage;
RvG::Button* btnLaunch;

RvG::Button* swiMinecraft;
RvG::Container* pageMinecraft;
RvG::Button* btnMinecraftEnd;
RvG::Label* labMinecraftLog;
RvG::Label* labMinecraftVersionPrompt;
RvG::ListBox* lisMinecraftVersion;
int intMinecraftSel = 0;

RvG::Button* swiMinecraftDownloads;
RvG::Container* pageMinecraftDownloads;

RvG::Button* swiAccounts;
RvG::Container* pageAccounts;
RvG::Button* btnAccountsRegister;
RvG::Button* swiAccountsAdd;
RvG::Container* pageAccountsAdd;
RvG::ListBox* lisAccountsList;
RvG::Label* labAccountsPrompt;

RvG::Button* swiSettings;
RvG::Container* pageSettings;
RvG::InputBox* ediSettingsDir;
RvG::InputBox* ediSettingsWid;
RvG::InputBox* ediSettingsHei;

RvG::Button* swiMods;
RvG::Container* pageMods;

RvG::Button* swiMinecraftBE;
RvG::Container* pageMinecraftBE;
RvG::Button* btnMinecraftBELaunch;
RvG::Button* btnMinecraftBEPreviewLaunch;

char baseStr[258];
char newStr[258];

int main() {
	RVG_START;
	initData();
	x = new RvG::Window(L"RiverLauncher2", 0);
	SendMessage(*x, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(RvG::_hInstance, MAKEINTRESOURCE(IDI_ICON2)));
	SendMessage(*x, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(RvG::_hInstance, MAKEINTRESOURCE(IDI_ICON1)));
	btnLaunch = new RvG::Button(L"Launch Java", 0, 0, 125, 125, x);
	swiMinecraft = new RvG::Button(L"Minecraft JE", 0, 150, 100, 50, x);
	swiAccounts = new RvG::Button(L"Accounts", 0, 200, 100, 50, x);
	swiSettings = new RvG::Button(L"Settings", 0, 250, 100, 50, x);
	swiMods = new RvG::Button(L"Developing", 0, 300, 100, 50, x);
	swiMinecraftBE = new RvG::Button(L"Minecraft BE", 0, 350, 100, 50, x);



	// Page Minecraft

	pageMinecraft = new RvG::Container(170, 25, 600, 400, x);
	btnMinecraftEnd = new RvG::Button(L"End Process", 0, 0, 161, 100, pageMinecraft);
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
	btnAccountsRegister->bindCommand([](HWND win, HWND btn) {
		thread thr([&]() {
			RvG::Window* dialog = new RvG::Window(L"Dialog", 1, CW_USEDEFAULT, CW_USEDEFAULT, 400, 250);
			RvG::Label* labDialogPromptUsernamee = new RvG::Label(L"Player Name: ", 10, 10, 362, 20, dialog);
			inpDialogUsername = new RvG::InputBox(L"Player", 10, 30, 362, 30, dialog);
			RvG::Button* btnDialogOk = new RvG::Button(L"Add", 166, 70, 100, 40, dialog);
			RvG::Button* btnDialogCan = new RvG::Button(L"Cancel", 272, 70, 100, 40, dialog);
			btnDialogOk->bindCommand([](HWND win, HWND btn) {
				Json::Value temp = Json::objectValue;
				//memset(baseStr, 0, 256);
				GetWindowTextA(*inpDialogUsername, baseStr, 256);
				temp["userName"] = baseStr;
				accounts.append(temp);
				Json::FastWriter writer;
				string s = writer.write(accounts);
				s[s.size() - 1] = 0;
				strcpy(baseStr, s.c_str());
				RegSetKeyValueA(hData, NULL, "Accounts", REG_SZ, baseStr, strlen(baseStr)+1);
				char tempA[1026];
				wchar_t tempW[1026];
				SendMessage(*lisAccountsList, LB_RESETCONTENT, 0, 0);
				for (Json::Value i : accounts) {
					strcpy(tempA, i["userName"].asCString());
					MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, tempA, strlen(tempA), tempW, 1024);
					tempW[strlen(tempA)] = 0;
					lisAccountsList->add(tempW);
				}
				lisAccountsList->show();
				lisAccountsList->setSelIndex(accounts.size() - 1);
				intAccountsSel = accounts.size() - 1;
				RegSetKeyValueA(hData, NULL, "SelectedAccount", REG_DWORD, &intAccountsSel, 4);
				labAccountsPrompt->hide();
				DestroyWindow(win);
				return 0;
			});
			btnDialogCan->bindCommand([](HWND win, HWND btn) {
				DestroyWindow(win);
				return 0;
			});
			SetFocus(*dialog);
			dialog->keepResponding();
		});
		thr.detach();
		return 0;
	});	
	char tempA[1026] = {};
	wchar_t tempW[1026];
	sz = 1024;
	RegGetValueA(hData, NULL, "Accounts", RRF_RT_REG_SZ, NULL, tempA, &sz);
	tempW[strlen(tempA)] = 0;
	reader.parse(tempA, accounts);
	sz = 4;
	RegGetValueA(hData, NULL, "SelectedAccount", RRF_RT_REG_DWORD, NULL, &intAccountsSel, &sz);
	if (accounts.size() != 0) {
		for (Json::Value i : accounts) {
			strcpy(tempA, i["userName"].asCString());
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
		RegGetValueA(hData, NULL, "Accounts", RRF_RT_REG_SZ, NULL, tempA, &sz);
		reader.parse(tempA, accounts);
		RegGetValueA(hData, NULL, "SelectedAccount", RRF_RT_REG_DWORD, NULL, &intAccountsSel, &sz);
		if (accounts.size() != 0) {
			SendMessage(*lisAccountsList, LB_RESETCONTENT, 0, 0);
			for (Json::Value i : accounts) {
				strcpy(tempA, i["userName"].asCString());
				MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, tempA, strlen(tempA), tempW, 1024);
				tempW[strlen(tempA)] = 0;
				lisAccountsList->add(tempW);
			}
			lisAccountsList->setSelIndex(intAccountsSel);
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
				baseStr[strlen(baseStr)] = 0;
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
			reader.parse((resp).GetText(), versions);
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
			writeLog("Resp", msg);
			return 0;
		}
	});
	thr2.detach();
	x->keepResponding();
	return 0;
}