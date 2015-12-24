#include "StdAfx.h"

#include "..\..\lltool\include\lllogger.h"
#include "..\..\lltool\include\llutils.h"
#include "llguicommands.h"

int gamemode = 0;
const char *game[MAX_GAMES];
const char *plugin[MAX_GAMES];
const char *pattern[MAX_GAMES];
const char *std_ws[MAX_GAMES];

float guiwidth, guiheight, vdist, value;
char *guiname, *guitab, *guitext, *guihelp, *guiparent;
char *flagtext;
int   myswitch, textinput, fileinput, sameline, guienabled, dumpbatch, unselect;

/* ----------------------------------- */

llSetPath::llSetPath() : llWorker() {
	SetCommandName(COM_SETPATH_CMD);
	SetFixedIndex(COM_SETPATH);
}

int llSetPath::RegisterOptions(void) {
	if (!llWorker::RegisterOptions()) return 0;
	RegisterValue("-value", &flagtext, LLWORKER_OBL_OPTION);
	return 1;
}

int llSetPath::Prepare(void) {
	if (!llWorker::Prepare()) return 0;
	flagtext = NULL;
	return 1;
}

/* ----------------------------------- */

llGameMode::llGameMode() : llWorker() {
	SetCommandName(COM_GAMEMODE_CMD);
	SetFixedIndex(COM_GAMEMODE);
}

int llGameMode::RegisterOptions(void) {
	if (!llWorker::RegisterOptions()) return 0;

	RegisterValue("-name", &name, LLWORKER_OBL_OPTION);

	return 1;
}

int llGameMode::Exec(void) {
	llWorker::Exec();

	gamemode = 0;
	for (int i=0; i<MAX_GAMES; i++) {
		if (game[i] && _stricmp(game[i], name) == 0) {
			_llLogger()->WriteNextLine(LOG_INFO, "Game mode set to %s (-n=%i)", COM_GAMEMODE_CMD, name, i);
			gamemode = i;
			_llUtils()->SetValue("_gamemode", game[gamemode]);
			_llUtils()->SetHidden("_gamemode");
		}
	}
	if (!gamemode) {
		_llLogger()->WriteNextLine(LOG_ERROR, "%s: game %s not defined", COM_GAMEMODE_CMD, name);
		return 0;
	}

	return 1;
}

/* ----------------------------------- */

llAddGame::llAddGame() : llWorker() {
	SetCommandName(COM_ADDGAME_CMD);
	SetFixedIndex(COM_ADDGAME);
}

int llAddGame::RegisterOptions(void) {
	if (!llWorker::RegisterOptions()) return 0;

	RegisterValue("-n",    &n,    LLWORKER_OBL_OPTION);
	RegisterValue("-name", &name, LLWORKER_OBL_OPTION);

	return 1;
}

int llAddGame::Exec(void) {
	llWorker::Exec();

	_llLogger()->WriteNextLine(LOG_INFO, "Game %s added", name);
	if (n > MAX_GAMES) {
		_llLogger()->WriteNextLine(LOG_ERROR, "Too many games");
		return 0;
	}
	game[n] = name;

	return 1;
}

/* ----------------------------------- */

llSetGamePluginFile::llSetGamePluginFile() : llWorker() {
	SetCommandName(COM_SETGAMEPLUGINFILE_CMD);
	SetFixedIndex(COM_SETGAMEPLUGINFILE);

	n = -1;
}

int llSetGamePluginFile::RegisterOptions(void) {
	if (!llWorker::RegisterOptions()) return 0;

	RegisterValue("-n",     &n);
	RegisterValue("-value", &name, LLWORKER_OBL_OPTION);

	return 1;
}

int llSetGamePluginFile::Exec(void) {
	llWorker::Exec();

	if (n<0 && !gamemode) {
		_llLogger()->WriteNextLine(LOG_ERROR, "Game number (-n) not set");
	} else if (n<0 && gamemode) {
		plugin[gamemode] = name;
	} else {
		if (n > MAX_GAMES) {
			_llLogger()->WriteNextLine(LOG_ERROR, "Too many games");
			return 0;
		}
		plugin[n] = name;
	}

	return 1;
}

