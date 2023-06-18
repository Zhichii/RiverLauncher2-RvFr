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
		RvG::Window* dialog = new RvG::Window(doTranslate("dialog"), 1, CW_USEDEFAULT, CW_USEDEFAULT, 750, 250);
		new RvG::Label(doTranslate("type.accounts.ask"), 10, 10, 512, 20, dialog);
		RvG::Button* btnDialogOfficial = new RvG::Button(doTranslate("do.accounts.add.mojang"), 410, 157, 100, 40, dialog);
		RvG::Button* btnDialogOffline = new RvG::Button(doTranslate("do.accounts.add.legacy"), 516, 157, 100, 40, dialog);
		RvG::Button* btnDialogCan = new RvG::Button(doTranslate("sel.cancel"), 622, 157, 100, 40, dialog);
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

	CloseHandle(hWrite);
	CloseHandle(hRead);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	for (RvG::ParentWidget* i : dialog->children) {
		if (i == nullptr) break;
		i->remove();
	}
	new RvG::Label(doTranslate("prompt.accounts.after"), 10, 10, 712, 20, dialog);
	inpDialogUsername = new RvG::InputBox("https://login.live.com/oauth20_desktop.srf?code=......&lc=....", 10, 30, 712, 30, dialog);
	RvG::Button* btnDialogOK = new RvG::Button(doTranslate("do.accounts.sure"), 516, 157, 100, 40, dialog);
	RvG::Button* btnDialogCan = new RvG::Button(doTranslate("sel.cancel"), 622, 157, 100, 40, dialog);
	btnDialogCan->bindCommand([](HWND win, HWND btn) {
		DestroyWindow(win);
		return 0;
		});
	btnDialogOK->bindCommand([](HWND win, HWND btn)->int {
		TerminateProcess(pi.hProcess, 0);
		pi.hProcess = 0;
		RvG::Window* dialog = (RvG::Window*)GetWindowLongPtr(win, GWLP_USERDATA);
		thread thr([dialog, win]()->int {
			char tS[256];
			Json::Value temp = Json::objectValue;
			Json::Value x;
			GetWindowTextA(*inpDialogUsername, strInpBox, 256);
			for (RvG::ParentWidget* i : dialog->children) {
				if (i == nullptr) break;
				i->remove();
			}
			strcpyf(tS, doTranslate("prompt.accounts.step"), 0);
			new RvG::Label(tS, 0, 75, 700, 20, dialog, SS_CENTER);
			
			// Get Username and token

			pgbProgress->set(0);
			char tmpString[256];
			sp(strInpBox, 256, "?code=", "&lc=", tmpString);
			char url[3200];
			strcpyf(url, "https://login.live.com/oauth20_token.srf?client_id=00000000402b5328&client_secret=client_secret&code=%s&grant_type=authorization_code&redirect_uri=https://login.live.com/oauth20_desktop.srf", tmpString);

			// Step 1
			Response resp = Get(url);
			reader.parse(resp.GetText(), temp);
			strcpyf(url, "{\"Properties\":{\"AuthMethod\":\"RPS\", \"SiteName\":\"user.auth.xboxlive.com\", \"RpsTicket\":\"d=%s\"}, \"RelyingParty\":\"http://auth.xboxlive.com\", \"TokenType\":\"JWT\"}", temp["access_token"].asCString());
			char refreshToken[128];
			strcpy(refreshToken, temp["refresh_token"].asCString());
			for (RvG::ParentWidget* i : dialog->children) {
				if (i == nullptr) break;
				i->remove();
			}
			strcpyf(tS, doTranslate("prompt.accounts.step"), 1);
			new RvG::Label(tS, 0, 75, 700, 20, dialog, SS_CENTER);
			pgbProgress->set(100 / 6 * 1);

			// Step 2
			resp = Post("https://user.auth.xboxlive.com/user/authenticate", string(url));
			reader.parse(resp.GetText(), temp);
			strcpyf(url, "{\"Properties\":{\"SandboxId\":\"RETAIL\", \"UserTokens\":[\"%s\"]}, \"RelyingParty\":\"rp://api.minecraftservices.com/\", \"TokenType\":\"JWT\"}", temp["Token"].asCString());
			for (RvG::ParentWidget* i : dialog->children) {
				if (i == nullptr) break;
				i->remove();
			}
			strcpyf(tS, doTranslate("prompt.accounts.step"), 2);
			new RvG::Label(tS, 0, 75, 700, 20, dialog, SS_CENTER);
			pgbProgress->set(100 / 6 * 2);

			// Step 3
			resp = Post("https://xsts.auth.xboxlive.com/xsts/authorize", string(url));
			reader.parse(resp.GetText(), temp);
			strcpyf(url, "{\"identityToken\":\"XBL3.0 x=%s;%s\"}", temp["DisplayClaims"]["xui"][0]["uhs"].asCString(), temp["Token"].asCString());
			for (RvG::ParentWidget* i : dialog->children) {
				if (i == nullptr) break;
				i->remove();
			}
			strcpyf(tS, doTranslate("prompt.accounts.step"), 3);
			new RvG::Label(tS, 0, 75, 700, 20, dialog, SS_CENTER);
			pgbProgress->set(100 / 6 * 3);

			// Step 4
			resp = Post("https://api.minecraftservices.com/authentication/login_with_xbox", string(url));
			reader.parse(resp.GetText(), temp);
			for (RvG::ParentWidget* i : dialog->children) {
				if (i == nullptr) break;
				i->remove();
			}
			strcpyf(tS, doTranslate("prompt.accounts.step"), 4);
			new RvG::Label(tS, 0, 75, 700, 20, dialog, SS_CENTER);
			pgbProgress->set(100 / 6 * 4);

			// Step 5: Has Minecraft? 
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
			strcpyf(tS, doTranslate("prompt.accounts.step"), 5);
			new RvG::Label(tS, 0, 75, 700, 20, dialog, SS_CENTER);
			pgbProgress->set(100 / 6 * 5);

			// Step 6
			resp = Get("https://api.minecraftservices.com/minecraft/profile", headers);
			reader.parse(resp.GetText(), x);
			for (RvG::ParentWidget* i : dialog->children) {
				if (i == nullptr) break;
				i->remove();
			}
			strcpyf(tS, doTranslate("prompt.accounts.step"), 6);
			new RvG::Label(tS, 0, 75, 700, 20, dialog, SS_CENTER);
			pgbProgress->set(100);
			
			// Check re-add
			bool readd = 0;
			for (int i = 0; i < accounts.size(); i ++) {
				if (strcmp(accounts[i]["userName"].asCString(), x["name"].asCString()) == 0)
					if (accounts[i]["userType"].asInt() == 1) {
						accounts[i]["userId"] = x["id"].asCString();
						writeLog("???", "%s\n%s", accounts[i]["userToken"].asCString(), temp["access_token"].asCString());
						accounts[i]["userToken"] = temp["access_token"].asCString();
						accounts[i]["userSkin"] = x["skins"][0]["url"].asCString();
						accounts[i]["userSkinId"] = x["skins"][0]["id"].asCString();
						accounts[i]["userCapes"] = Json::arrayValue;
						Json::FastWriter writer;
						string s = writer.write(accounts);
						RegSetKeyValueA(hData, NULL, "Accounts", REG_SZ, s.c_str(), s.size() + 1);
						readd = 1;
						break;
					}
			}
			if (readd) {
				for (RvG::ParentWidget* i : dialog->children) {
					if (i == nullptr) break;
					i->remove();
				}
				staticLab = new RvG::Label(doTranslate("prompt.accounts.already"), 0, 75, 700, 20, dialog, SS_CENTER);
				staticBtn = new RvG::Button(doTranslate("sel.yes"), 622, 157, 100, 40, dialog);
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
			SendMessage(*lisAccountsList, LB_RESETCONTENT, 0, 0);
			for (Json::Value i : accounts) {
				strcpy(tempA, i["userName"].asCString());
				if (i["userType"].asInt() == 0) strcat(tempA, " (Offline)");
				if (i["userType"].asInt() == 1) strcat(tempA, " (Official)");
				lisAccountsList->add(tempA);
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
	new RvG::Label(doTranslate("prompt.accounts.name"), 10, 10, 562, 20, dialog);
	inpDialogUsername = new RvG::InputBox("Player", 10, 30, 712, 30, dialog);
	RvG::Button* btnDialogOK = new RvG::Button(doTranslate("do.accounts.sure"), 516, 157, 100, 40, dialog);
	RvG::Button* btnDialogCan = new RvG::Button(doTranslate("sel.cancel"), 622, 157, 100, 40, dialog);
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
		SendMessage(*lisAccountsList, LB_RESETCONTENT, 0, 0);
		for (Json::Value i : accounts) {
			strcpy(tempA, i["userName"].asCString());
			if (i["userType"].asInt() == 0) strcat(tempA, " (Offline)");
			if (i["userType"].asInt() == 1) strcat(tempA, " (Official)");
			lisAccountsList->add(tempA);
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
		RvG::Window* dialog = new RvG::Window(doTranslate("dialog"), 1, CW_USEDEFAULT, CW_USEDEFAULT, 750, 250);
		char temp[256] = {};
		wchar_t t2[256] = {};
		sprintf(temp, doTranslate("prompt.accounts.remove"), accounts[intAccountsSel]["userName"].asCString());
		MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, temp, strlen(temp), t2, 256);
		new RvG::Label(t2, 10, 10, 512, 20, dialog);
		RvG::Button* btnDialogOK = new RvG::Button(doTranslate("sel.yes"), 516, 157, 100, 40, dialog);
		RvG::Button* btnDialogCan = new RvG::Button(doTranslate("sel.no"), 622, 157, 100, 40, dialog);
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
			if (accounts.size() != 0) {
				SendMessage(*lisAccountsList, LB_RESETCONTENT, 0, 0);
				for (Json::Value i : accounts) {
					strcpy(tempA, i["userName"].asCString());
					if (i["userType"].asInt() == 0) strcat(tempA, " (Offline)");
					if (i["userType"].asInt() == 1) strcat(tempA, " (Official)");
					lisAccountsList->add(tempA);
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