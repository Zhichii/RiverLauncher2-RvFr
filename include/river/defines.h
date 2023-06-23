#pragma once

#include <stdio.h>
#include <filesystem>
#include <urlmon.h>
#include <ctime>
#include <cstring>
#include <string>
#include <malloc.h>
#include <direct.h>
#include <fstream>
#include <string.h>
#include <windows.h>
#include <thread>
#include <sys/stat.h>
#include <cstdarg>
#include <zip/unzip.h>
#include <requests/requests.h>
#include <json/json.h> // The GuiFrame has already compiled JsonCpp into .lib, so do not include the cpp files
#include <river/ui.h>
#include <mutex>
#include <ImageHlp.h>
#include "resource.h"
#define INT_MAX 2147483647
#define ALLOC 4096
#define DEBUG_MODE 0
using namespace std;
using namespace requests;

char loadStringBuf[258];
Json::Reader reader;
FILE* programmeLog = fopen("RvL\\Log.txt", "w");
HKEY hData;
DWORD sz = 4;
Json::Value accounts = Json::arrayValue;
Json::Value langs = Json::arrayValue;
int intAccountsSel = 0;
int intSettingsWid;
int intSettingsHei;
RvG::Window* minecraftLog;
RvG::Label* staticLab;
RvG::Button* staticBtn;
Json::Value versionManifest = Json::arrayValue;
HINTERNET hInternet;
map<string, HINTERNET> hosts;

char baseStr[258];
char newStr[258];


HANDLE hRead, hWrite;
STARTUPINFOA si;
PROCESS_INFORMATION pi;
double progress = 0;
std::mutex mtx;

/*
 * Minecraft [Download]
 * Accounts [Login to accounts]
 * Settings
 * Connect [Create new rooms]
 */

// %% means "%". %d means "int". %l means "long long". %s means "char*". %c means "char". 
int writeLog(const char* logger, const char* format, ...) {
	if (programmeLog == nullptr) return 1;
	char str[256];
	strcpy(str, "[%d, %s] ");
	va_list args;
	va_start(args, format);
	long long i;
	printf(str, time(NULL), logger);
	fprintf(programmeLog, str, time(NULL), logger);
	fflush(programmeLog);
	int fori;
	for (fori = 0; fori < strlen(format); fori++) {
		if (format[fori] == '%') {
			switch (format[fori + 1]) {
			case '%': {
				printf("%%");
				fprintf(programmeLog, "%%");
				fflush(programmeLog);
				break;
			}
			case 's': {
				i = va_arg(args, long long);
				printf("%s", i);
				fprintf(programmeLog, "%s", i);
				fflush(programmeLog);
				break;
			}
			case 'd': {
				i = va_arg(args, int);
				printf("%d", i);
				fprintf(programmeLog, "%d", i);
				fflush(programmeLog);
				break;
			}
			case 'l': {
				i = va_arg(args, long long);
				printf("%lld", i);
				fprintf(programmeLog, "%lld", i);
				fflush(programmeLog);
				break;
			}
			case 'c': {
				i = va_arg(args, char);
				printf("%c", i);
				fprintf(programmeLog, "%c", i);
				fflush(programmeLog);
				break;
			}
			default: {
				break;
			}
			}
			fori++;
		}
		else {
			printf("%c", format[fori]);
			fprintf(programmeLog, "%c", format[fori]);
			fflush(programmeLog);
		}
	}
	va_end(args);
	fprintf(programmeLog, "\n");
	fflush(programmeLog);
	return 0;
}

int split(const char* baseStr, char* newStr, int newSize, int from = 0, int to = 0, int step = 1) {
	if (from == to) to = strlen(baseStr) - to;
	if (from < 0) from = strlen(baseStr) + from;
	if (to < 0) to = strlen(baseStr) + to;
	if (step == 0) step = 1;
	if (from > to) step = -step;
	strcpy(newStr, "\0");
	if (to > newSize) to = strlen(baseStr);

	int cur = 0;
	for (int i = from; (from < to) ? (i < to) : (i > to); i += step) {
		newStr[cur++] = baseStr[i];
	}
	newStr[cur] = 0;

	return 0;
}

