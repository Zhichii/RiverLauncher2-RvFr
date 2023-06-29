#pragma once

#include <string.h>
#include <string>
#include <malloc.h>
#define RVT_SPACE 1
using namespace std;

struct str {
    char* data = nullptr;
    int dataSize = 0; // With \0
    bool isAllocated = 0;
public:
    str(const char* _str) {
        if (!isAllocated) {
            dataSize = strlen(_str) + 1;
            data = (char*)malloc(dataSize + RVT_SPACE);
            strcpy(data, _str);
            isAllocated = 1;
        }
    }
    str(string _str) {
        if (!isAllocated) {
            dataSize = ((int)_str.length()) + 1;
            data = (char*)malloc(dataSize + RVT_SPACE);
            strcpy(data, _str.c_str());
            isAllocated = 1;
        }
    }
    ~str() {
        if (isAllocated) {
            dataSize = 0;
            free(data);
            isAllocated = 0;
        }
    }

    int length() {
        if (isAllocated) {
            return dataSize - 1;
        }
        return 0;
    }
    char* toCArr() {
        if (isAllocated) {
            return data;
        }
        return 0;
    }

    str operator= (str b) {
        if (b.length() != 0) {
            if (isAllocated) {
                dataSize = 0;
                free(data);
                isAllocated = 0;
            }
            dataSize = b.length() + 1;
            data = (char*)malloc(dataSize + RVT_SPACE);
            strcpy(data, b.toCArr());
            isAllocated = 1;
        }
        return *this;
    }
    str operator+ (str b) {
        dataSize = length() + b.length() + 1;
        char* tempOutput = (char*)malloc(length() + b.length() + RVT_SPACE);
        if (b.length() != 0) {
            if (isAllocated) {
                strcpy(tempOutput, toCArr());
                strcat(tempOutput, b.toCArr());
            }
            else {
                strcpy(tempOutput, b.toCArr());
            }
        }
        str tempStr(tempOutput);
        free(tempOutput);
        return tempStr;
    }
    str operator* (int n) {
        if (isAllocated) {
            dataSize = length() * n + 1;
            char* tempOutput = (char*)malloc(length() * n + RVT_SPACE);
            for (int i = 0; i < n; i++) {
                if (i == 0) strcpy(tempOutput, toCArr());
                else strcat(tempOutput, toCArr());
            }
            str tempStr(tempOutput);
            free(tempOutput);
            return tempStr;
        }
        else {
            return *this;
        }        
    }
    char operator[] (int n) {
        if (isAllocated) {
            return data[n];
        }
        else {
            return 0;
        }        
    }

    str substr(int a, int b, int step = 1) { // Includes a and b
        if (isAllocated) {
            if (a < 0) a = dataSize - a + 1;
            if (a >= dataSize) a = dataSize - 1;
            if (b < 0) b = dataSize - b + 1;
            if (b >= dataSize) b = dataSize - 1;
            if (step == 0) step = 1;
            if (step < 0 && a < b) step = -step;
            if (step > 0 && a > b) step = -step;
            dataSize = b - a + 2;
            char* tempOutput = (char*)malloc(b - a + 1 + RVT_SPACE);
            int cur = 0;
            for (int i = a; i != b; i += step) {
                tempOutput[cur++] = data[i];
            }
            str tempStr(tempOutput);
            free(tempOutput);
            return tempStr;
        }
        else {
            return *this;
        }
    }
    int find(str substr) {
        if (isAllocated) {
	    for (int i = 0; i < length(); i++) {
                for (int j = 0; j < substr.length(); j++) {
                    if ((i + j) > length()) break;
                    if (data[i + j] != substr[j]) break;
                    if (j == substr.length() - 1) return i;
                }
            }
        }
        else {
            return -1;
        }
    }
    int count(str substr) {
        if (isAllocated) {
            int cnt = 0;
            for (int i = 0; i < length(); i++) {
                for (int j = 0; j < substr.length(); j++) {
                    if ((i + j) > length()) break;
                    if (data[i + j] != substr[j]) break;
                    if (j == substr.length() - 1) {
                        cnt++;
                        i += j;
                        break;
                    }
                }
            }
        }
        else {
            return 0;
        }
    }
    str replace(str from, str to) {
        if (from.length() == 0 || to.length() == 0) return *this;
        if (isAllocated) {
            dataSize = length() + (to.length() - from.length()) * count(from) + 1;
            int cur = 0, curLast = 0;
            char* tempOutput = (char*)malloc(dataSize - 1 + RVT_SPACE);
            char* tmp = (char*)malloc(length() + RVT_SPACE);
            char tmp2[2];
            for (int i = 0; i < length(); i++) {
                for (int j = 0; j < from.length(); j++) {
                    if ((i + j) > length()) break;
                    tmp[cur++] = data[i + j];
                    if (data[i + j] != from[j]) {
                        tmp2[0] = data[i];
                        tmp2[1] = 0;
                        strcat(tempOutput, tmp2);
                        strcpy(tmp, "\0");
                        cur = 0;
                        break;
                    }
                    if (j == from.length() - 1) {
                        strcat(tempOutput, to.toCArr());
                        strcpy(tmp, "\0");
                        i += j;
                        break;
                    }
                }
            }
            str tempStr(tempOutput);
            free(tempOutput);
            return tempStr;
        }
        else {
            return *this;
        }
    }
};