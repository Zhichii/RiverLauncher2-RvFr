#pragma once
#include "H:\RiverGuiFrame\RiverGuiFrame\rvg.h"

RvG::Window* x;
RvG::Container* curPage;
RvG::Button* btnLaunch;

RvG::Button* swiMinecraft;
RvG::Container* pageMinecraft;
RvG::Button* btnMinecraftEnd;
RvG::Label* labMinecraftLog;
RvG::Label* labMinecraftVersionPrompt;
RvG::ListBox* lisMinecraftVersion;
int intMinecraftSel = 0;

RvG::Button* swiMinecraftDownloads;
RvG::Container* pageMinecraftDownloads;

RvG::Button* swiAccounts;
RvG::Container* pageAccounts;
RvG::Button* btnAccountsRegister;
RvG::Button* btnAccountsRemove;
RvG::Button* swiAccountsAdd;
RvG::Container* pageAccountsAdd;
RvG::ListBox* lisAccountsList;
RvG::Label* labAccountsPrompt;
RvG::InputBox* inpDialogUsername;

RvG::Button* swiSettings;
RvG::Container* pageSettings;
RvG::InputBox* ediSettingsDir;
RvG::InputBox* ediSettingsWid;
RvG::InputBox* ediSettingsHei;

RvG::Button* swiMods;
RvG::Container* pageMods;

RvG::Button* swiMinecraftBE;
RvG::Container* pageMinecraftBE;
RvG::Button* btnMinecraftBELaunch;
RvG::Button* btnMinecraftBEPreviewLaunch;