#ifndef _PLLGUICOMMANDS_H_
#define _PLLGUICOMMANDS_H_

//GUI
#define COM_GUIAPPLICATION		1
#define COM_GUIAPPLICATION_CMD	"GUITab"
#define COM_GUITEXTBOX			2
#define COM_GUITEXTBOX_CMD		"GUITextBox"
#define COM_GUICHECKBOX			3
#define COM_GUICHECKBOX_CMD		"GUICheckBox"
#define COM_GUIDROPDOWN			4
#define COM_GUIDROPDOWN_CMD		"GUIDropDown"
#define COM_GUIDROPDOWNITEM		5
#define COM_GUIDROPDOWNITEM_CMD	"GUIDropDownItem"
#define COM_GUIBUTTON			6
#define COM_GUIBUTTON_CMD		"GUIButton"
#define COM_GUIEXEC				7
#define COM_GUIEXEC_CMD			"GUIExec"
#define COM_GUIREQUESTVERSION   8
#define COM_GUIREQUESTVERSION_CMD	"GUIRequestVersion"

#define COM_GUIENABLE			9
#define COM_GUIENABLE_CMD		"GUIEnable"
#define COM_GUIDISABLE			10
#define COM_GUIDISABLE_CMD		"GUIDisable"

#define COM_GUISPLASHECHO		11
#define COM_GUISPLASHECHO_CMD	"GUIConsoleEcho"
#define COM_GUIMESSAGEBOX		12
#define COM_GUIMESSAGEBOX_CMD	"GUIMessageBox"
#define COM_GUIWINDOWSIZE		13
#define COM_GUIWINDOWSIZE_CMD	"GUIWindowSize"


//misc
#define COM_SETOPTION				60
#define COM_SETOPTION_CMD			"SetOption"

#define COM_ADDGAME					64
#define COM_ADDGAME_CMD				"AddGame"
#define COM_SETGAMEPLUGINFILE		65
#define COM_SETGAMEPLUGINFILE_CMD	"SetGamePluginFile"
#define COM_SETGAMESEARCHPATTERN	66
#define COM_SETGAMESEARCHPATTERN_CMD	"SetGameSearchPattern"
#define COM_SETGAMESTDWS			67
#define COM_SETGAMESTDWS_CMD		"SetGameStdWorldspace"
#define COM_SETPATH					68
#define COM_SETPATH_CMD				"SetPath"
#define COM_GAMEMODE				69
#define COM_GAMEMODE_CMD			"GameMode"

#define COM_LOGFILE					70
#define COM_SETFLAG					71


#include "..\..\lltool\include\llworker.h"

extern int gamemode;
#define            MAX_GAMES           10
extern const char *game[MAX_GAMES];
extern const char *plugin[MAX_GAMES];
extern const char *pattern[MAX_GAMES];
extern const char *std_ws[MAX_GAMES];

extern float guiwidth, guiheight, vdist, value;
extern char *guiname, *guitab, *guitext, *guihelp, *guiparent;
extern char *flagtext;
extern int   myswitch, textinput, fileinput, sameline, guienabled, dumpbatch, unselect;

/* ----------------------------------- */

class llSetPath : public llWorker {

public:

	llSetPath();

	virtual llWorker * Clone() {
		return new llSetPath(*this);
	}

	virtual int RegisterOptions(void);
	virtual int Prepare(void);

};

/* ----------------------------------- */

class llGameMode : public llWorker {

protected:

	char *name;

public:

	llGameMode();

	virtual llWorker * Clone() {
		return new llGameMode(*this);
	}

	virtual int RegisterOptions(void);
	virtual int Exec(void);

};

/* ----------------------------------- */

class llAddGame : public llWorker {

protected:

	int   n;
	char *name;

public:

	llAddGame();

	virtual llWorker * Clone() {
		return new llAddGame(*this);
	}

	virtual int RegisterOptions(void);
	virtual int Exec(void);

};

/* ----------------------------------- */

class llSetGamePluginFile : public llWorker {

protected:

	int   n;
	char *name;

public:

	llSetGamePluginFile();

	virtual llWorker * Clone() {
		return new llSetGamePluginFile(*this);
	}

