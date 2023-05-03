#include <stdio.h>
#include "H:\RiverGuiFrame\RiverGuiFrame\rvg.h"
#include <river\launch.h>
#include <Commctrl.h>
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
RvG::Edit* edit;
char baseStr[258];
char newStr[258];

int main() {
	RvG::start();
	const wchar_t className[255] = L"test";
	x = new RvG::Window(className, L"RiverLauncher2");
	btn1 = new RvG::Button(L"test1", 100, 100, 100, 100, x);
	btn1->bindCommand(f1);
	btn2 = new RvG::Button(L"test2", 100, 200, 100, 100, x);
	edit = new RvG::Edit(L"ab<ca>bc", 100, 300, 400, 200, x);
	//wchar_t placeholder[25] = L"TEST";
	//SendMessage(edit->hWnd, EM_SETCUEBANNER, TRUE, (LPARAM)placeholder);
	auto f = [](HWND win, HWND btn)->int {
		launchInstance("23w14a", "F:\\Minecraft", edit->hWnd);
		//GetWindowTextA(edit->hWnd, baseStr, 256);
		//strcpy(newStr, baseStr);
		//strcatf(newStr, "test,%d:%s! ", 1, "test");
		//sp(baseStr, 256, "<", ">", newStr);
		//writeLog("test", "%s: %% %s", "HlHill", newStr);
		//SetWindowTextA(edit->hWnd, newStr);
		return 0;
	};
	btn2->bindCommand(f);
	x->keepResponding();
	RvG::end();
	return 0;
}