/* ----------------------------------- */

llSetGameStdWorldspace::llSetGameStdWorldspace() : llWorker() {
	SetCommandName(COM_SETGAMESTDWS_CMD);
	SetFixedIndex(COM_SETGAMESTDWS);
}

int llSetGameStdWorldspace::RegisterOptions(void) {
	if (!llWorker::RegisterOptions()) return 0;

	RegisterValue("-n",     &n);
	RegisterValue("-value", &name, LLWORKER_OBL_OPTION);

	return 1;
}

int llSetGameStdWorldspace::Exec(void) {
	llWorker::Exec();

	if (n<0 && !gamemode) {
		_llLogger()->WriteNextLine(LOG_ERROR, "Game number (-n) not set");
	} else if (n<0 && gamemode) {
		std_ws[gamemode] = name;
	} else {
		if (n > MAX_GAMES) {
			_llLogger()->WriteNextLine(LOG_ERROR, "Too many games");
			return 0;
		}
		std_ws[n] = name;
	}

	return 1;
}

/* ----------------------------------- */

llSetGameSearchPattern::llSetGameSearchPattern() : llWorker() {
	SetCommandName(COM_SETGAMESEARCHPATTERN_CMD);
	SetFixedIndex(COM_SETGAMESEARCHPATTERN);
}

int llSetGameSearchPattern::RegisterOptions(void) {
	if (!llWorker::RegisterOptions()) return 0;

	RegisterValue("-n",     &n);
	RegisterValue("-value", &name, LLWORKER_OBL_OPTION);

	return 1;
}

int llSetGameSearchPattern::Exec(void) {
	llWorker::Exec();

	if (n<0 && !gamemode) {
		_llLogger()->WriteNextLine(LOG_ERROR, "Game number (-n) not set");
	} else if (n<0 && gamemode) {
		pattern[gamemode] = name;
	} else {
		if (n > MAX_GAMES) {
			_llLogger()->WriteNextLine(LOG_ERROR, "Too many games");
			return 0;
		}
		pattern[n] = name;
	}

	return 1;
}

/* ----------------------------------- */

llGUIConsoleEcho::llGUIConsoleEcho() : llWorker() {
	SetCommandName(COM_GUISPLASHECHO_CMD);
	SetFixedIndex(COM_GUISPLASHECHO);
}

int llGUIConsoleEcho::RegisterOptions(void) {
	if (!llWorker::RegisterOptions()) return 0;

	RegisterValue("-text",  &guitext, LLWORKER_OBL_OPTION);
	RegisterFlag("-switch", &myswitch);

	return 1;
}

int llGUIConsoleEcho::Prepare(void) {
	myswitch = 0;
	return 1;
}

/* ----------------------------------- */

llGUITab::llGUITab() : llWorker() {
	SetCommandName(COM_GUIAPPLICATION_CMD);
	SetFixedIndex(COM_GUIAPPLICATION);
}

int llGUITab::RegisterOptions(void) {
	if (!llWorker::RegisterOptions()) return 0;

	RegisterValue("-name",  &guiname);
	RegisterValue("-help",  &guihelp);
	RegisterValue("-text",  &guitext);

	return 1;
}

int llGUITab::Prepare(void) {
	if (!llWorker::Prepare()) return 0;

	guiname = "<Unknown>";
	guitab  = "<Unknown>";
	guitext = "<Undefined>";
	guihelp = "No help available";
	return 1;
}

/* ----------------------------------- */

llGUITextBox::llGUITextBox() : llWorker() {
	SetCommandName(COM_GUITEXTBOX_CMD);
	SetFixedIndex(COM_GUITEXTBOX);
}

int llGUITextBox::RegisterOptions(void) {
	if (!llWorker::RegisterOptions()) return 0;

	RegisterFlag("-input",     &textinput);
	RegisterFlag("-fileinput", &fileinput);
	RegisterFlag("-sameline",  &sameline);

	RegisterValue("-name",  &guiname, LLWORKER_OBL_OPTION);
	RegisterValue("-help",  &guihelp);
	RegisterValue("-text",  &guitext);

	RegisterValue("-vdist",  &vdist);
	RegisterValue("-width",  &guiwidth);
	RegisterValue("-height", &guiheight);

	return 1;
}

