#include "H:\RiverGuiFrame\RiverGuiFrame\rvg.h"
#include <river\defines.h>
#include <river\launch.h>
#include <river\data.h>
#include "resource.h"
#include <SDKDDKVer.h>
#include <Commctrl.h>
#include <thread>
using namespace std;

int btn1_i = 0;

RvG::Window* x;
RvG::Container* curPage;
RvG::Button* btnLaunch;

RvG::Button* swiMinecraft;
RvG::Container* pageMinecraft;
RvG::Button* btnMinecraftEnd;
RvG::Label* labMinecraftLog;
RvG::Label* lisMinecraftVersionPrompt;
RvG::ListBox* lisMinecraftVersion;
int intMinecraftSel = 0;

RvG::Button* swiMinecraftDownloads;
RvG::Container* pageMinecraftDownloads;

RvG::Button* swiAccounts;
RvG::Container* pageAccounts;
RvG::Button* btnAccountsRegister;
RvG::Button* swiAccountsAdd;
RvG::Container* pageAccountsAdd;

RvG::Button* swiSettings;
RvG::Container* pageSettings;
RvG::Label* labSettingsDir;
RvG::InputBox* ediSettingsDir;

RvG::Button* swiMods;
RvG::Container* pageMods;

char baseStr[258];
char newStr[258];

