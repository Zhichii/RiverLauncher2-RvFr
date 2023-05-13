#include "H:\RiverGuiFrame\RiverGuiFrame\rvg.h"
#include <river\launch.h>
#include <Commctrl.h>
#include "resource.h"
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

RvG::Window* x;
RvG::Container* curPage;
RvG::Button* swiMinecraft;
RvG::Container* pageMinecraft;
RvG::Button* swiMinecraftDownloads;
RvG::Container* pageMinecraftDownloads;
RvG::Button* swiAccounts;
RvG::Container* pageAccounts;
RvG::Button* btn1;
RvG::Button* btn2;
RvG::Edit* edit;
RvG::Edit* edit2;
char baseStr[258];
char newStr[258];

int main() {
	RvG::start();
	x = new RvG::Window(L"RiverLauncher2");
	swiMinecraft = new RvG::Button(L"Page Launch", 0, 0, 100, 100, x);
	swiAccounts = new RvG::Button(L"Page Accounts", 0, 100, 100, 100, x);



	// Page Minecraft

	pageMinecraft = new RvG::Container(125, 25, x);
	btn2 = new RvG::Button(L"Launch", 0, 0, 100, 100, pageMinecraft);
	edit = new RvG::Edit(L"F:\\Minecraft\r\n23w18a", 0, 100, 400, 200, pageMinecraft);
	edit2 = new RvG::Edit(L"OL", 400, 100, 600, 400, pageMinecraft);
	auto f = [](HWND win, HWND btn)->int {
		GetWindowTextA(edit->hWnd, baseStr, 256);
		for (int i = 0; i < 256; i++) {
			if (baseStr[i] == '\r') {
				baseStr[i] = 0;
				strcpy(newStr, baseStr + i + 2);
				break;
			}
		}
		if (launchInstance(newStr, baseStr, edit->hWnd, edit2, x) == 1) {
			MessageBox(win, L"Uncorrect Parameters! Check the folder and the instance exists! ", L"Hint", MB_OK | MB_ICONINFORMATION);
		}
		return 0;
	};
	btn2->bindCommand(f);

	// Container 1

	pageAccounts = new RvG::Container(125, 25, x);
	btn1 = new RvG::Button(L"Test Button", 0, 0, 100, 100, pageAccounts);
	btn1->bindCommand(f1);



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

	curPage = pageMinecraft;
	pageAccounts->hide();
	x->keepResponding();
	RvG::end();
	return 0;
}