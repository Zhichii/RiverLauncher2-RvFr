#pragma once
#include "H:\RiverGuiFrame\RiverGuiFrame\rvg.h"

RvG::Window* x;
RvG::Container* curPage;
bool hasPage2 = 0;
RvG::Container* curPage2;
RvG::Button* btnLaunch;
RvG::ProgressBar* pgbProgress;

RvG::Button* swiMinecraft;
RvG::Container* pageMinecraft;
RvG::ListBox* lisMinecraftVersion;
RvG::Label* labMinecraftVersionPrompt;
RvG::Button* btnMinecraftAdd;
RvG::Button* btnMinecraftRemove;
RvG::Button* btnMinecraftOpen;
int intMinecraftSel = 0;

RvG::Container* pageMinecraftDownloads;
RvG::ListBox* lisMinecraftDownloads;
RvG::Label* labMinecraftDownloadsPrompt;
RvG::Button* btnMinecraftDownloads;

RvG::Button* swiAccounts;
RvG::Container* pageAccounts;
RvG::ListBox* lisAccountsList;
RvG::Label* labAccountsPrompt;
RvG::Button* btnAccountsRegister;
RvG::Button* btnAccountsRemove;
RvG::Button* swiAccountsAdd;
RvG::InputBox* inpDialogUsername;

RvG::Button* swiSettings;
RvG::Container* pageSettings;
RvG::InputBox* ediSettingsDir;
RvG::InputBox* ediSettingsWid;
RvG::InputBox* ediSettingsHei;

RvG::Button* swiMods;
RvG::Container* pageMods;
RvG::Label* labModsPrompt;
RvG::ListBox* lisMods;
RvG::Button* btnModsEnable;
RvG::Button* btnModsSwitch;
RvG::Button* btnModsDisable;
RvG::Button* btnModsOpen;

RvG::Button* swiLang;
RvG::Container* pageLang;
RvG::ListBox* lisLangList;
int intLangSel = 0;

RvG::Button* swiMinecraftBE;
RvG::Container* pageMinecraftBE;
RvG::Button* btnMinecraftBELaunch;
RvG::Button* btnMinecraftBEPreviewLaunch;