#include <stdio.h>
#include "H:\RiverGuiFrame\rvg.h"
#include <river\defines.h>
#include <thread>
using namespace std;

int btn1_i = 0;

int f1(HWND win, HWND btn) {
	wchar_t x[16];
	_itow_s(++btn1_i, x + 5, 10, 10);
	wchar_t y[128];
	lstrcpy(y, L"Ouch. You clicked me ");
	lstrcat(y, x + 5);
	lstrcat(y, L" times! Don't click me anymore! ");
	for (int i = 0; i < 5; i++)
		x[i] = L"Hint "[i];
	SetWindowText(btn, L"Ouch");
	MessageBox(win, y, x, MB_OK | MB_ICONINFORMATION);
	return 0;
}

int f2(HWND win, HWND btn) {
	MessageBox(win, L"Please click the other button not this! ", L"Hint", MB_OK | MB_ICONINFORMATION);
	return 0;
}

RvG::Window* x;
RvG::Button* btn1;
RvG::Button* btn2;
HWND edit;
char baseStr[258];
char newStr[258];

int main() {
	RvG::start();
	const wchar_t className[255] = L"test";
	x = new RvG::Window(className, L"RiverLauncher2");
	btn1 = new RvG::Button(L"test1", 100, 100, 100, 100, x);
	btn1->bindCommand(f1);
	btn2 = new RvG::Button(L"test2", 100, 200, 100, 100, x);
	edit = CreateWindow(L"Edit", L"RiverGuiFrame Button using Win32", WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
		100, 300, 400, 200, x->hWnd, (HMENU)3, RvG::hInstance, nullptr);
	ShowWindow(edit, 1);
	SetWindowText(edit, L"LoremIpsum");
	auto f = [](HWND win, HWND btn)->int {
		GetWindowTextA(edit, baseStr, 256);
		replace(256, baseStr, newStr, "tesa", "xxx");
		SetWindowTextA(edit, newStr);
		return 0;
	};
	btn2->bindCommand(f);
	x->keepResponding();
	RvG::end();
	return 0;
}