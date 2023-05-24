#pragma once

#include <river/defines.h>

int launchInstance(const char* versionId, const char* dir, HWND edit, RvG::Label* edi, RvG::Window* x) {
	
	// Prepare

	struct _stat fileStat;
	if ((_stat(dir, &fileStat) == 0) && (fileStat.st_mode & _S_IFDIR)) {}
	else return 1;
	char cwd[1024];
	strcpy(cwd, dir);
	strcat(cwd, "\\");
	char fvJson[1024]; // Full Version Json
	strcpy(fvJson, cwd);
	strcat(fvJson, "versions\\");
	strcat(fvJson, versionId);
	strcat(fvJson, "\\");
	strcat(fvJson, versionId);
	strcat(fvJson, ".json");
	if ((_stat(fvJson, &fileStat) == 0)) {}
	else return 1;
	if (accounts.size() == 0) {
		MessageBox(edit, L"No accounts created! ", L"Error", MB_OK | MB_ICONERROR);
		return 0;
	}

	ifstream file(fvJson);
	Json::Value versionInfo;
	reader.parse(file, versionInfo);
	int mark = 0;
	Json::Value available;
	Json::Value libraries = versionInfo["libraries"];
	Json::Value tmp;
	char theVersion[1026];
	Json::Value versionLib;
	Json::Value libNameSp;
	char libDir[1026];
	char tmpS[1026];
	char* tmpC = NULL;
	char* tmpC2 = NULL;
	int tmpI;
	char output[6400];
	if (output == nullptr) {
		return 1;
	}

	// Libraries

	for (Json::Value i : libraries) {
		valueSplit(i["name"].asCString(), ":", &tmp);
		tmp[2] = "";
		join(tmp, theVersion, 1024, ":");
		mark = 0;
		if (i["downloads"].isMember("artifact")) {
			versionLib = i["downloads"]["artifact"];
			mark = 1;
		}
		if (i["downloads"].isMember("classifiers")) {
			if (i["downloads"]["classifiers"].isMember("natives-windows")) {
				versionLib = i["downloads"]["classifiers"]["natives-windows"];
				mark = 1;
			}
		}
		else if (mark == 0) {
			if (i.isMember("name")) {
				valueSplit(i["name"].asCString(), ":", &libNameSp);
				tmpC = (char*)malloc(strlen(libNameSp[0].asCString()) + 2);
				strcpy(tmpC, libNameSp[0].asCString());
				for (int i = 0; i < strlen(tmpC); i++) {
					if (tmpC[i] == '.') tmpC[i] = '\\';
				}
				libNameSp[0] = tmpC;
				free(tmpC);
				tmpC = NULL;
				join(libNameSp, libDir, 1024, "\\");
				strcpyf(tmpS, "libraries\\%s\\%s-%s.jar", libDir, libNameSp[1].asCString(), libNameSp[2].asCString());
				for (int i = 0; i < strlen(libDir); i++) {
					if (libDir[i] == '/') libDir[i] = '\\';
				}
				available[theVersion] = tmpS;
			}
			continue;
		}
		if (i.isMember("rules")) {
			tmpI = 0;
			for (Json::Value l : i["rules"]) {
				if (strcmp(l["action"].asCString(), "allow") == 0) {
					if (l.isMember("os")) {
						if (strcmp(l["os"]["name"].asCString(), "windows") == 0) {
							tmpI = 1;
						}
					}
					else tmpI = 1;
				}
				if (strcmp(l["action"].asCString(), "disallow") == 0) {
					if (l.isMember("os")) {
						if (strcmp(l["os"]["name"].asCString(), "windows") == 0) {
							tmpI = 0;
						}
					}
					else tmpI = 0;
				}
			}
			if (tmpI == 0) continue;	
		}
		strcpyf(libDir, "libraries\\%s", versionLib["path"].asCString());
		for (int i = 0; i < strlen(libDir); i++) {
			if (libDir[i] == '/') libDir[i] = '\\';
		}
		if (i.isMember("natives")) {
			char nativeTmp[1026] = {};
			strcpyf(nativeTmp, "%slibraries\\%s", cwd, versionLib["path"].asCString());
			for (int i = 0; i < 1024; i++) {
				if (nativeTmp[i] == 0) break;
				if (nativeTmp[i] == '/') nativeTmp[i] = '\\';
			}
			wchar_t nativeTmpW[1026] = {};
			MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, nativeTmp, strlen(nativeTmp), nativeTmpW, 1024);
			HZIP hZip = OpenZip(nativeTmpW, NULL);
			ZIPENTRY ze;
			GetZipItem(hZip, -1, &ze);
			int nums = ze.index;
			char nativeLatestA[1026] = {};
			strcpyf(nativeLatestA, "%sversions\\%s\\%s-natives\\", cwd, versionId, versionId);
			wchar_t nativeLatestW[1026] = {};
			MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, nativeLatestA, strlen(nativeLatestA), nativeLatestW, 1024);
			SetUnzipBaseDir(hZip, nativeLatestW);
			for (int i = 0; i < nums; i++) {
				GetZipItem(hZip, i, &ze);
				int len = lstrlenW(ze.name);
				if (ze.name[len - 1] == L'l' && ze.name[len - 2] == L'l' && ze.name[len - 3] == L'd' && ze.name[len - 4] == L'.')
					UnzipItem(hZip, i, ze.name);
			}
			CloseZip(hZip);
			continue;
		}
		available[theVersion] = libDir;
	}
	tmp.clear();
	for (Json::Value i : available) {
		tmp.append(i.asCString());
	}
	available.empty();
	char fvJar[1024];
	strcpy(fvJar, fvJson);
	strcpy(fvJar + strlen(fvJson) - 5, ".jar\0");
	tmp.append(fvJar);
	tmpC = (char*)malloc(6402);
	join(tmp, tmpC, 6400, ";");
	writeLog("launchInstance", tmpC);

	// Get Launch Level

	bool level;
	if (versionInfo.isMember("arguments")) level = 1;
	else if (versionInfo.isMember("minecraftArguments")) level = 0;
	else {
		writeLog("launchInstance", "Unknow launch level, default to 0. ");
		level = 0;
	}
	libraries.empty();
	tmp.empty();
	versionLib.empty();
	libNameSp.empty();

	Json::Value gameArg;
	char* gameArgC = NULL;
	Json::Value jvmArg;
	char* jvmArgC = NULL;
	if (level == 0) {
		gameArgC = (char*)malloc(strlen(versionInfo["minecraftArguments"].asCString())+2);
		strcpy(gameArgC, versionInfo["minecraftArguments"].asCString());
		jvmArg.clear();
		jvmArg.append("-cp");
		jvmArg.append(tmpC);
	}
	if (level == 1) {
		gameArg.clear();
		gameArg = Json::arrayValue;
		for (Json::Value i : versionInfo["arguments"]["game"]) {
			if (i.isString()) {
				if (find(i.asCString(), " ") != -1) {
					strcpyf(tmpS, "\"%s\"", i.asCString());
					gameArg.append(tmpS);
				}
				else gameArg.append(i);
			}
			else {
				tmp = i["value"];
				for (Json::Value j : tmp) {
					if (find(j.asCString(), " ") != -1) {
						strcpyf(tmpS, "\"%s\"", j.asCString());
						gameArg.append(tmpS);
					}
					else gameArg.append(j);
				}
			}
		}
		gameArgC = (char*)malloc(6402);
		join(gameArg, gameArgC, 6400, " ");
		jvmArg.clear();
		for (Json::Value i : versionInfo["arguments"]["jvm"]) {
			if (i.isString()) {
				if (find(i.asCString(), " ") != -1) {
					strcpyf(tmpS, "\"%s\"", i.asCString());
					jvmArg.append(tmpS);
				}
				else jvmArg.append(i);
			}
			else {
				tmp = i["value"];
				tmpI = 0;
				for (Json::Value j : i["rules"]) {
					if (strcmp(j["action"].asCString(), "allow") == 0) {
						if (j.isMember("os")) {
							if (!j["os"].isMember("name")) continue;
							if (strcmp(j["os"]["name"].asCString(), "windows") == 0) {
								tmpI = 1;
							}
						}
						else tmpI = 1;
					}
					if (strcmp(j["action"].asCString(), "disallow") == 0) {
						if (j.isMember("os")) {
							if (strcmp(j["os"]["name"].asCString(), "windows") == 0) {
								tmpI = 0;
							}
						}
						else tmpI = 0;
					}
				}
				if (tmpI == 0) continue;
				if (tmp.isString()) {
					if (find(tmp.asCString(), " ") != -1) {
						strcpyf(tmpS, "\"%s\"", tmp.asCString());
						jvmArg.append(tmpS);
					}
					else jvmArg.append(tmp);
				}
				else {
					for (Json::Value j : tmp) {
						if (find(j.asCString(), " ") != -1) {
							strcpyf(tmpS, "\"%s\"", j.asCString());
							jvmArg.append(tmpS);
						}
						else jvmArg.append(j);
					}
				}
			}
		}
	}
	jvmArgC = (char*)malloc(6402);
	join(jvmArg, jvmArgC, 6400, " ");

	// Get Javas //* Skipped

	// Write into output

	strcpy(output, "javaw -Dminecraft.client.jar=");
	strcatf(output, "versions\\%s\\%s.jar", versionId, versionId);
	if (find(jvmArgC, "-Djava.library.path") == -1) {
		strcatf(output, " -Djava.library.path=%sversions\\%s\\%s-natives\\", cwd, versionId, versionId);
	}
	char loggingTmp[256];
	char loggingTmp2[256];
	char loggingTmp3[256];
	if (versionInfo.isMember("logging")) {
		if (versionInfo["logging"].isMember("client")) {
			strcpy(loggingTmp, versionInfo["logging"]["client"]["argument"].asCString());
			strcpyf(loggingTmp3, "%sversions\\%s\\", cwd, versionId, versionInfo["logging"]["client"]["file"]["id"].asCString());
			replace(256, loggingTmp, loggingTmp2, "${path}", loggingTmp3);
			strcatf(output, " %s", loggingTmp2);
		}
	}
	char* gameArgCReplaced = (char*)malloc(6402);
	if (find(gameArgC, " --tweakClass optifine.OptiFineForgeTweaker") != -1) {
		replace(6400, gameArgC, gameArgCReplaced, " --tweakClass optifine.OptiFineForgeTweaker", "");
		strcpyf(gameArgC, "%s --tweakClass optifine.OptiFineForgeTweaker", gameArgCReplaced);
	}
	replace(6400, gameArgC, gameArgCReplaced, "${auth_player_name}", accounts[intAccountsSel]["userName"].asCString());
	replace(6400, gameArgCReplaced, gameArgC, "${version_name}", versionId);
	replace(6400, gameArgC, gameArgCReplaced, "${game_directory}", cwd);
	replace(6400, gameArgCReplaced, gameArgC, "${assets_root}", "assets\\");
	replace(6400, gameArgC, gameArgCReplaced, "${assets_index_name}", versionInfo["assets"].asCString());
	replace(6400, gameArgCReplaced, gameArgC, "${auth_uuid}", "${auth_uuid}"); //* Hey
	replace(6400, gameArgC, gameArgCReplaced, "${auth_access_token}", "${auth_access_token}"); //* Hey
	replace(6400, gameArgCReplaced, gameArgC, "${auth_session}", "${auth_session}"); //* Hey
	replace(6400, gameArgC, gameArgCReplaced, "${user_type}", "legacy"); //* Hey
	replace(6400, gameArgCReplaced, gameArgC, "${clientId}", "${clientId}"); //* Hey
	replace(6400, gameArgC, gameArgCReplaced, "${version_type}", versionInfo["type"].asCString());
	int wid, hei;
	RegGetValueA(hData, NULL, "WindowWidth", RRF_RT_REG_DWORD, NULL, &wid, &sz);
	RegGetValueA(hData, NULL, "WindowHeight", RRF_RT_REG_DWORD, NULL, &hei, &sz);
	char temp[20];
	replace(6400, gameArgCReplaced, gameArgC, "${resolution_width}", itoa(wid, temp, 10));
	replace(6400, gameArgC, gameArgCReplaced, "${resolution_height}", itoa(hei, temp, 10));
	char latest[256];
	strcpyf(latest, "versions\\%s\\%s-natives\\", versionId, versionId);
	replace(6400, gameArgCReplaced, gameArgC, "${natives_directory}", latest);
	replace(6400, gameArgCReplaced, gameArgC, "${user_properties}", "{}");
	replace(6400, gameArgC, gameArgCReplaced, "${client_id}", "00000000402b5328");
	replace(6400, gameArgCReplaced, gameArgC, "${classpath_separator}", ";");
	replace(6400, gameArgC, gameArgCReplaced, "${library_directory}", "libraries\\");
	char* jvmArgCReplaced = (char*)malloc(6402);
	replace(6400, jvmArgC, jvmArgCReplaced, "${classpath}", tmpC);
	replace(6400, jvmArgCReplaced, jvmArgC, "${natives_directory}", latest);
	free(jvmArgCReplaced);
	strcatf(output, " %s %s %s", jvmArgC, versionInfo["mainClass"].asCString(), gameArgCReplaced);
	if (find(gameArgC, "--width") == -1) {
		strcatf(output, " --width %d --height %d", wid, hei);
	}

	// Clean

	free(tmpC);
	free(gameArgC);
	free(jvmArgC);
	free(gameArgCReplaced);
	int f = 0;

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
	si.wShowWindow = SW_SHOW;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	strcpyf(tmpS, "%sversions\\%s\\river_launch.bat", cwd, versionId);
	FILE* outFile = fopen(tmpS, "w");
	fprintf(outFile, "@echo off\ntitle Minecraft Log\ncd /d %s\n%s", cwd, output);
	fclose(outFile);
	if (!CreateProcessA(NULL, output, NULL, NULL, TRUE, NULL,
		NULL, cwd, &si, &pi)) {
		DWORD ret = GetLastError();
		CloseHandle(hRead);
		CloseHandle(hWrite);
		return ret ? ret : -1;
	}

	CloseHandle(hWrite);
	char buf[4098];
	DWORD bytesRead;
	thread thr([&]()->int {
		while (ReadFile(hRead, (char*)buf, 10240, (LPDWORD) &bytesRead, NULL)) {
			SetWindowTextA(edi->hWnd, buf);
		}
		char temp[10242];
		strcpyf(temp, "Process ended! ");
		SetWindowTextA(edi->hWnd, temp);
		DWORD exitCode = 0;
		GetExitCodeProcess(pi.hProcess, &exitCode);
		CloseHandle(hRead);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		return 0;
	});
	thr.detach();
	return 0;
}