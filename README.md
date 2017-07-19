# Multi Purpose GUI - Reference Manual

## Introduction

MPGUI (Multi Purpose GUI) is a graphical frontend for various command-line based tools such as Lightwaves TESAnnwyn or TES4qLOD or [tes4ll](https://github.com/Gruftikus/tes4ll). 
In order to be flexible for possible new versions of each of the tools, MPGUI does not use hard coded elements, 
but it is based on a scripting language (Multi Purpose Batch, or just “batch”). In fact the [syntax](https://github.com/Gruftikus/lltool/wiki/Batch-Syntax) is the same as used for tes4ll/lltool.

This manual describes the GUI-syntax of the batch files (*.mpb).

## The start sequence

The first step after starting MPGUI is that the auto load batch file (mpgui_autoload.mpb) is loaded. If the  orking directory has an auto load file, this file is used, otherwise the standard one of the installation.

## Usage

MPGUI can be either started by hand, and any batch file can be loaded via "Open batch", or by making a double-click on the batch file. After the batch file has been read, MPGUI will examine its section [\_gamemode]. This allows to extent or overwrite settings done in the auto load file.

In the following, MPGUI tries to evaluate the game mode, by comparing the current working directory to the defined search pattern defined by SetGameSearchPattern in the auto load file. This search for the game mode is disabled, if one adds the lines

`[_gamemode]`

`GameMode -name=”...”`

in the batch file. This can be helpful if MPGUI is not able to determine the game mode.

The next step is that the tools searches for the section [MPGUI] in the user batch file and executes the commands which are listed there. This aim of this section is to extent the GUI with tabs. Keep in mind that, if the user opens several batch files, these sections are merged. Therefore, the use of “doonce” variables is advised.

## The GUI elements

The user batch file also describes which GUI elements are shown, and their content. In most of the GUI commands, a unique name has to be defined, which is batch flag name as well. This means, by defining a GUI element, a corresponding flag will be created and stored. These two items are correlated: by changing the GUI content, the flag value is changed, and vice versa.

Also the tabs itself have unique names, which is defined in the [MPGUI] section with the GUITab command. Here, the name is connected to a specific section with the same name, where the elements of the tab are defined.

## The tab sections

The tab sections itself are evaluated directly after the batch file has been loaded, and the GUITab command connects the tab to its section. The section is re-evaluated each time, after GUI elements have been changed by the user.

# Command list

## Game management

* [`AddGame`](https://github.com/Gruftikus/lltool/wiki/AddGame)
* [`SetGamePluginFile`](https://github.com/Gruftikus/lltool/wiki/SetGamePluginFile)
* [`SetGameSearchPattern`](https://github.com/Gruftikus/lltool/wiki/SetGameSearchPattern)
* [`SetGameStdWorldspace`](https://github.com/Gruftikus/lltool/wiki/SetGameStdWorldspace)
* [`GameMode`](https://github.com/Gruftikus/lltool/wiki/GameMode)

## Miscellaneous GUI commands

* [`GUIRequestVersion`](https://github.com/Gruftikus/lltool/wiki/GUIRequestVersion)
* [`GUIConsoleEcho`](https://github.com/Gruftikus/lltool/wiki/GUIConsoleEcho)
* [`GUIMessageBox`](https://github.com/Gruftikus/lltool/wiki/GUIMessageBox)
* [`GUIExec`](https://github.com/Gruftikus/lltool/wiki/GUIExec)

## Graphics GUI commands

* [`GUITextBox`](https://github.com/Gruftikus/lltool/wiki/GUITextBox)
* [`GUICheckBox`](https://github.com/Gruftikus/lltool/wiki/GUICheckBox)
* [`GUIDropDown`](https://github.com/Gruftikus/lltool/wiki/GUIDropDown)
* [`GUIDropDownItem`](https://github.com/Gruftikus/lltool/wiki/GUIDropDownItem)
* [`GUIButton`](https://github.com/Gruftikus/lltool/wiki/GUIButton)
* [`GUIEnable`](https://github.com/Gruftikus/lltool/wiki/GUIEnable)
* [`GUIDisable`](https://github.com/Gruftikus/lltool/wiki/GUIDisable)