int llGUITextBox::Prepare(void) {
	if (!llWorker::Prepare()) return 0;

	guiname = "<Unknown>";
	guitext = "<Undefined>";
	guihelp = "No help available";

	textinput = fileinput = sameline = 0;
	vdist = 0;
	guiwidth  = 1;
	guiheight = 18;

	return 1;
}

/* ----------------------------------- */

llGUICheckBox::llGUICheckBox() : llWorker() {
	SetCommandName(COM_GUICHECKBOX_CMD);
	SetFixedIndex(COM_GUICHECKBOX);
}

int llGUICheckBox::RegisterOptions(void) {
	if (!llWorker::RegisterOptions()) return 0;

	RegisterFlag("-select",   &guienabled);
	RegisterFlag("-sameline", &sameline);

	RegisterValue("-name",  &guiname, LLWORKER_OBL_OPTION);
	RegisterValue("-help",  &guihelp);
	RegisterValue("-text",  &guitext);

	RegisterValue("-vdist",  &vdist);
	RegisterValue("-width",  &guiwidth);
	RegisterValue("-height", &guiheight);

	return 1;
}

int llGUICheckBox::Prepare(void) {
	if (!llWorker::Prepare()) return 0;

	guiname = "<Unknown>";
	guitext = "<Undefined>";
	guihelp = "No help available";

	guienabled = sameline = 0;
	vdist = 0;
	guiwidth  = 1;
	guiheight = 18;

	return 1;
}

/* ----------------------------------- */

llGUIDropDown::llGUIDropDown() : llWorker() {
	SetCommandName(COM_GUIDROPDOWN_CMD);
	SetFixedIndex(COM_GUIDROPDOWN);
}

int llGUIDropDown::RegisterOptions(void) {
	if (!llWorker::RegisterOptions()) return 0;

	RegisterFlag("-sameline", &sameline);

	RegisterValue("-name",  &guiname, LLWORKER_OBL_OPTION);
	RegisterValue("-help",  &guihelp);
	RegisterValue("-text",  &guitext);

	RegisterValue("-vdist",  &vdist);
	RegisterValue("-width",  &guiwidth);
	RegisterValue("-height", &guiheight);

	return 1;
}

int llGUIDropDown::Prepare(void) {
	if (!llWorker::Prepare()) return 0;

	guiname = "<Unknown>";
	guitext = "<Undefined>";
	guihelp = "No help available";

	sameline = 0;
	vdist = 0;
	guiwidth  = 1;
	guiheight = 18;

	return 1;
}

/* ----------------------------------- */

llGUIDropDownItem::llGUIDropDownItem() : llWorker() {
	SetCommandName(COM_GUIDROPDOWNITEM_CMD);
	SetFixedIndex(COM_GUIDROPDOWNITEM);
}

int llGUIDropDownItem::RegisterOptions(void) {
	if (!llWorker::RegisterOptions()) return 0;

	RegisterFlag("-select", &guienabled);

	RegisterValue("-name",   &guiname, LLWORKER_OBL_OPTION);
	RegisterValue("-parent", &guiparent, LLWORKER_OBL_OPTION);
	RegisterValue("-text",   &guitext);

	return 1;
}

int llGUIDropDownItem::Prepare(void) {
	if (!llWorker::Prepare()) return 0;

	guiname = "<Unknown>";
	guitext = "<Undefined>";

	guienabled = 0;

	return 1;
}

/* ----------------------------------- */

llGUIButton::llGUIButton() : llWorker() {
	SetCommandName(COM_GUIBUTTON_CMD);
	SetFixedIndex(COM_GUIBUTTON);
}

int llGUIButton::RegisterOptions(void) {
	if (!llWorker::RegisterOptions()) return 0;

	RegisterFlag("-sameline", &sameline);

	RegisterValue("-name",   &guiname, LLWORKER_OBL_OPTION);
	RegisterValue("-text",   &guitext);

	RegisterValue("-vdist",  &vdist);
	RegisterValue("-width",  &guiwidth);
	RegisterValue("-height", &guiheight);

	return 1;
}

