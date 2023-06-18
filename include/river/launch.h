#pragma once

#include <river/defines.h>

int launchInstance(const char* versionId, const char* dir, RvG::Window* x) {
	
	// Prepare

	pgbProgress->set(0);
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
		MessageBoxA(*x, doTranslate("prompt.accounts.no"), doTranslate("error"), MB_OK | MB_ICONERROR);
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
	char libDir[514];
	char tmpS[1026];
	char* tmpC = NULL;
	char* tmpC2 = NULL;
	int tmpI;
	char output[6400];
	if (output == nullptr) {
		return 1;
	}
	pgbProgress->add(10);

	// Libraries

	char nativeLatestA[256] = {};
	strcpyf(nativeLatestA, "%sversions\\%s\\%s-natives\\", cwd, versionId, versionId);
	wchar_t nativeLatestW[256] = {};
	MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, nativeLatestA, strlen(nativeLatestA), nativeLatestW, 256);
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
				join(libNameSp, libDir, 512, "\\");
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
		for (int j = 0; j < strlen(libDir); j++) {
			if (libDir[j] == '/') libDir[j] = '\\';
		}
		if (i.isMember("natives")) {
			char nativeTmp[256] = {};
			strcpyf(nativeTmp, "%slibraries\\%s", cwd, versionLib["path"].asCString());
			for (int i = 0; i < 256; i++) {
				if (nativeTmp[i] == 0) break;
				if (nativeTmp[i] == '/') nativeTmp[i] = '\\';
			}
			wchar_t nativeTmpW[256] = {};
			MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, nativeTmp, strlen(nativeTmp), nativeTmpW, 256);
			HZIP hZip = OpenZip(nativeTmpW, NULL);
			ZIPENTRY ze;
			GetZipItem(hZip, -1, &ze);
			int nums = ze.index;
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
	writeLog("LaunchInstance", tmpC);
	pgbProgress->add(40);

	// Get Launch Level

	bool level;
	if (versionInfo.isMember("arguments")) level = 1;
	else if (versionInfo.isMember("minecraftArguments")) level = 0;
	else {
		writeLog("LaunchInstance", "Unknow launch level. ");
		free(tmpC);
		MessageBoxA(*x, doTranslate("prompt.mcje.notlaunch"), doTranslate("error"), MB_OK | MB_ICONERROR);
		return 0;
	}
	libraries.empty();
	tmp.empty();
	versionLib.empty();
	libNameSp.empty();
	pgbProgress->add(10);

	Json::Value gameArg;
	char* gameArgC = NULL;
	Json::Value jvmArg;
	char* jvmArgC = NULL;
	int gameArgCLen = 1600;
	if (level == 0) {
		gameArgC = (char*)malloc(1602);
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
				tmp = i["rules"][0]["features"];
				if (!i.isMember("has_custom_resolution")) continue;
				tmp = i["value"];
				if (tmp.isString()) {
					gameArg.append(tmp);
				}
				else {
					for (Json::Value j : tmp) {
						if (find(j.asCString(), " ") != -1) {
							strcpyf(tmpS, "\"%s\"", j.asCString());
							gameArg.append(tmpS);
						}
						else gameArg.append(j);
					}
				}
			}
		}
		gameArgC = (char*)malloc(1602);
		join(gameArg, gameArgC, 1600, " ");
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
	int javaVersion = versionInfo["javaVersion"]["majorVersion"].asInt();
	if (javaVersion > 11) javaVersion = 17;
	else if (javaVersion == 11) javaVersion = 8;
	else if (javaVersion <= 11) javaVersion = 8;
	else javaVersion = 17;
	char gottenJavaVersion[512];
	pgbProgress->add(10);

	// Get Javas

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
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	strcpy(output, "cmd /c where java");
	if (!CreateProcessA(NULL, output, NULL, NULL, TRUE, NULL,
		NULL, cwd, &si, &pi)) {
		DWORD ret = GetLastError();
		CloseHandle(hRead);
		CloseHandle(hWrite);
		return ret ? ret : -1;
	}

	CloseHandle(hWrite);
	Json::Value javas = Json::arrayValue;
	char javasTemp[4098] = {};
	char bufJava[512] = {};
	DWORD bytesRead;
	while (ReadFile(hRead, (char*)bufJava, 512, &bytesRead, NULL)) {
		strcat(javasTemp, bufJava);
		memset(bufJava, 0, MAX_PATH + 1);
	}
	valueSplit(javasTemp, "\r\n", &javas);
	javas.removeIndex(javas.size() - 1, NULL);

	CloseHandle(hRead);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	// For Each to Get the Most Compatible Java

	for (Json::Value java : javas) {

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
		si.wShowWindow = SW_HIDE;
		si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

		strcpyf(output, "%s getJavaMainVersion", java.asCString());
		if (!CreateProcessA(NULL, output, NULL, NULL, TRUE, NULL,
			NULL, "RvL\\", &si, &pi)) {
			MessageBoxA(*x, java.asCString(), "?", MB_OK | MB_ICONERROR);
			DWORD ret = GetLastError();
			CloseHandle(hRead);
			CloseHandle(hWrite);
			return ret ? ret : -1;
		}

		CloseHandle(hWrite);
		char tmpEachJava[130] = {};
		char bufEachJava[130] = {};
		Json::Value javaInfo;
		DWORD bytesRead;
		while (ReadFile(hRead, tmpEachJava, 128, &bytesRead, NULL)) {
			strcat(bufEachJava, tmpEachJava);
			memset(tmpEachJava, 0, 130);
		}
		valueSplit(bufEachJava, "\r\n", &javaInfo);

		CloseHandle(hRead);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		if (strcmp(javaInfo[1].asCString(), "64") != 0) continue;
		writeLog(output, javaInfo[0].asCString());
		int tar = 0;
		if (strcmp(javaInfo[0].asCString(), "1.8") == 0) tar = 8;
		else tar = atoi(javaInfo[0].asCString());
		if (tar == javaVersion) {
			strcpy(gottenJavaVersion, java.asCString());
			int len = strlen(gottenJavaVersion);
			gottenJavaVersion[len + 1] = 0;
			gottenJavaVersion[len] = 'e';
			gottenJavaVersion[len - 1] = 'x';
			gottenJavaVersion[len - 2] = 'e';
			gottenJavaVersion[len - 3] = '.';
			gottenJavaVersion[len - 4] = 'w';
			break;
		}
	}
	pgbProgress->add(20);

	// Write into output

	strcpyf(output, "\"%s\" -Dminecraft.client.jar=", gottenJavaVersion);
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
			strcpyf(loggingTmp3, "%sversions\\%s\\%s", cwd, versionId, versionInfo["logging"]["client"]["file"]["id"].asCString());
			replace(256, loggingTmp, loggingTmp2, "${path}", loggingTmp3);
			strcatf(output, " %s", loggingTmp2);
		}
	}
	char* gameArgCReplaced = (char*)malloc(gameArgCLen+2);
	if (find(gameArgC, " --tweakClass optifine.OptiFineForgeTweaker") != -1) {
		replace(gameArgCLen, gameArgC, gameArgCReplaced, " --tweakClass optifine.OptiFineForgeTweaker", "");
		strcpyf(gameArgC, "%s --tweakClass optifine.OptiFineForgeTweaker", gameArgCReplaced);
	}
	replace(gameArgCLen, gameArgC, gameArgCReplaced, "${auth_player_name}", accounts[intAccountsSel]["userName"].asCString());
	replace(gameArgCLen, gameArgCReplaced, gameArgC, "${version_name}", versionId);
	replace(gameArgCLen, gameArgC, gameArgCReplaced, "${game_directory}", cwd);
	replace(gameArgCLen, gameArgCReplaced, gameArgC, "${assets_root}", "assets\\");
	replace(gameArgCLen, gameArgC, gameArgCReplaced, "${assets_index_name}", versionInfo["assets"].asCString());
	replace(gameArgCLen, gameArgCReplaced, gameArgC, "${auth_uuid}", accounts[intAccountsSel]["userId"].asCString());
	replace(gameArgCLen, gameArgC, gameArgCReplaced, "${auth_access_token}", accounts[intAccountsSel]["userToken"].asCString());
	replace(gameArgCLen, gameArgCReplaced, gameArgC, "${auth_session}", accounts[intAccountsSel]["userToken"].asCString());
	replace(gameArgCLen, gameArgC, gameArgCReplaced, "${user_type}", (accounts[intAccountsSel]["usrType"].asInt() == 0)? ("legacy") : ("mojang"));
	replace(gameArgCLen, gameArgCReplaced, gameArgC, "${clientId}", accounts[intAccountsSel]["userId"].asCString());
	replace(gameArgCLen, gameArgC, gameArgCReplaced, "${version_type}", versionInfo["type"].asCString());
	int wid = intSettingsWid, hei = intSettingsHei;
	char temp[20];
	replace(gameArgCLen, gameArgCReplaced, gameArgC, "${resolution_width}", itoa(wid, temp, 10));
	replace(gameArgCLen, gameArgC, gameArgCReplaced, "${resolution_height}", itoa(hei, temp, 10));
	char latest[128];
	strcpyf(latest, "versions\\%s\\%s-natives\\", versionId, versionId);
	replace(gameArgCLen, gameArgCReplaced, gameArgC, "${natives_directory}", latest);
	replace(gameArgCLen, gameArgCReplaced, gameArgC, "${user_properties}", "{}");
	replace(gameArgCLen, gameArgC, gameArgCReplaced, "${client_id}", "00000000402b5328");
	replace(gameArgCLen, gameArgCReplaced, gameArgC, "${classpath_separator}", ";");
	replace(gameArgCLen, gameArgC, gameArgCReplaced, "${library_directory}", "libraries\\");
	char* jvmArgCReplaced = (char*)malloc(6400);
	replace(6400, jvmArgC, jvmArgCReplaced, "${classpath}", tmpC);
	replace(6400, jvmArgCReplaced, jvmArgC, "${natives_directory}", latest);
	replace(6400, jvmArgC, jvmArgCReplaced, "${launcher_name}", "RiverLauncher2");
	replace(6400, jvmArgCReplaced, jvmArgC, "${launcher_version}", "2.0.0.0");
	free(jvmArgCReplaced);
	strcatf(output, " %s %s %s", jvmArgC, versionInfo["mainClass"].asCString(), gameArgCReplaced);
	if (find(gameArgC, "--width") == -1) {
		strcatf(output, " --width %d --height %d", wid, hei);
	}

	// Clean

	free(tmpC);
	free(gameArgC);
	free(gameArgCReplaced);
	free(jvmArgC);
	int f = 0;
	DWORD rec;
	pgbProgress->add(10);

	// Launch

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

	strcpyf(tmpS, "%sversions\\%s\\RiverLaunch.bat", cwd, versionId);
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
	pgbProgress->set(100);

	// Create dia'log'

	char buf[4098];
	RvG::Label* edi;
	thread thr2([&, buf] {
		minecraftLog = new RvG::Window(doTranslate("prompt.mcje.log"), 1, CW_USEDEFAULT, CW_USEDEFAULT, 650, 450);
		edi = new RvG::Label(doTranslate("prompt.mcje.loghere"), 25, 25, 600, 400, minecraftLog, WS_BORDER);
		minecraftLog->keepResponding();
		minecraftLog = nullptr;
		});
	thr2.detach();
	thread thr([&, buf] {
		while (ReadFile(hRead, (char*)buf, 4096, &bytesRead, NULL)) {
			if (minecraftLog != nullptr) {
				SetWindowTextA(edi->hWnd, buf);
				UpdateWindow(minecraftLog->hWnd);
			}
		}
		if (minecraftLog != nullptr) {
			SetWindowTextA(edi->hWnd, doTranslate("prompt.mcje.processended"));
			UpdateWindow(minecraftLog->hWnd);
		}
		CloseHandle(hRead);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	});
	thr.detach();
	return 0;
}