#pragma once

#include <river/defines.h>

char strInpBox[256];

int initAccounts() {
	char tempA[1026] = {};
	sz = 1024;
	RegGetValueA(hData, NULL, "Accounts", RRF_RT_REG_SZ, NULL, tempA, &sz);
	reader.parse(tempA, accounts);
	sz = 4;
	RegGetValueA(hData, NULL, "SelectedAccount", RRF_RT_REG_DWORD, NULL, &intAccountsSel, &sz);
	int i = 0;
	int n = accounts.size();
	while (i < n) {
		if (!accounts[i].isMember("userName")) {
			accounts.removeIndex(i, NULL);
			n = accounts.size();
			if (i >= n) break;
			continue;
		}
		if (!accounts[i].isMember("userType"))	accounts[i]["userType"] = 0;
		if (!accounts[i].isMember("userId")) 	accounts[i]["userId"] = "4d696e6563726166744d696e65637261";
		if (!accounts[i].isMember("userToken"))	accounts[i]["userToken"] = "${auth_access_token}";
		if (!accounts[i].isMember("userSkin"))	accounts[i]["userSkin"] = "";
		if (!accounts[i].isMember("userSkinId"))accounts[i]["userSkinId"] = "";
		if (!accounts[i].isMember("userCapes")) accounts[i]["userCapes"] = Json::arrayValue;
		i++;
	}
	Json::FastWriter writer;
	string s = writer.write(accounts);
	RegSetKeyValueA(hData, NULL, "Accounts", REG_SZ, s.c_str(), strlen(s.c_str()) + 1);
	if (intAccountsSel < 0 || intAccountsSel >= n) intAccountsSel = accounts.size() - 1;
	RegSetKeyValueA(hData, NULL, "SelectedAccount", REG_DWORD, &intAccountsSel, 4);
	return 0;
}

int addOfficialAcc(HWND win, HWND btn);

int addOfflineAcc(HWND win, HWND btn);

int addAcc(HWND win, HWND btn) {
	thread thr([=]()->int {
		RvG::Window* dialog = new RvG::Window(L"Dialog", 1, CW_USEDEFAULT, CW_USEDEFAULT, 750, 250);
		new RvG::Label(L"Account Type: ", 10, 10, 512, 20, dialog);
		RvG::Button* btnDialogOfficial = new RvG::Button(L"Add Official Account", 410, 157, 100, 40, dialog);
		RvG::Button* btnDialogOffline = new RvG::Button(L"Add Offline Account", 516, 157, 100, 40, dialog);
		RvG::Button* btnDialogCan = new RvG::Button(L"Cancel", 622, 157, 100, 40, dialog);
		btnDialogCan->bindCommand([](HWND win, HWND btn) {
			DestroyWindow(win);
			return 0;
			});
		btnDialogOfficial->bindCommand(addOfficialAcc);
		btnDialogOffline->bindCommand(addOfflineAcc);
		SetFocus(*dialog);
		dialog->keepResponding();
		return 0;
		});
	thr.detach();
	return 0;
}