int llGUIButton::Prepare(void) {
	if (!llWorker::Prepare()) return 0;

	guiname = "<Unknown>";
	guitext = "<Undefined>";

	sameline = 0;
	vdist = 0;
	guiwidth  = 1;
	guiheight = 20;

	return 1;
}

/* ----------------------------------- */

llGUIExec::llGUIExec() : llWorker() {
	SetCommandName(COM_GUIEXEC_CMD);
	SetFixedIndex(COM_GUIEXEC);
}

int llGUIExec::RegisterOptions(void) {
	if (!llWorker::RegisterOptions()) return 0;

	RegisterFlag("-dumpbatch", &dumpbatch);

	RegisterValue("-exe",   &guitext, LLWORKER_OBL_OPTION);

	return 1;
}

int llGUIExec::Prepare(void) {
	if (!llWorker::Prepare()) return 0;

	dumpbatch = 0;

	return 1;
}

/* ----------------------------------- */

llGUIEnable::llGUIEnable() : llWorker() {
	SetCommandName(COM_GUIENABLE_CMD);
	SetFixedIndex(COM_GUIENABLE);
}

int llGUIEnable::RegisterOptions(void) {
	if (!llWorker::RegisterOptions()) return 0;

	RegisterFlag("-unselect", &unselect);
	RegisterFlag("-select",   &guienabled);

	RegisterValue("-name", &guiname, LLWORKER_OBL_OPTION);

	return 1;
}

int llGUIEnable::Prepare(void) {
	if (!llWorker::Prepare()) return 0;

	unselect = guienabled = 0;

	return 1;
}

/* ----------------------------------- */

llGUIDisable::llGUIDisable() : llWorker() {
	SetCommandName(COM_GUIDISABLE_CMD);
	SetFixedIndex(COM_GUIDISABLE);
}

int llGUIDisable::RegisterOptions(void) {
	if (!llWorker::RegisterOptions()) return 0;

	RegisterFlag("-unselect", &unselect);
	RegisterFlag("-select",   &guienabled);

	RegisterValue("-name", &guiname, LLWORKER_OBL_OPTION);

	return 1;
}

int llGUIDisable::Prepare(void) {
	if (!llWorker::Prepare()) return 0;

	unselect = guienabled = 0;

	return 1;
}

/* ----------------------------------- */

llGUIMessageBox::llGUIMessageBox() : llWorker() {
	SetCommandName(COM_GUIMESSAGEBOX_CMD);
	SetFixedIndex(COM_GUIMESSAGEBOX);
}

int llGUIMessageBox::RegisterOptions(void) {
	if (!llWorker::RegisterOptions()) return 0;

	RegisterValue("-text",  &guitext, LLWORKER_OBL_OPTION);
	RegisterValue("-title", &guiname, LLWORKER_OBL_OPTION);

	return 1;
}

/* ----------------------------------- */

llGUIWindowSize::llGUIWindowSize() : llWorker() {
	SetCommandName(COM_GUIWINDOWSIZE_CMD);
	SetFixedIndex(COM_GUIWINDOWSIZE);
}

int llGUIWindowSize::RegisterOptions(void) {
	if (!llWorker::RegisterOptions()) return 0;

	RegisterValue("-x", &guiwidth,  LLWORKER_OBL_OPTION);
	RegisterValue("-y", &guiheight, LLWORKER_OBL_OPTION);

	return 1;
}

/* ----------------------------------- */

llGUIRequestVersion::llGUIRequestVersion() : llWorker() {
	SetCommandName(COM_GUIREQUESTVERSION_CMD);
	SetFixedIndex(COM_GUIREQUESTVERSION);
}

int llGUIRequestVersion::RegisterOptions(void) {
	if (!llWorker::RegisterOptions()) return 0;

	RegisterValue("-value", &value, LLWORKER_OBL_OPTION);

	return 1;
}

