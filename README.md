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

The next step is to search for the section [MPGUI] and executes the commands listed there. This aim of this section is to extent the GUI with tabs. Keep in mind that, if the user opens several batch files, these sections are merged. Therefore, the use of “doonce” variables is advised.