	virtual int RegisterOptions(void);
	virtual int Exec(void);

};

/* ----------------------------------- */

class llSetGameStdWorldspace : public llWorker {

protected:

	int   n;
	char *name;

public:

	llSetGameStdWorldspace();

	virtual llWorker * Clone() {
		return new llSetGameStdWorldspace(*this);
	}

	virtual int RegisterOptions(void);
	virtual int Exec(void);

};

/* ----------------------------------- */

class llSetGameSearchPattern : public llWorker {

protected:

	int   n;
	char *name;

public:

	llSetGameSearchPattern();

	virtual llWorker * Clone() {
		return new llSetGameSearchPattern(*this);
	}

	virtual int RegisterOptions(void);
	virtual int Exec(void);

};

/* ----------------------------------- */

class llGUIConsoleEcho : public llWorker {

public:

	llGUIConsoleEcho();

	virtual llWorker * Clone() {
		return new llGUIConsoleEcho(*this);
	}

	virtual int RegisterOptions(void);
	virtual int Prepare(void);

};

/* ----------------------------------- */

class llGUITab : public llWorker {

public:

	llGUITab();

	virtual llWorker * Clone() {
		return new llGUITab(*this);
	}

	virtual int RegisterOptions(void);
	virtual int Prepare(void);

};

/* ----------------------------------- */

class llGUITextBox : public llWorker {

public:

	llGUITextBox();

	virtual llWorker * Clone() {
		return new llGUITextBox(*this);
	}

	virtual int RegisterOptions(void);
	virtual int Prepare(void);

};

/* ----------------------------------- */

class llGUICheckBox : public llWorker {

public:

	llGUICheckBox();

	virtual llWorker * Clone() {
		return new llGUICheckBox(*this);
	}

	virtual int RegisterOptions(void);
	virtual int Prepare(void);

};


/* ----------------------------------- */

class llGUIDropDown : public llWorker {

public:

	llGUIDropDown();

	virtual llWorker * Clone() {
		return new llGUIDropDown(*this);
	}

	virtual int RegisterOptions(void);
	virtual int Prepare(void);

};

/* ----------------------------------- */

class llGUIDropDownItem : public llWorker {

public:

	llGUIDropDownItem();

	virtual llWorker * Clone() {
		return new llGUIDropDownItem(*this);
	}

	virtual int RegisterOptions(void);
	virtual int Prepare(void);

};

/* ----------------------------------- */

class llGUIButton : public llWorker {

public:

	llGUIButton();

	virtual llWorker * Clone() {
		return new llGUIButton(*this);
	}

	virtual int RegisterOptions(void);
	virtual int Prepare(void);

};

/* ----------------------------------- */

class llGUIExec : public llWorker {

public:

	llGUIExec();

	virtual llWorker * Clone() {
		return new llGUIExec(*this);
	}

	virtual int RegisterOptions(void);
	virtual int Prepare(void);

};

/* ----------------------------------- */

class llGUIEnable : public llWorker {

public:

	llGUIEnable();

	virtual llWorker * Clone() {
		return new llGUIEnable(*this);
	}

	virtual int RegisterOptions(void);
	virtual int Prepare(void);

};

/* ----------------------------------- */

class llGUIDisable : public llWorker {

public:

	llGUIDisable();

	virtual llWorker * Clone() {
		return new llGUIDisable(*this);
	}

	virtual int RegisterOptions(void);
	virtual int Prepare(void);
};

/* ----------------------------------- */

class llGUIMessageBox : public llWorker {

public:

	llGUIMessageBox();

	virtual llWorker * Clone() {
		return new llGUIMessageBox(*this);
	}

	virtual int RegisterOptions(void);
};

/* ----------------------------------- */

class llGUIWindowSize : public llWorker {

public:

	llGUIWindowSize();

	virtual llWorker * Clone() {
		return new llGUIWindowSize(*this);
	}

	virtual int RegisterOptions(void);
};

/* ----------------------------------- */

class llGUIRequestVersion : public llWorker {

public:

	llGUIRequestVersion();

	virtual llWorker * Clone() {
		return new llGUIRequestVersion(*this);
	}

	virtual int RegisterOptions(void);
};

#endif
