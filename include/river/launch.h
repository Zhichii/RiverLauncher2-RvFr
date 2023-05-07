#include <river/defines.h>

int launchInstance(const char* versionId, const char* dir, HWND edit, RvG::Edit* edi, RvG::Window* x) {
	
	// Prepare

	struct _stat fileStat;
	if ((_stat(dir, &fileStat) == 0) && (fileStat.st_mode & _S_IFDIR)) {}
	else return 1;
	if (accounts.size() == 0) {
		MessageBox(edit, L"No accounts created! ", L"Prompt", MB_OK | MB_ICONINFORMATION);
	}
	char cwd[512];
	strcpy(cwd, dir);
	strcat(cwd, "\\");
	char fvJson[512]; // Full Version Json
	strcpy(fvJson, cwd);
	strcat(fvJson, "versions\\");
	strcat(fvJson, versionId);
	strcat(fvJson, "\\");
	strcat(fvJson, versionId);
	strcat(fvJson, ".json");
	if ((_stat(fvJson, &fileStat) == 0)) {}
	else return 1;

	ifstream file(fvJson);
	Json::Value versionInfo;
	reader.parse(file, versionInfo);
	int mark = 0;
	Json::Value available;
	Json::Value libraries = versionInfo["libraries"];
	Json::Value tmp;
	char theVersion[514];
	Json::Value versionLib;
	Json::Value libNameSp;
	char libDir[514];
	char tmpS[514];
	char* tmpC = NULL;
	char* tmpC2 = NULL;
	int tmpI;
	strcpyf(tmpS, "%sversions\\%s\\river_launch.bat", cwd, versionId);
	FILE* output = fopen(tmpS, "w");
	if (output == nullptr) {
		return 1;
	}

	// Libraries

	for (Json::Value i : libraries) {
		valueSplit(i["name"].asCString(), ":", &tmp);
		tmp[2] = "";
		join(tmp, theVersion, 512, ":");
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
		for (int i = 0; i < strlen(libDir); i++) {
			if (libDir[i] == '/') libDir[i] = '\\';
		}
		if (i.isMember("natives")) {
			//* Extract Native Libraries
			continue;
		}
		available[theVersion] = libDir;
	}
	tmp.clear();
	for (Json::Value i : available) {
		tmp.append(i.asCString());
	}
	available.empty();
	char fvJar[512];
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

	// Write into the BAT file
		
	strcpyf(tmpS, "%sversions\\%s\\river_launch.bat", cwd, versionId);

	fprintf(output, "@echo off\ntitle Minecraft Log\ncd /d ");
	fprintf(output, cwd);
	fprintf(output, "\n");
	fprintf(output, "java -Dminecraft.client.jar=");
	fprintf(output, "versions\\%s\\%s.jar", versionId, versionId);
	if (find(jvmArgC, "-Djava.library.path") == -1) {
		fprintf(output, " -Djava.library.path=%sversions\\%s\\%s-natives\\", cwd, versionId, versionId);
	}
	char loggingTmp[256];
	char loggingTmp2[256];
	char loggingTmp3[256];
	if (versionInfo.isMember("logging")) {
		if (versionInfo["logging"].isMember("client")) {
			strcpy(loggingTmp, versionInfo["logging"]["client"]["argument"].asCString());
			strcpyf(loggingTmp3, "%sversions\\%s\\", cwd, versionId, versionInfo["logging"]["client"]["file"]["id"].asCString());
			replace(256, loggingTmp, loggingTmp2, "${path}", loggingTmp3);
			fprintf(output, " %s", loggingTmp2);
		}
	}
	char* gameArgCReplaced = (char*)malloc(6402);
	if (find(gameArgC, " --tweakClass optifine.OptiFineForgeTweaker") != -1) {
		replace(6400, gameArgC, gameArgCReplaced, " --tweakClass optifine.OptiFineForgeTweaker", "");
		strcpyf(gameArgC, "%s --tweakClass optifine.OptiFineForgeTweaker", gameArgCReplaced);
	}
	replace(6400, gameArgC, gameArgCReplaced, "${auth_player_name}", "HillQiu");
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
	replace(6400, gameArgCReplaced, gameArgC, "${resolution_width}", "1618"); //* Hey
	replace(6400, gameArgC, gameArgCReplaced, "${resolution_height}", "1000"); //* Hey
	char latest[256];
	strcpyf(latest, "versions\\%s\\%s-natives\\", versionId, versionId);
	replace(6400, gameArgCReplaced, gameArgC, "${natives_directory}", latest);
	replace(6400, gameArgCReplaced, gameArgC, "${user_properties}", "{}");
	replace(6400, gameArgC, gameArgCReplaced, "${client_id}", "00000000402b5328");
	replace(6400, gameArgCReplaced, gameArgC, "${classpath_separator}", ";");
	replace(6400, gameArgC, gameArgCReplaced, "${library_directory}", "libraries\\");
	char* jvmArgCReplaced = (char*)malloc(6402);
	replace(6400, jvmArgC, jvmArgCReplaced, "${classpath}", tmpC);
	free(jvmArgC);
	fprintf(output, " %s %s %s", jvmArgCReplaced, versionInfo["mainClass"].asCString(), gameArgCReplaced);
	if (find(gameArgC, "--width") == -1) {
		fprintf(output, " --width %d --height %d", 1618, 1000);
	}

	// Clean

	fclose(output);
	free(tmpC);
	free(gameArgC);
	free(jvmArgCReplaced);
	free(gameArgCReplaced);
	char tmpS2[514];
	strcpyf(tmpS2, "cmd /k %s", tmpS);
	int f = 0;
	thread thr([=]() {
		SECURITY_ATTRIBUTES sa;
		HANDLE hRead, hWrite;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;

		if (!CreatePipe(&hRead, &hWrite, &sa, 0)) {
			DWORD ret = GetLastError();
			return ret ? ret : -1;
		}

		STARTUPINFOA si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(STARTUPINFO));

		si.cb = sizeof(STARTUPINFO);
		GetStartupInfoA(&si);
		si.hStdError = hWrite;
		si.hStdOutput = hWrite;
		si.wShowWindow = SW_HIDE;
		si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

		if (!CreateProcessA(NULL, (char*)tmpS, NULL, NULL, TRUE, NULL,
			NULL, NULL, &si, &pi)) {
			DWORD ret = GetLastError();
			CloseHandle(hRead);
			CloseHandle(hWrite);
			return ret ? ret : -1;
		}

		CloseHandle(hWrite);
		char buf[4098];
		DWORD bytesRead;
		while (ReadFile(hRead, buf, 4096, &bytesRead, NULL)) {
			SetWindowTextA(edi->hWnd, buf);
			Sleep(100);
		}

		DWORD exitCode = 0;
		GetExitCodeProcess(pi.hProcess, &exitCode);
		CloseHandle(hRead);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	});
	thr.detach();
	return 0;
}