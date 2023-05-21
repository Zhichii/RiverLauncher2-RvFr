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
HWND hListBox;
RvG::Button* btnLaunch;

RvG::Button* swiMinecraft;
RvG::Container* pageMinecraft;
RvG::Button* btnMinecraftEnd;
RvG::Edit* ediMinecraftLog;
RvG::ListBox* lisMinecraftVersion;

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
RvG::Edit* ediSettingsDir;

RvG::Button* swiMods;
RvG::Container* pageMods;

char baseStr[258];
char newStr[258];

int main() {
	RVG_START;
	initData();
	x = new RvG::Window(L"RiverLauncher2");
	SendMessage(x->hWnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(RvG::_hInstance, MAKEINTRESOURCE(IDI_ICON1)));
	SendMessage(x->hWnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(RvG::_hInstance, MAKEINTRESOURCE(IDI_ICON1)));
	btnLaunch = new RvG::Button(L"Launch", 0, 0, 125, 125, x);
	swiMinecraft = new RvG::Button(L"Minecraft", 0, 150, 100, 50, x);
	swiAccounts = new RvG::Button(L"Developing", 0, 200, 100, 50, x);
	swiSettings = new RvG::Button(L"Settings", 0, 250, 100, 50, x);
	swiMods = new RvG::Button(L"Developing", 0, 300, 100, 50, x);



	// Page Minecraft

	pageMinecraft = new RvG::Container(150, 25, 600, 400, x);
	btnMinecraftEnd = new RvG::Button(L"End Process", 0, 0, 161, 100, pageMinecraft);
	ediMinecraftLog = new RvG::Edit(L"the log will show here", 0, 100, 600, 400, true, pageMinecraft);
	lisMinecraftVersion = new RvG::ListBox(161, 0, 161, 100, pageMinecraft);
	for (auto& v : std::filesystem::directory_iterator::directory_iterator("F:\\Minecraft\\versions\\")) {
		std::string fileName = v.path().filename().string();
		lisMinecraftVersion->add(fileName.c_str());
	}
	int t;
	RegGetValueA(hData, NULL, "SelectedLaunch", RRF_RT_REG_DWORD, NULL, &t, &sz);
	lisMinecraftVersion->setSelIndex(t);
	btnLaunch->bindCommand([](HWND win, HWND btn)->int {
		GetWindowTextA(ediSettingsDir->hWnd, newStr, 256);
		int t = lisMinecraftVersion->getSelIndex();
		lisMinecraftVersion->getText(t, baseStr);
		RegSetKeyValueA(hData, NULL, "SelectedLaunch", REG_DWORD, &t, 4);
		DWORD rec;
		if (GetHandleInformation(pi.hProcess, &rec)) {
			MessageBox(win, L"One process is already running! ", L"Error", MB_OK | MB_ICONERROR);
		}
		else if (launchInstance(baseStr, newStr, ediSettingsDir->hWnd, ediMinecraftLog, x) == 1) {
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
		wchar_t x[16];
		_itow_s(++btn1_i, x + 8, 10, 10);
		wchar_t y[128];
		lstrcpy(y, L"Ouch. You clicked me ");
		lstrcat(y, x + 8);
		lstrcat(y, L" times! Don't click me anymore! ");
		for (int i = 0; i < 8; i++)
			x[i] = L"Prompt "[i];
		SetWindowText(btn, L"Ouch");
		MessageBox(win, y, x, MB_OK | MB_ICONINFORMATION);
		return 0;
	});

	// Page Settings

	pageSettings = new RvG::Container(150, 25, 600, 400, x);
	labSettingsDir = new RvG::Label(L"Minecraft Folder", 3, 5, 200, 20, pageSettings);
	ediSettingsDir = new RvG::Edit(L"F:\\Minecraft", 107, 0, 300, 25, false, pageSettings);



	swiMinecraft->bindCommand([](HWND win, HWND btn)->int {
		curPage->hide();
		curPage = pageMinecraft;
		curPage->show();
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

	curPage = pageMinecraft;
	pageAccounts->hide();
	pageSettings->hide();
	x->keepResponding();
	return 0;
}