int find(const char* str, const char* substr) {
	for (int i = 0; i < strlen(str); i++) {
		for (int j = 0; j < strlen(substr); j++) {
			if ((i + j) > strlen(str)) break;
			if (DEBUG_MODE) writeLog("find", "%d, %d, %c, %c", i, j, str[i + j], substr[j]);
			if (str[i + j] != substr[j]) break;
			if (j == strlen(substr) - 1) {
				return i;
			}
		}
	}
	return -1;
}

int count(const char* str, const char* substr) {
	int i;
	int cnt = 0;
	for (i = 0; i < strlen(str); i++) {
		for (int j = 0; j < strlen(substr); j++) {
			if ((i + j) > strlen(str)) break;
			if (str[i + j] != substr[j]) break;
			if (j == strlen(substr) - 1) {
				cnt++;
				i += j;
				break;
			}
		}
	}
	return cnt;
}

int replace(const int newSize, const char* baseStr, char* newStr, const char* from, const char* to) {
	if (strlen(baseStr) < strlen(from)) {
		strcpy(newStr, baseStr);
		return 0;
	}
	const int cnt = count(baseStr, from);
	int i = 0, j = 0;
	const int baseLen = strlen(baseStr);
	const int fromLen = strlen(from);
	const int toLen = strlen(to);
	const int newLen = baseLen + (toLen)*cnt + 2;

	char tmp[5120];
	char tmp2[2];
	strcpy(tmp, "\0");
	strcpy(newStr, "\0");
	int cur = 0, curLast = 0;
	bool flag = 0;
	for (i = 0; i < baseLen; i++) {
		flag = 0;
		for (j = 0; j < strlen(from); j++) {
			if ((i + j) > baseLen) break;
			if (DEBUG_MODE) writeLog("replace", "%d, %d, %c, %c, %s\t%s", i, j, baseStr[i + j], from[j], tmp, newStr);
			tmp[cur++] = baseStr[i + j];
			if (baseStr[i + j] != from[j]) {
				tmp2[0] = baseStr[i];
				tmp2[1] = 0;
				strcat(newStr, tmp2);
				strcpy(tmp, "\0");
				cur = 0;
				break;
			}
			if (j == strlen(from) - 1) {
				flag = 1;
				strcat(newStr, to);
				strcpy(tmp, "\0");
				i += j;
				break;
			}
		}
	}
	if (DEBUG_MODE) writeLog("replace", "%s\t%s", tmp, newStr);
	return 0;
}

int valueSplit(const char* str, const char* sep, Json::Value* output) {
	*output = Json::arrayValue;
	output->clear();
	if (strlen(str) < strlen(sep)) {
		output->append(str);
		return 0;
	}
	const int cnt = count(str, sep);
	int i = 0, j = 0;
	const int strLen = strlen(str);
	const int sepLen = strlen(sep);

	char* tmp = (char*)malloc(strLen + 2);
	char tmp2[2];
	char* tmp3 = (char*)malloc(strLen + 2);
	strcpy(tmp, "\0");
	strcpy(tmp3, "\0");

	int cur = 0, curLast = 0;
	bool flag = 0;
	for (i = 0; i < strLen; i++) {
		flag = 0;
		for (j = 0; j < sepLen; j++) {
			if ((i + j) > strLen) break;
			if (DEBUG_MODE) writeLog("valueSplit", "%d, %d, %c, %c, %s\t%s", i, j, str[i + j], sep[j], tmp, tmp3);
			tmp[cur++] = str[i + j];
			if (str[i + j] != sep[j]) {
				tmp2[0] = str[i];
				tmp2[1] = 0;
				strcat(tmp3, tmp2);
				strcpy(tmp, "\0");
				cur = 0;
				break;
			}
			if (j == strlen(sep) - 1) {
				flag = 1;
				output->append(tmp3);
				strcpy(tmp, "\0");
				strcpy(tmp3, "\0");
				i += j;
				break;
			}
		}
	}
	output->append(tmp3);
	if (DEBUG_MODE) writeLog("valueSplit", "%s\t%s", tmp, tmp3);

	free(tmp);
	free(tmp3);
	return 0;
}

