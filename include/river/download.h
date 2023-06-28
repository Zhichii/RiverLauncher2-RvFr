#pragma once
#include <river/defines.h>

DWORD WINAPI downloadAsset(LPVOID lpParam) {
	try {
		//char* temp;
		const char* tmpS2 = "https://resources.download.minecraft.net";
		Json::Value* i = ((Json::Value*)lpParam);
		char tmpS3[512];
		//int size;
		strcpyf(tmpS3, "%s/%c%c/%s", tmpS2, (*i)["hash"].asCString()[0], (*i)["hash"].asCString()[1], (*i)["hash"].asCString());
		//winGetHttps(&temp, tmpS3, &size);
		fprintf(programmeLog, "...  %s \n", tmpS3);
		Response resp = Get(tmpS3);
		GetWindowTextA(*ediSettingsDir, tmpS3, 512);
		strcatf(tmpS3, "\\assets\\objects\\%c%c\\%s", (*i)["hash"].asCString()[0], (*i)["hash"].asCString()[1], (*i)["hash"].asCString());
		MakeSureDirectoryPathExists(tmpS3);
		FILE* fp = fopen(tmpS3, "wb");
		fwrite(resp.GetBinary(), 1, resp.size(), fp);
		fclose(fp);
		mtx.lock();
		progress += 1;
		mtx.unlock();
		pgbProgress->set((int)progress);
	}
	catch (const char* e) {
		fprintf(programmeLog, "...  %s \n", e);
		ExitProcess(0);
	}
	return 0;
}

int download(HWND win, HWND btn) {
	thread thr([=]()->int {
		pgbProgress->set(0);
		char versionId[256] = {};
		int n = lisMinecraftDownloads->getSelIndex();
		if (n == -1) {
			MessageBoxA(win, doTranslate("prompt.mcje.download.unable"), doTranslate("error", 1), MB_OK | MB_ICONERROR);
			return 1;
		}
		lisMinecraftDownloads->getText(lisMinecraftDownloads->getSelIndex(), versionId);
		char tmpS[512];
		GetWindowTextA(*ediSettingsDir, tmpS, 512);
		strcatf(tmpS, "\\versions\\%s", versionId);
		//MessageBoxA(win, tmpS, "Test", MB_OK | MB_ICONINFORMATION);
		int index = lisMinecraftDownloads->getSelIndex();

		// Get Json

		Response resp = Get(versionManifest["versions"][index]["url"].asCString());
		char tmpS2[512];
		strcpyf(tmpS2, "%s\\%s.json", tmpS, versionId);
		MakeSureDirectoryPathExists(tmpS2);
		Json::Value tempValue;
		reader.parse(resp.GetText(), tempValue);
		Json::StyledWriter sw;
		FILE* fp = fopen(tmpS2, "w");
		Json::String s = sw.write(tempValue);
		fwrite(s.c_str(), 1, s.size(), fp);
		fclose(fp);
		pgbProgress->add(5);

		// Get Libraries
		char tmpS3[512];
		GetWindowTextA(*ediSettingsDir, tmpS3, 512);
		double step = 30.0 / tempValue["libraries"].size();
		progress = 5;

		int size;
		for (int i = 0; i < tempValue["libraries"].size(); i++) {
			try {
				char* tempChars;
				if (tempValue["libraries"][i]["downloads"].isMember("artifact")) {
					strcpyf(tmpS2, "%s\\libraries\\%s", tmpS3, tempValue["libraries"][i]["downloads"]["artifact"]["path"].asCString());
					for (int j = 0; j < 512; j++) {
						if (tmpS2[j] == '/') tmpS2[j] = '\\';
					}
					int size;
					winGetHttps(&tempChars, tempValue["libraries"][i]["downloads"]["artifact"]["url"].asCString(), &size);
					MakeSureDirectoryPathExists(tmpS2);
				}
				if (tempValue["libraries"][i]["downloads"].isMember("classifiers") &&
					tempValue["libraries"][i]["downloads"]["classifiers"].isMember("natives-windows")) {
					strcpyf(tmpS2, "%s\\libraries\\%s", tmpS3, tempValue["libraries"][i]["downloads"]["classifiers"]["natives-windows"]["path"].asCString());
					for (int j = 0; j < 512; j++) {
						if (tmpS2[j] == '/') tmpS2[j] = '\\';
					}
					winGetHttps(&tempChars, tempValue["libraries"][i]["downloads"]["classifiers"]["natives-windows"]["url"].asCString(), &size);
					MakeSureDirectoryPathExists(tmpS2);
				}
				FILE* fp = fopen(tmpS2, "wb");
				fwrite(tempChars, 1, size, fp);
				fclose(fp);
				free(tempChars);
			}
			catch (const char* msg) {
				DebugBreak();
			};
			progress += step;
			pgbProgress->set((int)progress);
			UpdateWindow(*pgbProgress);
		}
		for (pair<string, HINTERNET> hConnectPair : hosts) {
			InternetCloseHandle(hConnectPair.second);
		}
		InternetCloseHandle(hInternet);
		pgbProgress->set(35);

		// Get Game Jar
		Json::Value jar = tempValue["downloads"]["client"];
		resp = Get(jar["url"].asCString());
		strcpyf(tmpS2, "%s\\%s.jar", tmpS, versionId);
		fp = fopen(tmpS2, "wb");
		fwrite(resp.GetBinary(), 1, resp.size(), fp);
		fclose(fp);
		pgbProgress->add(30);

		// Get Logging
		if (tempValue.isMember("logging")) {
			Json::Value tmp = tempValue["logging"]["client"]["file"];
			resp = Get(tmp["url"].asCString());
			strcpyf(tmpS2, "%s\\%s", tmpS, tmp["id"].asCString());
			fp = fopen(tmpS2, "w");
			fwrite(resp.GetText().c_str(), 1, resp.size(), fp);
			fclose(fp);
		}
		pgbProgress->add(5);

		// Get Assets Index
		resp = Get(tempValue["assetIndex"]["url"].asCString());
		GetWindowTextA(*ediSettingsDir, tmpS2, 512);
		strcatf(tmpS2, "\\assets\\indexes\\%s.json", tempValue["assetIndex"]["id"].asCString());
		MakeSureDirectoryPathExists(tmpS2);
		reader.parse(resp.GetText(), tempValue);
		s = sw.write(tempValue);
		fp = fopen(tmpS2, "w");
		fwrite(s.c_str(), 1, resp.size(), fp);
		fclose(fp);

		//* Get Assets

		progress = 70;
		step = 30.0 / tempValue["objects"].size();
		for (Json::Value i : tempValue["objects"]) {
			DWORD threadID;
			//HANDLE hThread = CreateThread(NULL, 0, downloadAsset, &i, 0, &threadID);	// 创建线程
			//thread thr(downloadAsset, i, step);
			//thr.detach();
		}
		pgbProgress->add(30);

		// Finish

		pgbProgress->set(100);
		strcpyf(tmpS, doTranslate("prompt.mcje.download.ok"), versionId);
		MessageBoxA(win, tmpS, doTranslate("prompt"), MB_OK | MB_ICONINFORMATION);
		//pgbProgress->set(0);

		return 0;
		});
	thr.detach();
	return 0;
}
