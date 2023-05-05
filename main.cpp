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
RvG::Container* ctn1;
RvG::Container* ctn2;
RvG::Button* btn1;
RvG::Button* btn2;
RvG::Button* btnS1;
RvG::Button* btnS2;
RvG::Edit* edit;
char baseStr[258];
char newStr[258];

int main() {
	RvG::start();
	x = new RvG::Window(L"RiverLauncher2");
	btnS1 = new RvG::Button(L"Page Test", 0, 0, 100, 100, x);
	btnS2 = new RvG::Button(L"Page Launch", 0, 100, 100, 100, x);

	// Container 1

	ctn1 = new RvG::Container(125, 25, x);
	btn1 = new RvG::Button(L"Test Button", 0, 0, 100, 100, ctn1);
	btn1->bindCommand(f1);

	// Container 2

	ctn2 = new RvG::Container(125, 25, x);
	btn2 = new RvG::Button(L"Launch", 0, 0, 100, 100, ctn2);
	edit = new RvG::Edit(L"F:\\Minecraft\r\n23w16a-Fabric", 0, 100, 400, 200, ctn2);
	auto f = [](HWND win, HWND btn)->int {
		GetWindowTextA(edit->hWnd, baseStr, 256);
		for (int i = 0; i < 256; i++) {
			if (baseStr[i] == '\r') {
				baseStr[i] = 0;
				strcpy(newStr, baseStr + i + 2);
				break;
			}
		}
		launchInstance(newStr, baseStr, edit->hWnd);
		return 0;
	};
	btn2->bindCommand(f);

	btnS1->bindCommand([](HWND win, HWND btn)->int {
		ctn2->hide();
		ctn1->show();
		return 0;
	});
	btnS2->bindCommand([](HWND win, HWND btn)->int {
		ctn1->hide();
		ctn2->show();
		return 0;
	});

	ctn2->hide();
	x->keepResponding();
	RvG::end();
	return 0;
}