int join(Json::Value arr, char* output, int outputSize, const char* joiner) {
	strcpy(output, "\0");
	int flag = 0;
	for (Json::Value i : arr) {
		if (strlen(i.asCString()) == 0) continue;
		if (flag) {
			strcat(output, joiner);
		}
		if (!flag) {
			flag = 1;
		}
		strcat(output, i.asCString());
	}
	return 0;
}

int writeFile(FILE* stream, const char* str) {
	if (stream == nullptr) return 1;
	fwrite(str, 1, strlen(str), stream);
	return 0;
}

int sp(const char* str, int strSize, const char* a, const char* b, char* output) {
	strcpy(output, "\0");
	char* tmp = (char*)malloc(strSize + 2);

	int aPos = 0;
	int lenA = 0;
	if (a != nullptr) {
		aPos = find(str, a);
		lenA = strlen(a);
	}
	if (aPos == -1) split(str, tmp, strSize, 0, strlen(str));
	else split(str, tmp, strSize, aPos + lenA, strlen(str));

	int bPos = strSize + 8;
	if (b != nullptr) bPos = find(tmp, b);
	split(tmp, output, strSize, 0, bPos);

	free(tmp);
	return 0;
}

// %% means "%". %d means "int". %l means "long long". %s means "char*". %c means "char". 
char* strcpyf(char* dst, const char* srcFormat, ...) {
	char tmp[258];
	strcpy(dst, "\0");
	va_list args;
	va_start(args, srcFormat);
	long long i;
	int fori;
	for (fori = 0; fori < strlen(srcFormat); fori++) {
		if (srcFormat[fori] == '%') {
			switch (srcFormat[fori + 1]) {
			case '%': {
				strcat(dst, "%%");
				break;
			}
			case 's': {
				i = va_arg(args, long long);
				strcat(dst, (char*)i);
				break;
			}
			case 'd': {
				i = va_arg(args, int);
				itoa(i, tmp, 10);
				strcat(dst, tmp);
				break;
			}
			case 'l': {
				i = va_arg(args, long long);
				ltoa(i, tmp, 10);
				strcat(dst, tmp);
				break;
			}
			case 'c': {
				i = va_arg(args, char);
				tmp[0] = i;
				tmp[1] = 0;
				strcat(dst, tmp);
				break;
			}
			default: {
				break;
			}
			}
			fori++;
		}
		else {
			tmp[0] = srcFormat[fori];
			tmp[1] = 0;
			strcat(dst, tmp);
		}
	}
	va_end(args);
	return dst;
}

// %% means "%". %d means "int". %l means "long long". %s means "char*". %c means "char". 
char* strcatf(char* dst, const char* srcFormat, ...) {
	char tmp[258];
	va_list args;
	va_start(args, srcFormat);
	long long i;
	int fori;
	for (fori = 0; fori < strlen(srcFormat); fori++) {
		if (srcFormat[fori] == '%') {
			switch (srcFormat[fori + 1]) {
			case '%': {
				strcat(dst, "%%");
				break;
			}
			case 's': {
				i = va_arg(args, long long);
				strcat(dst, (char*)i);
				break;
			}
			case 'd': {
				i = va_arg(args, int);
				itoa(i, tmp, 10);
				strcat(dst, tmp);
				break;
			}
			case 'l': {
				i = va_arg(args, long long);
				ltoa(i, tmp, 10);
				strcat(dst, tmp);
				break;
			}
			case 'c': {
				i = va_arg(args, char);
				tmp[0] = i;
				tmp[1] = 0;
				strcat(dst, tmp);
				break;
			}
			default: {
				break;
			}
			}
			fori++;
		}
		else {
			tmp[0] = srcFormat[fori];
			tmp[1] = 0;
			strcat(dst, tmp);
		}
	}
	va_end(args);
	return dst;
}

int findFile(const char* fileEnding) {
	return 0;
}