int main() {
	RVG_START;
	initData();
	x = new RvG::Window(L"RiverLauncher2", 0);
	SendMessage(x->hWnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(RvG::_hInstance, MAKEINTRESOURCE(IDI_ICON1)));
	SendMessage(x->hWnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(RvG::_hInstance, MAKEINTRESOURCE(IDI_ICON1)));
	btnLaunch = new RvG::Button(L"Launch", 0, 0, 125, 125, x);
	swiMinecraft = new RvG::Button(L"Minecraft", 0, 150, 100, 50, x);
	swiAccounts = new RvG::Button(L"Accounts", 0, 200, 100, 50, x);
	swiSettings = new RvG::Button(L"Settings", 0, 250, 100, 50, x);
	swiMods = new RvG::Button(L"Developing", 0, 300, 100, 50, x);



	// Page Minecraft

	pageMinecraft = new RvG::Container(150, 25, 600, 400, x);
	btnMinecraftEnd = new RvG::Button(L"End Process", 0, 0, 161, 100, pageMinecraft);
	labMinecraftLog = new RvG::Label(L"The log will show here", 0, 100, 600, 400, pageMinecraft, WS_BORDER);
	struct _stat fileStat;
	char temp[514];
	sz = 512;
	RegGetValueA(hData, NULL, "MinecraftDirectory", RRF_RT_REG_SZ, NULL, temp, &sz);
	strcat(temp, "\\versions");
	lisMinecraftVersion = new RvG::ListBox(161, 0, 161, 100, pageMinecraft);
	lisMinecraftVersionPrompt = new RvG::Label(L"Please set a valid Minecraft directory! ", 161, 0, 161, 100, pageMinecraft);
	if ((_stat(temp, &fileStat) == 0) && (fileStat.st_mode & _S_IFDIR)) {
			for (auto& v : std::filesystem::directory_iterator::directory_iterator(temp)) {
			std::string fileName = v.path().filename().string();
			const char* A = fileName.c_str();
			wchar_t W[514] = {};
			MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, A, strlen(A), W, 512);
			lisMinecraftVersion->add(W);
		}
		int t = 0; sz = 4;
		RegGetValueA(hData, NULL, "SelectedLaunch", RRF_RT_REG_DWORD, NULL, &t, &sz);
		lisMinecraftVersion->setSelIndex(t);
		lisMinecraftVersionPrompt->hide();
	}
	else {
		lisMinecraftVersion->hide();
	}

	btnLaunch->bindCommand([](HWND win, HWND btn)->int {
		if (lisMinecraftVersion == nullptr) {
			MessageBox(win, L"Please set a valid Minecraft directory! ", L"Error", MB_OK | MB_ICONERROR);
			return 0;
		}
		GetWindowTextA(ediSettingsDir->hWnd, newStr, 256);
		lisMinecraftVersion->getText(intMinecraftSel, baseStr);
		DWORD rec;
		if (GetHandleInformation(pi.hProcess, &rec)) {
			MessageBox(win, L"One process is already running! ", L"Error", MB_OK | MB_ICONERROR);
		}
		else if (launchInstance(baseStr, newStr, ediSettingsDir->hWnd, labMinecraftLog, x) == 1) {
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

	pageAccounts = new RvG::Container(150, 25, 600, 400, x);
	btnAccountsRegister = new RvG::Button(L"Add account", 0, 0, 161, 100, pageAccounts);
	btnAccountsRegister->bindCommand([](HWND win, HWND btn){
		RvG::Window* dialog = new RvG::Window(L"Dialog", 1, CW_USEDEFAULT, CW_USEDEFAULT, 400, 200);
		dialog->keepResponding();
		return 0;
	});

	// Page Settings
	
	wchar_t tempW[514] = {};
	sz = 512;
	RegGetValue(hData, NULL, L"MinecraftDirectory", RRF_RT_REG_SZ, NULL, tempW, &sz);
	pageSettings = new RvG::Container(150, 25, 600, 400, x);
	labSettingsDir = new RvG::Label(L"Minecraft Directory", 3, 5, 200, 20, pageSettings);
	ediSettingsDir = new RvG::InputBox(tempW, 125, 0, 300, 25, pageSettings);



	swiMinecraft->bindCommand([](HWND win, HWND btn)->int {
		curPage->hide();
		curPage = pageMinecraft;
		curPage->show();
		struct _stat fileStat;
		GetWindowTextA(ediSettingsDir->hWnd, baseStr, 256);
		strcat(baseStr, "\\versions");
		if ((_stat(baseStr, &fileStat) == 0) && (fileStat.st_mode & _S_IFDIR)) {
			SendMessage(lisMinecraftVersion->hWnd, LB_RESETCONTENT, 0, 0);
			for (auto& v : std::filesystem::directory_iterator::directory_iterator(baseStr)) {
				std::string fileName = v.path().filename().string();
				const char* A = fileName.c_str();
				wchar_t W[514] = {};
				MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, A, strlen(A), W, 512);
				lisMinecraftVersion->add(W);
			}
			int t; sz = 4;
			RegGetValueA(hData, NULL, "SelectedLaunch", RRF_RT_REG_DWORD, NULL, &t, &sz);
			lisMinecraftVersion->setSelIndex(t);
			lisMinecraftVersion->show();
			lisMinecraftVersionPrompt->hide();
		}
		else {
			lisMinecraftVersionPrompt->show();
			lisMinecraftVersion->hide();
		}
		return 0;
	});
	swiAccounts->bindCommand([](HWND win, HWND btn)->int {
		curPage->hide();
		curPage = pageAccounts;
		curPage->show();
		return 0;
	});
	swiSettings->bindCommand([](HWND win, HWND btn)->int {
		curPage->hide();
		curPage = pageSettings;
		curPage->show();
		return 0;
	});
	thread thr([]()->int {
		while (1) {
			if (curPage == pageMinecraft) {
				intMinecraftSel = lisMinecraftVersion->getSelIndex();
				RegSetKeyValueA(hData, NULL, "SelectedLaunch", REG_DWORD, &intMinecraftSel, 4);
			}
			if (curPage == pageSettings) {
				GetWindowTextA(ediSettingsDir->hWnd, baseStr, 256);
				baseStr[strlen(baseStr)] = 0;
				RegSetKeyValueA(hData, NULL, "MinecraftDirectory", REG_SZ, baseStr, strlen(baseStr) + 1);
			}
		}
		return 0;
	});
	thr.detach();

	curPage = pageMinecraft;
	pageAccounts->hide();
	pageSettings->hide();
	x->keepResponding();
	return 0;
}