int addOfficialAcc(HWND win, HWND btn) {
	RvG::Window* dialog = (RvG::Window*)GetWindowLongPtr(win, GWLP_USERDATA);

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	GetStartupInfoA(&si);
	si.hStdError = hWrite;
	si.hStdOutput = hWrite;
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	char output[256];
	strcpy(output, "explorer \"https://login.live.com/oauth20_authorize.srf?client_id=00000000402b5328&response_type=code&scope=XboxLive.signin%20offline_access&redirect_uri=https%3a%2f%2flogin.live.com%2foauth20_desktop.srf\"");
	if (!CreateProcessA(NULL, output, NULL, NULL, TRUE, NULL,
		NULL, NULL, &si, &pi)) {
		DWORD ret = GetLastError();
		CloseHandle(hRead);
		CloseHandle(hWrite);
		return ret ? ret : -1;
	}

	CloseHandle(hRead);
	CloseHandle(hWrite);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	for (RvG::ParentWidget* i : dialog->children) {
		if (i == nullptr) break;
		i->remove();
	}
	new RvG::Label(L"After the page become an blank page, please copy the address here: ", 10, 10, 712, 20, dialog);
	inpDialogUsername = new RvG::InputBox(L"Template: https://login.live.com/oauth20_desktop.srf?code=xxxx&lc=xxxx", 10, 30, 712, 30, dialog);
	RvG::Button* btnDialogOK = new RvG::Button(L"Add", 516, 157, 100, 40, dialog);
	RvG::Button* btnDialogCan = new RvG::Button(L"Cancel", 622, 157, 100, 40, dialog);
	btnDialogCan->bindCommand([](HWND win, HWND btn) {
		DestroyWindow(win);
		return 0;
		});
	btnDialogOK->bindCommand([](HWND win, HWND btn)->int {
		RvG::Window* dialog = (RvG::Window*)GetWindowLongPtr(win, GWLP_USERDATA);
		thread thr([dialog, win]()->int {
			Json::Value temp = Json::objectValue;
			Json::Value x;
			GetWindowTextA(*inpDialogUsername, strInpBox, 256);
			for (RvG::ParentWidget* i : dialog->children) {
				if (i == nullptr) break;
				i->remove();
			}
			//dialog->respond();
			new RvG::Label(L"Getting informations from your Microsoft account... (0/6)", 0, 75, 700, 20, dialog, SS_CENTER);
			//dialog->respond();

			// Get Username and token

			char tmpString[256];
			sp(strInpBox, 256, "?code=", "&lc=", tmpString);
			char url[3200];
			strcpyf(url, "https://login.live.com/oauth20_token.srf?client_id=00000000402b5328&client_secret=client_secret&code=%s&grant_type=authorization_code&redirect_uri=https://login.live.com/oauth20_desktop.srf", tmpString);

			// Step 1
			Response resp = Get(url);
			reader.parse(resp.GetText(), temp);
			char refreshToken[128];
			strcpy(refreshToken, temp["refresh_token"].asCString());
			for (RvG::ParentWidget* i : dialog->children) {
				if (i == nullptr) break;
				i->remove();
			}
			new RvG::Label(L"Getting informations from your Microsoft account... (1/6)", 0, 75, 700, 20, dialog, SS_CENTER);
			//dialog->respond();

			// Step 2
			strcpyf(url, "{\"Properties\":{\"AuthMethod\":\"RPS\", \"SiteName\":\"user.auth.xboxlive.com\", \"RpsTicket\":\"d=%s\"}, \"RelyingParty\":\"http://auth.xboxlive.com\", \"TokenType\":\"JWT\"}", temp["access_token"].asCString());
			resp = Post("https://user.auth.xboxlive.com/user/authenticate", string(url));
			reader.parse(resp.GetText(), temp);
			for (RvG::ParentWidget* i : dialog->children) {
				if (i == nullptr) break;
				i->remove();
			}
			new RvG::Label(L"Getting informations from your Microsoft account... (2/6)", 0, 75, 700, 20, dialog, SS_CENTER);
			//dialog->respond();

			// Step 3
			strcpyf(url, "{\"Properties\":{\"SandboxId\":\"RETAIL\", \"UserTokens\":[\"%s\"]}, \"RelyingParty\":\"rp://api.minecraftservices.com/\", \"TokenType\":\"JWT\"}", temp["Token"].asCString());
			resp = Post("https://xsts.auth.xboxlive.com/xsts/authorize", string(url));
			reader.parse(resp.GetText(), temp);
			for (RvG::ParentWidget* i : dialog->children) {
				if (i == nullptr) break;
				i->remove();
			}
			new RvG::Label(L"Getting informations from your Microsoft account... (3/6)", 0, 75, 700, 20, dialog, SS_CENTER);
			//dialog->respond();

			// Step 4
			strcpyf(url, "{\"identityToken\":\"XBL3.0 x=%s;%s\"}", temp["DisplayClaims"]["xui"][0]["uhs"].asCString(), temp["Token"].asCString());
			resp = Post("https://api.minecraftservices.com/authentication/login_with_xbox", string(url));
			reader.parse(resp.GetText(), temp);
			for (RvG::ParentWidget* i : dialog->children) {
				if (i == nullptr) break;
				i->remove();
			}
			new RvG::Label(L"Getting informations from your Microsoft account... (4/6)", 0, 75, 700, 20, dialog, SS_CENTER);
			//dialog->respond();

			// Has Minecraft? 
			map<string, string> headers;
			headers["Authorization"] = "Bearer " + temp["access_token"].asString();
			resp = Get("https://api.minecraftservices.com/entitlements/mcstore", headers);
			reader.parse(resp.GetText(), x);
			int fl = 0;
			for (Json::Value i : x["items"]) {
				if (strcmp(i["name"].asCString(), "game_minecraft")) {
					fl = 1;
					break;
				}
			}
			if (fl == 0) {
				//* Hey: errorOfficiak();
				return 1;
			}
			for (RvG::ParentWidget* i : dialog->children) {
				if (i == nullptr) break;
				i->remove();
			}
			new RvG::Label(L"Getting informations from your Microsoft account... (5/6)", 0, 75, 700, 20, dialog, SS_CENTER);
			//dialog->respond();

			// Step 5
			resp = Get("https://api.minecraftservices.com/minecraft/profile", headers);
			reader.parse(resp.GetText(), x);
			for (RvG::ParentWidget* i : dialog->children) {
				if (i == nullptr) break;
				i->remove();
			}
			new RvG::Label(L"Getting informations from your Microsoft account... (6/6)", 0, 75, 700, 20, dialog, SS_CENTER);
			// Check re-add
			bool readd = 0;
			for (Json::Value i : accounts) {
				if (strcmp(i["userName"].asCString(), x["name"].asCString()) == 0)
					if (i["userType"].asInt() == 1) {
						i["userId"] = x["id"];
						i["userToken"] = temp["access_token"];
						i["userSkin"] = x["skins"][0]["url"];
						i["userSkinId"] = x["skins"][0]["id"];
						i["userCapes"] = Json::arrayValue;
						readd = 1;
						break;
					}
			}
			if (readd) {
				for (RvG::ParentWidget* i : dialog->children) {
					if (i == nullptr) break;
					i->remove();
				}
				staticLab = new RvG::Label(L"You have already added this Microsoft account! ", 0, 75, 700, 20, dialog, SS_CENTER);
				staticBtn = new RvG::Button(L"OK", 622, 157, 100, 40, dialog);
				staticBtn->bindCommand([](HWND win, HWND btn) {
					DestroyWindow(win);
					return 0;
					});
				dialog->keepResponding();
				return 0;
			};

			// Set Temp

			Json::Value profile = Json::objectValue;
			profile["userType"] = 1;
			profile["userName"] = x["name"];
			profile["userId"] = x["id"];
			profile["userToken"] = temp["access_token"];
			profile["userSkin"] = x["skins"][0]["url"];
			profile["userSkinId"] = x["skins"][0]["id"];
			profile["userCapes"] = Json::arrayValue;

			accounts.append(profile);
			Json::FastWriter writer;
			string s = writer.write(accounts);
			RegSetKeyValueA(hData, NULL, "Accounts", REG_SZ, s.c_str(), s.size() + 1);
			char tempA[512];
			wchar_t tempW[512];
			SendMessage(*lisAccountsList, LB_RESETCONTENT, 0, 0);
			for (Json::Value i : accounts) {
				strcpy(tempA, i["userName"].asCString());
				MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, tempA, strlen(tempA), tempW, 512);
				tempW[strlen(tempA)] = 0;
				if (i["userType"].asInt() == 0) lstrcat(tempW, L" (Offline)");
				if (i["userType"].asInt() == 1) lstrcat(tempW, L" (Official)");
				lisAccountsList->add(tempW);
			}
			lisAccountsList->show();
			lisAccountsList->setSelIndex(accounts.size() - 1);
			intAccountsSel = accounts.size() - 1;
			RegSetKeyValueA(hData, NULL, "SelectedAccount", REG_DWORD, &intAccountsSel, 4);
			labAccountsPrompt->hide();
			dialog->remove();
			return 0;
			});
		thr.detach();
		return 0;
		});
	SetFocus(*dialog);
	return 0;
}

