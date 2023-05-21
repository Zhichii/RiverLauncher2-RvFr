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

int f1(HWND win, HWND btn) {
	wchar_t x[16];
	_itow_s(++btn1_i, x + 8, 10, 10);
	wchar_t y[128];
	lstrcpy(y, L"Ouch. You clicked me ");
	lstrcat(y, x + 5);
	lstrcat(y, L" times! Don't click me anymore! ");
	for (int i = 0; i < 8; i++)
		x[i] = L"Prompt "[i];
	SetWindowText(btn, L"Ouch");
	MessageBox(win, y, x, MB_OK | MB_ICONINFORMATION);
	return 0;
}

RvG::Window* x;
RvG::Container* curPage;
HWND hListBox;
RvG::Button* btnLaunch;

RvG::Button* swiMinecraft;
RvG::Container* pageMinecraft;
RvG::Button* btnMinecraftEnd;
RvG::Edit* ediMinecraftLog;

RvG::Button* swiMinecraftDownloads;
RvG::Container* pageMinecraftDownloads;

RvG::Button* swiAccounts;
RvG::Container* pageAccounts;
RvG::Button* btnAccountsRegister;
RvG::Button* swiAccountsAdd;
RvG::Container* pageAccountsAdd;

RvG::Button* swiSettings;
RvG::Container* pageSettings;
RvG::Edit* ediSettingsDir;

RvG::Button* swiServers;
RvG::Container* pageServers;

char baseStr[258];
char newStr[258];

int main() {
	initData();
	RvG::_hInstance = GetModuleHandle(NULL);
	x = new RvG::Window(L"RiverLauncher2");
	SendMessage(x->hWnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(RvG::_hInstance, MAKEINTRESOURCE(IDI_ICON1)));
	SendMessage(x->hWnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(RvG::_hInstance, MAKEINTRESOURCE(IDI_ICON1)));
	btnLaunch = new RvG::Button(L"Launch", 0, 0, 100, 100, x);
	swiMinecraft = new RvG::Button(L"Minecraft", 0, 150, 100, 50, x);
	swiAccounts = new RvG::Button(L"Developing", 0, 200, 100, 50, x);
	swiSettings = new RvG::Button(L"Settings", 0, 250, 100, 50, x);
	swiServers = new RvG::Button(L"Developing", 0, 300, 100, 50, x);



	// Page Minecraft

	pageMinecraft = new RvG::Container(125, 25, x);
	btnMinecraftEnd = new RvG::Button(L"End Process", 161, 0, 161, 100, pageMinecraft);
	ediMinecraftLog = new RvG::Edit(L"the log will show here", 400, 100, 600, 400, pageMinecraft);
	hListBox = CreateWindowEx(0, L"ListBox", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | LBS_HASSTRINGS, 322, 0, 161, 100, x->hWnd, (HMENU)1001, RvG::_hInstance, 0);
	for (auto& v : std::filesystem::directory_iterator::directory_iterator("F:\\Minecraft\\versions\\")) {
		std::string fileName = v.path().filename().string();
		SendMessageA(hListBox, LB_ADDSTRING, 0, (LPARAM)fileName.c_str());
	}
	int t; DWORD sz = 4;
	RegGetValueA(hData, NULL, "SelectedLaunch", RRF_RT_REG_DWORD, NULL, &t, &sz);
	SendMessage(hListBox, LB_SETCURSEL, t, 0);
	btnLaunch->bindCommand([](HWND win, HWND btn)->int {
		GetWindowTextA(ediSettingsDir->hWnd, newStr, 256);
		SendMessageA(hListBox, LB_GETTEXT, SendMessage(hListBox, LB_GETCURSEL, 0, 0), (LPARAM)baseStr);
		int t = SendMessage(hListBox, LB_GETCURSEL, 0, 0);
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

	pageAccounts = new RvG::Container(125, 25, x);
	btnAccountsRegister = new RvG::Button(L"Add account", 0, 0, 161, 100, pageAccounts);
	btnAccountsRegister->bindCommand(f1);

	// Page Settings

	pageSettings = new RvG::Container(125, 25, x);
	ediSettingsDir = new RvG::Edit(L"F:\\Minecraft", 0, 100, 400, 200, pageSettings);



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
	x->keepResponding();
	return 0;
}