int reqGetBinary(const char* url, BYTE* output, int* size) {
	try {
		Response resp = Get(url);
		memcpy(output, resp.GetBinary(), resp.size());
		if (size != nullptr) *size = resp.size();
	}
	catch (const char* error) {
		writeLog("ReqGetBinary", error);
	}
	return 0;
}

int reqGetString(const char* url, char* output, int* size) {
	try {
		Response resp = Get(url);
		memcpy(output, resp.GetText().c_str(), resp.size());
		if (size != nullptr) *size = resp.size();
	}
	catch (const char* error) {
		writeLog("ReqGetString", error);
	}
	return 0;
}

int winGetHttps(char** out, const char* url, int* oSize) {
	const int size = strlen(url);
	DWORD flags;
	char* t = (char*)url;
	if (find(url, "https://") == 0) {
		flags = INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP |
			INTERNET_FLAG_KEEP_CONNECTION |
			INTERNET_FLAG_NO_UI |
			INTERNET_FLAG_SECURE |
			INTERNET_FLAG_RELOAD;
		t += 8;
	}
	else if (find(url, "http://") == 0) {
		flags = INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_RELOAD;
		t += 7;
	}
	char host[128];
	sp(t, strlen(t), nullptr, "/", host);
	char file[128] = { '/' };
	sp(t, strlen(t), "/", nullptr, file+1);


	if (hInternet == NULL) {
		hInternet = InternetOpenA("RiverLauncher2", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
		if (hInternet == NULL) {
			int error = GetLastError();
			writeLog("InternetOpenA", "%d", error);
			return error;
		}
	}
	DWORD d = 0;
	HINTERNET hConnect;
	if (!hosts.contains(host)) {
		hConnect = InternetConnectA(hInternet, host, INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, (DWORD_PTR)&d);
	}
	if (hConnect == NULL) {
		int error = GetLastError();
		writeLog("InternetConnectA", "%d", error);
		InternetCloseHandle(hInternet);
		return error;
	}
	hosts[host] = hConnect;
	PCSTR rgpszAcceptTypes[] = { "*/*", NULL };
	HINTERNET hRequest = HttpOpenRequestA(hConnect, "GET", file, "HTTP/1.1", NULL, rgpszAcceptTypes, flags, 0);
	if (hRequest == NULL) {
		int error = GetLastError();
		writeLog("HttpOpenRequestA", "%d", error);
		InternetCloseHandle(hConnect);
		InternetCloseHandle(hInternet);
		return error;
	}
	d = HttpSendRequestA(hRequest, NULL, 0, NULL, 0);
	if (d == FALSE) {
		int error = GetLastError();
		writeLog("HttpSendRequestA", "%d", error);
		InternetCloseHandle(hRequest);
		InternetCloseHandle(hConnect);
		InternetCloseHandle(hInternet);
		return error;
	}
	char buf[10];
	DWORD len = 10;
	if (HttpQueryInfoA(hRequest, HTTP_QUERY_CONTENT_LENGTH, &buf, &len, NULL) == FALSE) {
		int error = GetLastError();
		writeLog("HttpQueryInfoA", "%d", error);
		InternetCloseHandle(hRequest);
		InternetCloseHandle(hConnect);
		InternetCloseHandle(hInternet);
		return error;
	}
	char* temp = (char*)malloc(atoi(buf) + 2);
	if (oSize != nullptr) *oSize = atoi(buf);
	ZeroMemory(temp, 2);
	DWORD nLen;
	if (InternetReadFile(hRequest, temp, atoi(buf), &nLen) == FALSE) {
		int error = GetLastError();
		writeLog("InternetReadFile", "%d", error);
		free(temp);
		InternetCloseHandle(hRequest);
		InternetCloseHandle(hConnect);
		InternetCloseHandle(hInternet);
		return error;
	}
	InternetCloseHandle(hRequest);
	InternetCloseHandle(hConnect);
	InternetCloseHandle(hInternet);
	*out = temp;
	return 0;
}

char* doTranslate(const char* translationKey) {
	if ((RvG::lang).isMember(translationKey)) strcpy(loadStringBuf, (RvG::lang)[translationKey].asCString());
	else strcpy(loadStringBuf, translationKey);
	return loadStringBuf;
}