int addOfflineAcc(HWND win, HWND btn) {
	RvG::Window* dialog = (RvG::Window*)GetWindowLongPtr(win, GWLP_USERDATA);
	writeLog("AddOfflineAccount", "%l", dialog);
	for (RvG::ParentWidget* i : dialog->children) {
		if (i == nullptr) break;
		i->remove();
	}
	new RvG::Label(L"Account Name: ", 10, 10, 562, 20, dialog);
	inpDialogUsername = new RvG::InputBox(L"Player", 10, 30, 712, 30, dialog);
	RvG::Button* btnDialogOK = new RvG::Button(L"Add", 516, 157, 100, 40, dialog);
	RvG::Button* btnDialogCan = new RvG::Button(L"Cancel", 622, 157, 100, 40, dialog);
	btnDialogCan->bindCommand([](HWND win, HWND btn) {
		DestroyWindow(win);
		return 0;
		});
	btnDialogOK->bindCommand([](HWND win, HWND btn) {
		Json::Value temp = Json::objectValue;
		GetWindowTextA(*inpDialogUsername, strInpBox, 256);

		temp["userType"] = 0;
		temp["userName"] = strInpBox;
		temp["userId"] = "4d696e6563726166744d696e65637261";
		temp["userToken"] = "${auth_access_token}";
		temp["userSkin"] = "";
		temp["userSkinId"] = "";
		temp["userCapes"] = Json::arrayValue;

		accounts.append(temp);
		Json::FastWriter writer;
		string s = writer.write(accounts);
		RegSetKeyValueA(hData, NULL, "Accounts", REG_SZ, s.c_str(), strlen(s.c_str()) + 1);
		char tempA[512];
		wchar_t tempW[512];
		SendMessage(*lisAccountsList, LB_RESETCONTENT, 0, 0);
		for (Json::Value i : accounts) {
			strcpy(tempA, i["userName"].asCString());
			MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, tempA, strlen(tempA), tempW, 512);
			tempW[strlen(tempA)] = 0;
			if (i["userType"].asInt() == 0) lstrcat(tempW, L" (Offline)");
			if (i["userType"].asInt() == 1) lstrcat(tempW, L" (Official)");
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
	SetFocus(*dialog);
	return 0;
};

int remAcc(HWND win, HWND btn) {
	thread thr([]() {
		RvG::Window* dialog = new RvG::Window(L"Dialog", 1, CW_USEDEFAULT, CW_USEDEFAULT, 750, 250);
		char temp[256] = {};
		wchar_t t2[256] = {};
		sprintf(temp, "Really remove account '%s' ?", accounts[intAccountsSel]["userName"].asCString());
		MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, temp, strlen(temp), t2, 256);
		new RvG::Label(t2, 10, 10, 512, 20, dialog);
		RvG::Button* btnDialogOK = new RvG::Button(L"Yes", 516, 157, 100, 40, dialog);
		RvG::Button* btnDialogCan = new RvG::Button(L"No", 622, 157, 100, 40, dialog);
		btnDialogCan->bindCommand([](HWND win, HWND btn) {
			DestroyWindow(win);
			return 0;
			});
		btnDialogOK->bindCommand([](HWND win, HWND btn)->int {
			accounts.removeIndex(intAccountsSel, NULL);
			if (intAccountsSel < 0 || intAccountsSel >= accounts.size()) intAccountsSel = accounts.size() - 1;
			Json::FastWriter writer;
			string s = writer.write(accounts);
			RegSetKeyValueA(hData, NULL, "Accounts", REG_SZ, s.c_str(), strlen(s.c_str()) + 1);
			char tempA[512];
			wchar_t tempW[512];
			if (accounts.size() != 0) {
				SendMessage(*lisAccountsList, LB_RESETCONTENT, 0, 0);
				for (Json::Value i : accounts) {
					strcpy(tempA, i["userName"].asCString());
					tempW[strlen(tempA)] = 0;
					MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, tempA, strlen(tempA), tempW, 512);
					if (i["userType"].asInt() == 0) lstrcat(tempW, L" (Offline)");
					if (i["userType"].asInt() == 1) lstrcat(tempW, L" (Official)");
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
			DestroyWindow(win);
			return 0;
			});
		SetFocus(*dialog);
		dialog->keepResponding();
		});
	thr.detach();
	return 0;
}