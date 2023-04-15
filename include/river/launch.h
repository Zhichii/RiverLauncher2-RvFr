#include <river/defines.h>

void launchInstance(const char* versionId, const char* dir, HWND edit) {

    allocListCurOld = allocListCur;
    void* thisAllocList[ALLOC] = { 0 };
    allocListCur = thisAllocList;
    
    // Pre-check
    
    if (accounts.size() == 0) {
        writeLog("launchInstance(versionId)", "No accounts created! ");
        //return;
    }

    // Prepare

    char cwd[512] = "Haha, you can't find this without finding in the source! ";
    strcpy(cwd, dir);//getcwd(cwd, 512);
    strcat(cwd, "\\");
    char fullVersionJson[512] = "";
    strcpy(fullVersionJson, cwd);
    strcat(fullVersionJson, "versions\\");
    strcat(fullVersionJson, versionId);
    strcat(fullVersionJson, "\\");
    strcat(fullVersionJson, versionId);
    strcat(fullVersionJson, ".json");
    ifstream file(fullVersionJson);
    Json::Value versionInfo;
    reader.parse(file, versionInfo);
    int mark = 0;
    Json::Value available;
    Json::Value libraries = versionInfo["libraries"];
    Json::Value tmp;
    char* theVersion;
    Json::Value versionLib;
    Json::Value libNameSp;
    char* libDir;
    char* libSFName;
    char tmpS[512];
    char* tmpC;
    char* tmpC2;
    int tmpI;
    Json::Value k;

    // Get Libraries

    for (Json::Value i : libraries) {
        tmp = valueSplit(i["name"].asCString(), ":");
        tmp[2] = "";
        theVersion = join(tmp, ":");
        mark = 0;
        if (i["downloads"].isMember(Json::String("artifact"))) {
            versionLib = i["downloads"]["artifact"];
            mark = 1;
        }
        if (i["downloads"].isMember(Json::String("classifiers"))) {
            versionLib = i["downloads"]["classifiers"];
            mark = 1;
        }
        else if (mark == 0) {
            if (i.isMember(Json::String("name"))) {
                libNameSp = valueSplit(i["name"].asCString(), ":");
                tmpC = (char*)malloc(strlen(libNameSp.asCString()));
                strcpy(tmpC, libNameSp.asCString());
                libNameSp[0] = tmpC;
                free(tmpC);
                libDir = join(libNameSp, "\\");
                libNameSp.resize(libNameSp.size() - 2);
                libSFName = join(libNameSp, "-");
                strcat(libSFName, ".jar");
                memset(tmpS, 0, 256);
                strcpy(tmpS, cwd);
                strcat(tmpS, "libraries\\");
                strcat(tmpS, libDir);
                for (int i = 0; i < strlen(libDir); i++) {
                    if (libDir[i] == '/') {
                        libDir[i] = '\\';
                    }
                }
                available[theVersion] = tmpS;
                free(libDir);
                free(tmpC);
                free(libSFName);
            }
            continue;
        }
        if (i.isMember(Json::String("rules"))) {
            tmpI = 0;
            for (int l = 0; l < i["rules"].size(); l++) {
                k = i["rules"][l];
                if (strcmp(k["action"].asCString(), "allow") == 0) {
                    if (k.isMember(Json::String("os"))) {
                        if (strcmp(k["os"]["name"].asCString(), "windows") == 0) {
                            tmpI = 1;
                        }
                    }
                    else tmpI = 1;
                }
                if (strcmp(k["action"].asCString(), "disallow") == 0) {
                    if (k.isMember(Json::String("os"))) {
                        if (strcmp(k["os"]["name"].asCString(), "windows") == 0) {
                            tmpI = 0;
                        }
                    }
                    else tmpI = 0;
                }
            }
            if (tmpI == 0) {
                continue;
            }
        }
        libDir = (char*)malloc(256);
        strcpy(libDir, cwd);
        strcat(libDir, "libraries\\");
        tmpC = (char*)malloc(strlen(versionLib["path"].asCString()));
        strcpy(tmpC, versionLib["path"].asCString());
        strcat(libDir, tmpC);
        if (i.isMember(Json::String("natives"))) {
            // HlHill is lazy...
        }
        for (int i = 0; i < strlen(libDir); i++) {
            if (libDir[i] == '/') {
                libDir[i] = '\\';
            }
        }
        available[theVersion] = libDir;
        free(libDir);
        free(theVersion);
    }
    tmp.clear();
    available.empty();
    for (Json::Value i : available) {
        tmp.append(i.asCString());
    }
    char fullVersionJar[512];
    strcpy(fullVersionJar, fullVersionJson);
    strcpy(fullVersionJar + strlen(fullVersionJson) - 5, ".jar\0");
    tmp.append(fullVersionJar);
    tmpC = join(tmp, ";");
    writeLog("launchInstance(versionId)", tmpC);
    //ui->infoText->setText(tmpC);
    SetWindowTextA(edit, tmpC);

    // Get Launch Type

    bool level;
    if (versionInfo.isMember(Json::String("arguments"))) {
        level = 1;
    }
    else if (versionInfo.isMember(Json::String("minecraftArguments"))) {
        level = 0;
    }
    else {
        writeLog("launchInstance(versionId)", "�޷�ʶ���������, ��Ĭ�����ɰ�");
        level = 0;
    }
    available.empty();
    libraries.empty();
    tmp.empty();
    versionLib.empty();
    libNameSp.empty();
    k.empty();

    // Get Arguments

    Json::Value gameArg;
    char* gameArgC = NULL;
    Json::Value jvmArg;
    char* jvmArgC;
    if (level == 0) {
        gameArgC = (char*)malloc(sizeof(versionInfo["minecraftArguments"].asCString()));
        strcpy(gameArgC, versionInfo["minecraftArguments"].asCString());
        jvmArg.clear();
        jvmArg.append("-cp");
        jvmArg.append(tmpC);
    }
    if (level == 1) {
        for (Json::Value i : versionInfo["arguments"]["game"]) {
            if (i.isString()) {
                gameArg.append(i);
            }
            else {
                tmp = i["value"];
                for (Json::Value j : tmp) {
                    gameArg.append(j);
                }
            }
        }
        gameArgC = join(gameArg, " ");
        jvmArg.clear();
        for (Json::Value i : versionInfo["arguments"]["jvm"]) {
            if (i.isString()) {
                jvmArg.append(i);
            }
            else {
                tmp = i["value"];
                tmpI = 0;
                for (Json::Value j : i["rules"]) {
                    if (!j["os"].isMember(Json::String("os"))) {
                        continue;
                    }
                    if (strcmp(j["action"].asCString(), "allow")) {
                        if (j.isMember("os")) {
                            if (strcmp(j["os"]["name"].asCString(), "windows")) {
                                tmpI = 1;
                            }
                        }
                        else {
                            tmpI = 1;
                        }
                    }
                    if (strcmp(j["action"].asCString(), "disallow")) {
                        if (j.isMember("os")) {
                            if (strcmp(j["os"]["name"].asCString(), "windows")) {
                                tmpI = 0;
                            }
                        }
                        else {
                            tmpI = 0;
                        }
                    }
                }
                if (tmpI == 0) {
                    continue;
                }
            }
        }
    }
    jvmArgC = join(jvmArg, " ");
    strcpy(tmpS, cwd);
    strcat(tmpS, "versions\\");
    strcat(tmpS, versionId);
    strcat(tmpS, "\\river_launch.bat");

    // Get Java

    FILE* output = fopen(tmpS, "w");
    writeFile(output, "@echo off\ntitle Minecraft Log\ncd /d ");
    writeFile(output, cwd);
    writeFile(output, "\n");
    writeFile(output, "pause\n");
    FILE* tmpF = _popen("where java", "r");
    Json::Value java = Json::arrayValue;
    Json::Value tmp2 = Json::arrayValue;
    tmp = Json::arrayValue;
    while (fgets(tmpS, 512, tmpF)) {
        tmpS[strlen(tmpS)-1] = 0;
        tmp.append(tmpS);
    }
    fclose(tmpF);
    int flag3 = 0;
    struct stat st;
    if (tmp.size() != 0) {
        flag3 = 1;
        for (Json::Value i : tmp) {
            tmp2 = Json::arrayValue;
            tmp2.append(i);
            strcpy(tmpS, "\"");
            strcat(tmpS, i.asCString());
            strcat(tmpS, "\" -version 2>&1");
            tmpF = _popen(tmpS, "r");
            fgets(tmpS, 512, tmpF);
            fgets(tmpS, 512, tmpF);
            tmpS[strlen(tmpS)-1] = 0;
            tmpS[strlen(tmpS)-2] = 0;
            for (int j = strlen(tmpS) - 1; j > 0; j--) {
                if (tmpS[j] == '(') {
                    tmp2.append(tmpS+j+7);
                    printf("\n");
                    writeLog("launchInstance(versionId)", tmpS+j+7);
                    break;
                }
            }
            fgets(tmpS, 512, tmpF);
            if (tmpS[8] == '6') {
                tmp2.append(true);
                writeLog("launchInstance(versionId)", "64 Bit! ");
            }
            else {
                tmp2.append(false);
            }
            strcpy(tmpS, i.asCString());
            tmpI = strlen(tmpS);
            tmpS[tmpI-4] = 'c';
            tmpS[tmpI-3] = '.';
            tmpS[tmpI-2] = 'e';
            tmpS[tmpI-1] = 'x';
            tmpS[tmpI] = 'e';
            tmpS[tmpI+1] = 0;
            writeLog("launchInstance(versionId)", tmpS);
            if (stat(tmpS, &st) == 0) {
                tmp2.append(true);
                writeLog("launchInstance(versionId)", "JDK! ");
            }
            else {
                tmp2.append(false);
            }
            java.append(tmp2);
        }
    }
    tmp2.empty();

    // Choose Java

    int flag2 = 0;
    Json::Value j;
    if ((versionInfo.isMember(Json::String("javaVersion"))) && (versionInfo["javaVersion"]["majorVersion"].asInt() > 12)) {
        strcpy(tmpS, cwd);
        strcat(tmpS, "jre\\old\\bin\\java.exe");
        if (stat(tmpS, &st) == 0) {
            writeFile(output, tmpS);
        }
        else {
            flag2 = 1;
            for (Json::Value i : java) {
                j = i[1];

            }
        }
    }
    j.empty();
    java.empty();


    // Clean

    fclose(tmpF);
    fclose(output);
    free(tmpC);
    free(gameArgC);
    free(jvmArgC);
    gameArg.clear();
    jvmArg.clear();

    freeAllocList(thisAllocList);
    allocListCur = allocListCurOld;
}
