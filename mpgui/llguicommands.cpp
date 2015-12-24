#include "StdAfx.h"

#include "..\..\lltool\include\lllogger.h"
#include "..\..\lltool\include\llutils.h"
#include "llguicommands.h"

int gamemode = 0;
const char *game[MAX_GAMES];
const char *plugin[MAX_GAMES];
const char *pattern[MAX_GAMES];
const char *std_ws[MAX_GAMES];

float guiwidth, guiheight, vdist;
char *guiname, *guitab, *guitext, *guihelp, *guiparent;
char *flagtext;
int   myswitch, textinput, fileinput, sameline, guienabled;

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
	guitab  = "<Unknown>";
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
	guitab  = "<Unknown>";
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
	guitab  = "<Unknown>";
	guitext = "<Undefined>";
	guihelp = "No help available";

	sameline = 0;
	vdist = 0;
	guiwidth  = 1;
	guiheight = 18;

	return 1;
}


#if 0



		if (com == COM_GUIDROPDOWNITEM) {
			CurrentCommand = COM_GUIDROPDOWNITEM_CMD;
			if (_stricmp(ptr,"-select")==0) {
				guienabled=1;
			} else {
				ptr2 = strtok_int(ptr, '=',&saveptr2);
				if (ptr2!=NULL && strlen(ptr2)>0) {
					if (_stricmp(ptr2,"-text")==0) {
						ptr2 = strtok_int(NULL, '=',&saveptr2);
						if (ptr2) {
							guitext=new char[strlen(ptr2)+1];strcpy_s(guitext,strlen(ptr2)+1,ptr2);strip_quot(&guitext);
						} else {
							mesg->WriteNextLine(MH_ERROR,CM_SYNTAX_ERROR,ptr,CurrentCommand);return com;
						}
					} else if (_stricmp(ptr2,"-name")==0) {
						ptr2 = strtok_int(NULL, '=',&saveptr2);
						if (ptr2) {
							guiname=new char[strlen(ptr2)+1];strcpy_s(guiname,strlen(ptr2)+1,ptr2);strip_quot(&guiname);
						} else {
							mesg->WriteNextLine(MH_ERROR,CM_SYNTAX_ERROR,ptr,CurrentCommand);return com;
						}
					} else if (_stricmp(ptr2,"-parent")==0) {
						ptr2 = strtok_int(NULL, '=',&saveptr2);
						if (ptr2) {
							guiparent=new char[strlen(ptr2)+1];strcpy_s(guiparent,strlen(ptr2)+1,ptr2);strip_quot(&guiparent);
						} else {
							mesg->WriteNextLine(MH_ERROR,CM_SYNTAX_ERROR,ptr,CurrentCommand);return com;
						}
					} else {
						mesg->WriteNextLine(MH_ERROR,CM_UNKNOWN_OPTION,ptr,CurrentCommand);return com;
					}
				} else {
					mesg->WriteNextLine(MH_ERROR,CM_INVALID_OPTION,ptr,CurrentCommand);return com;
				}
			}
		}

		if (com == COM_GUIBUTTON) {
			CurrentCommand = COM_GUIBUTTON_CMD;
			ptr2 = strtok_int(ptr, '=',&saveptr2);
			if (ptr2!=NULL && strlen(ptr2)>0) {
				if (_stricmp(ptr2,"-text")==0) {
					ptr2 = strtok_int(NULL, '=',&saveptr2);
					if (ptr2) {
						guitext=new char[strlen(ptr2)+1];strcpy_s(guitext,strlen(ptr2)+1,ptr2);strip_quot(&guitext);
					} else {
						mesg->WriteNextLine(MH_ERROR,CM_SYNTAX_ERROR,ptr,CurrentCommand);return com;
					}
				} else if (_stricmp(ptr2,"-name")==0) {
					ptr2 = strtok_int(NULL, '=',&saveptr2);
					if (ptr2) {
						guiname=new char[strlen(ptr2)+1];strcpy_s(guiname,strlen(ptr2)+1,ptr2);strip_quot(&guiname);
					} else {
						mesg->WriteNextLine(MH_ERROR,CM_SYNTAX_ERROR,ptr,CurrentCommand);return com;
					}
				} else if (_stricmp(ptr2,"-vdist")==0) {
					ptr2 = strtok_int(NULL, '=',&saveptr2);
					if (ptr2)
						sscanf_s(ptr2,"%i",&vdist);
					else {
						mesg->WriteNextLine(MH_ERROR,CM_SYNTAX_ERROR,ptr,CurrentCommand);return com;
					}
				} else {
					mesg->WriteNextLine(MH_ERROR,CM_UNKNOWN_OPTION,ptr,CurrentCommand);return com;
				}
			} 
		}

		if (com == COM_GUIEXEC) {
			CurrentCommand = COM_GUIEXEC_CMD;
			if (_stricmp(ptr,"-dumpbatch")==0) {
				dumpbatch=1;
			} else {
				ptr2 = strtok_int(ptr, '=',&saveptr2);
				if (ptr2!=NULL && strlen(ptr2)>0) {
					if (_stricmp(ptr2,"-exe")==0) {
						ptr2 = strtok_int(NULL, '=',&saveptr2);
						if (ptr2) {
							guitext=new char[strlen(ptr2)+1];strcpy_s(guitext,strlen(ptr2)+1,ptr2);strip_quot(&guitext);
						} else {
							mesg->WriteNextLine(MH_ERROR,CM_SYNTAX_ERROR,ptr,CurrentCommand);return com;
						}
					} else {
						mesg->WriteNextLine(MH_ERROR,CM_UNKNOWN_OPTION,ptr,CurrentCommand);return com;
					}
				} else {
					mesg->WriteNextLine(MH_ERROR,CM_INVALID_OPTION,ptr,CurrentCommand);return com;
				}
			}
		}

		if (com == COM_GUIENABLE) {
			CurrentCommand = COM_GUIENABLE_CMD;
			if (_stricmp(ptr,"-unselect")==0) {
				unselect=1;
			} else if (_stricmp(ptr,"-select")==0) {
				guienabled=1;
			} else {
				ptr2 = strtok_int(ptr, '=',&saveptr2);
				if (ptr2!=NULL && strlen(ptr2)>0) {
					if (_stricmp(ptr2,"-name")==0) {
						ptr2 = strtok_int(NULL, '=',&saveptr2);
						if (ptr2) {
							guiname=new char[strlen(ptr2)+1];strcpy_s(guiname,strlen(ptr2)+1,ptr2);strip_quot(&guiname);
						} else {
							mesg->WriteNextLine(MH_ERROR,CM_SYNTAX_ERROR,ptr,CurrentCommand);return com;
						}
					} else {
						mesg->WriteNextLine(MH_ERROR,CM_UNKNOWN_OPTION,ptr,CurrentCommand);return com;
					}
				} else {
					mesg->WriteNextLine(MH_ERROR,CM_INVALID_OPTION,ptr,CurrentCommand);return com;
				}
			}
		}

		if (com == COM_GUIDISABLE) {
			CurrentCommand = COM_GUIDISABLE_CMD;
			if (_stricmp(ptr,"-unselect")==0) {
				unselect=1;
			} else if (_stricmp(ptr,"-select")==0) {
				guienabled=1;
			} else {
				ptr2 = strtok_int(ptr, '=',&saveptr2);
				if (ptr2!=NULL && strlen(ptr2)>0) {
					if (_stricmp(ptr2,"-name")==0) {
						ptr2 = strtok_int(NULL, '=',&saveptr2);
						if (ptr2) {
							guiname=new char[strlen(ptr2)+1];strcpy_s(guiname,strlen(ptr2)+1,ptr2);strip_quot(&guiname);
						} else {
							mesg->WriteNextLine(MH_ERROR,CM_SYNTAX_ERROR,ptr,CurrentCommand);return com;
						}
					} else {
						mesg->WriteNextLine(MH_ERROR,CM_UNKNOWN_OPTION,ptr,CurrentCommand);return com;
					}
				} else {
					mesg->WriteNextLine(MH_ERROR,CM_INVALID_OPTION,ptr,CurrentCommand);return com;
				}
			}
		}

		if (com == COM_GUISPLASHECHO) {
			CurrentCommand = COM_GUISPLASHECHO_CMD;
			if (_stricmp(ptr,"-switch")==0) {
				myswitch=1;
			} else {
				ptr2 = strtok_int(ptr, '=',&saveptr2);
				if (ptr2!=NULL && strlen(ptr2)>0) {
					if (_stricmp(ptr2,"-text")==0) {
						ptr2 = strtok_int(NULL, '=',&saveptr2);
						if (ptr2) {
							guitext=new char[strlen(ptr2)+1];strcpy_s(guitext,strlen(ptr2)+1,ptr2);strip_quot(&guitext);
						} else {
							mesg->WriteNextLine(MH_ERROR,CM_SYNTAX_ERROR,ptr,CurrentCommand);return com;
						}
					} else {
						mesg->WriteNextLine(MH_ERROR,CM_UNKNOWN_OPTION,ptr,CurrentCommand);return com;
					}
				} else {
					mesg->WriteNextLine(MH_ERROR,CM_INVALID_OPTION,ptr,CurrentCommand);return com;
				}
			}
		}

		if (com == COM_GUIMESSAGEBOX) {
			CurrentCommand = COM_GUIWINDOWSIZE_CMD;
			ptr2 = strtok_int(ptr, '=',&saveptr2);
			if (ptr2!=NULL && strlen(ptr2)>0) {
				if (_stricmp(ptr2,"-text")==0) {
					ptr2 = strtok_int(NULL, '=',&saveptr2);
					if (ptr2) {
						guitext=new char[strlen(ptr2)+1];strcpy_s(guitext,strlen(ptr2)+1,ptr2);strip_quot(&guitext);
					} else {
						mesg->WriteNextLine(MH_ERROR,CM_SYNTAX_ERROR,ptr,CurrentCommand);return com;
					}
				} else if (_stricmp(ptr2,"-title")==0) {
					ptr2 = strtok_int(NULL, '=',&saveptr2);
					if (ptr2) {
						guiname=new char[strlen(ptr2)+1];strcpy_s(guiname,strlen(ptr2)+1,ptr2);strip_quot(&guiname);
					} else {
						mesg->WriteNextLine(MH_ERROR,CM_SYNTAX_ERROR,ptr,CurrentCommand);return com;
					}
				} else {
					mesg->WriteNextLine(MH_ERROR,CM_UNKNOWN_OPTION,ptr,CurrentCommand);return com;
				}
			} else {
				mesg->WriteNextLine(MH_ERROR,CM_INVALID_OPTION,ptr,CurrentCommand);return com;
			}
		}

		if (com == COM_GUIWINDOWSIZE) {
			CurrentCommand = 
			ptr2 = strtok_int(ptr, '=',&saveptr2);
			if (ptr2!=NULL && strlen(ptr2)>0) {
				if (_stricmp(ptr2,"-x")==0) {
					ptr2 = strtok_int(NULL, '=',&saveptr2);
					if (ptr2)
						sscanf_s(ptr2,"%f",&gridx);
					else {
						mesg->WriteNextLine(MH_ERROR,CM_SYNTAX_ERROR,ptr,CurrentCommand);return com;
					}
				}
				else if (_stricmp(ptr2,"-y")==0) {
					ptr2 = strtok_int(NULL, '=',&saveptr2);
					if (ptr2)
						sscanf_s(ptr2,"%f",&gridy);
					else {
						mesg->WriteNextLine(MH_ERROR,CM_SYNTAX_ERROR,ptr,CurrentCommand);return com;
					}
				} else {
					mesg->WriteNextLine(MH_ERROR,CM_UNKNOWN_OPTION,ptr,CurrentCommand);return com;
				}
			} 
			else {
				mesg->WriteNextLine(MH_ERROR,CM_INVALID_OPTION,ptr,CurrentCommand);return com;
			}
		}


		if (com == COM_GUIREQUESTVERSION) {			
			CurrentCommand = COM_GUIREQUESTVERSION_CMD;
			ptr2 = strtok_int(ptr, '=',&saveptr2);
			if (ptr2!=NULL && strlen(ptr2)>0) {
				if (_stricmp(ptr2,"-value")==0) {
					ptr2 = strtok_int(NULL, '=',&saveptr2);
					if (ptr2) {
						myflagname=new char[strlen(ptr2)+1];strcpy_s(myflagname,strlen(ptr2)+1,ptr2);strip_quot(&myflagname);
					} else {
						mesg->WriteNextLine(MH_ERROR,CM_SYNTAX_ERROR,ptr,CurrentCommand);return com;
					}
				} else {
					mesg->WriteNextLine(MH_ERROR,"Unknown option [%s] after [GUIRequestVersion]",ptr);return com;
				}
			} else {
				mesg->WriteNextLine(MH_ERROR,CM_INVALID_OPTION,ptr,CurrentCommand);return com;
			}			
		}




		if (com == COM_SETOPTION) {
			CurrentCommand = COM_SETOPTION_CMD;
			if (_stricmp(ptr,"-createpedestals")==0) {
				mesg->WriteNextLine(MH_COMMAND,"SetOption -createpedestals");
				createpedestals=1;
			} else if (_stricmp(ptr,"-useshapes")==0) {
				mesg->WriteNextLine(MH_COMMAND,"SetOption -useshapes");
				useshapes=1;
			} else if (_stricmp(ptr,"-use16bit")==0) {
				mesg->WriteNextLine(MH_COMMAND,"SetOption -use16bit");
				use16bit=1;
			} else if (_stricmp(ptr,"-lodshadows")==0) {
				mesg->WriteNextLine(MH_COMMAND,"SetOption -lodshadows");
				lodshadows=1;
			} else if (_stricmp(ptr,"-gamemodeoblivion")==0) {
				mesg->WriteNextLine(MH_COMMAND,"SetOption -gamemodeoblivion");
				gamemode=2;
			} else if (_stricmp(ptr,"-gamemodeskyrim")==0) {
				mesg->WriteNextLine(MH_COMMAND,"SetOption -gamemodeskyrim");
				gamemode=5;
			} else if (_stricmp(ptr,"-noskipinfo")==0) {		
				mesg->WriteNextLine(MH_COMMAND,"SetOption -noskipinfo");
				noskipinfo=1;
			} else if (_stricmp(ptr,"-center")==0) {		
				mesg->WriteNextLine(MH_COMMAND,"SetOption -center");
				center=1;
			} else {
				ptr2 = strtok_int(ptr, '=',&saveptr2);
				if (ptr2!=NULL && strlen(ptr2)>0) {
					if (_stricmp(ptr2,"-mindistance")==0) {
						ptr2 = strtok_int(NULL, '=',&saveptr2);
						strip_quot(&ptr2);
						if (ptr2)
							sscanf_s(ptr2,"%i",&mindistance);
						else {
							mesg->WriteNextLine(MH_ERROR,CM_SYNTAX_ERROR,ptr,CurrentCommand);return com;
						}
					} else if (_stricmp(ptr2,"-zboost")==0) {
						ptr2 = strtok_int(NULL, '=',&saveptr2);
						strip_quot(&ptr2);
						if (ptr2)
							sscanf_s(ptr2,"%f",&overdrawing);
						else {
							mesg->WriteNextLine(MH_ERROR,CM_SYNTAX_ERROR,ptr,CurrentCommand);return com;
						}
					} else if (_stricmp(ptr2,"-nquadmax")==0) {
						ptr2 = strtok_int(NULL, '=',&saveptr2);
						strip_quot(&ptr2);
						if (ptr2)
							sscanf_s(ptr2,"%i",&nquadmax);
						else {
							mesg->WriteNextLine(MH_ERROR,CM_SYNTAX_ERROR,ptr,CurrentCommand);return com;
						}
					} else if (_stricmp(ptr2,"-sizex")==0) {
						ptr2 = strtok_int(NULL, '=',&saveptr2);
						strip_quot(&ptr2);
						if (ptr2)
							sscanf_s(ptr2,"%i",&size_x);
						else {
							mesg->WriteNextLine(MH_ERROR,CM_SYNTAX_ERROR,ptr,CurrentCommand);return com;
						}
					} else if (_stricmp(ptr2,"-sizey")==0) {
						ptr2 = strtok_int(NULL, '=',&saveptr2);
						strip_quot(&ptr2);
						if (ptr2)
							sscanf_s(ptr2,"%i",&size_y);
						else {
							mesg->WriteNextLine(MH_ERROR,CM_SYNTAX_ERROR,ptr,CurrentCommand);return com;
						}
					} else if (_stricmp(ptr2,"-worldspaceid")==0) {
						ptr2 = strtok_int(NULL, '=',&saveptr2);
						strip_quot(&ptr2);
						if (ptr2) {
							mesg->WriteNextLine(MH_INFO,"SetOption -worldspaceid=%s", ptr2);
							worldname=new char[strlen(ptr2)+1];strcpy_s(worldname,strlen(ptr2)+1,ptr2);strip_quot(&worldname);
						} else {
							mesg->WriteNextLine(MH_ERROR,CM_SYNTAX_ERROR,ptr,CurrentCommand);return com;
						}
					} else if (_stricmp(ptr2,"-installdirectory")==0) {					
						ptr2 = strtok_int(NULL, '=',&saveptr2);
						strip_quot(&ptr2);
						if (ptr2) {
							mesg->WriteNextLine(MH_INFO,"SetOption -installdirectory=%s", ptr2);
							install_dir=new char[strlen(ptr2)+1];strcpy_s(install_dir,strlen(ptr2)+1,ptr2);strip_quot(&install_dir);
							if (strlen(install_dir)>0) {
								for (unsigned int i=0;i<strlen(install_dir);i++) {
									if (install_dir[i]=='\\') {
										install_dir[i]='\0';
										_mkdir(install_dir);
										install_dir[i]='\\';
									}
								}
								_mkdir(install_dir);
							}
						} else {
							mesg->WriteNextLine(MH_ERROR,CM_SYNTAX_ERROR,ptr,CurrentCommand);return com;
						}
					} else if (_stricmp(ptr2,"-ddstool")==0) {					
						ptr2 = strtok_int(NULL, '=',&saveptr2);
						strip_quot(&ptr2);
						if (ptr2) {
							mesg->WriteNextLine(MH_INFO,"SetOption -ddstool=%s", ptr2);
							dds_tool=new char[strlen(ptr2)+1];strcpy_s(dds_tool,strlen(ptr2)+1,ptr2);strip_quot(&dds_tool);
						} else {
							mesg->WriteNextLine(MH_ERROR,CM_SYNTAX_ERROR,ptr,CurrentCommand);return com;
						}
					} else if (_stricmp(ptr2,"-minheight")==0) {					
						ptr2 = strtok_int(NULL, '=',&saveptr2);
						strip_quot(&ptr2);
						if (ptr2) {
							mesg->WriteNextLine(MH_INFO,"SetOption -minheight=%s", ptr2);
							sscanf_s(ptr2,"%f",&minheight);
						} else {
							mesg->WriteNextLine(MH_ERROR,CM_SYNTAX_ERROR,ptr,CurrentCommand);return com;
						}
					} else if (_stricmp(ptr2,"-bmpscaling")==0) {					
						ptr2 = strtok_int(NULL, '=',&saveptr2);
						strip_quot(&ptr2);
						if (ptr2) {
							mesg->WriteNextLine(MH_INFO,"SetOption -bmpscaling=%s", ptr2);
							sscanf_s(ptr2,"%f",&bmpscaling);
						} else {
							mesg->WriteNextLine(MH_ERROR,CM_SYNTAX_ERROR,ptr,CurrentCommand);return com;
						}
					} else if (_stricmp(ptr2,"-quadtreelevels")==0) {					
						ptr2 = strtok_int(NULL, '=',&saveptr2);
						strip_quot(&ptr2);
						if (ptr2) {
							sscanf_s(ptr2,"%f",&quadtreelevels);
						} else {
							mesg->WriteNextLine(MH_ERROR,CM_SYNTAX_ERROR,ptr,CurrentCommand);return com;
						}
					} else {
						mesg->WriteNextLine(MH_ERROR,CM_UNKNOWN_OPTION,ptr,CurrentCommand);return com;
					}
				} else {
					mesg->WriteNextLine(MH_ERROR,CM_INVALID_OPTION,ptr,CurrentCommand);return com;
				}
			}
		}









	if (com == COM_GUIREQUESTVERSION) {
		if (!myflagname) {
			mesg->WriteNextLine(MH_ERROR,"%s: missing -value", CurrentCommand);
			myflagname="0.0";
		} 
	}

	if (com == COM_SETFLAG) {
		if (!myflagname) {
			mesg->WriteNextLine(MH_ERROR,"%s: missing -name",COM_SETFLAG_CMD);
		} else {
			if (myflagvalue) {
				SetValue(myflagname,myflagvalue);
				if (hidden) SetHidden(myflagname);
			} else {
				AddFlag(myflagname);
				if (hidden) SetHidden(myflagname);
			} 
			if (unselect) DisableFlag(myflagname);
		}
	}




#endif
