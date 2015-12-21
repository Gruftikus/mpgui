// mpgui.cpp : main project file.

#include "stdafx.h"
#include "Form1.h"

using namespace mpgui;

[STAThreadAttribute]
int main(array<System::String ^> ^args) {
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	Form1 ^form;
	if (args->Length == 1) {
		form = gcnew Form1(gcnew String(args[0]));
	} else {
		form = gcnew Form1(nullptr);
	} 

	//Form1 ^form = gcnew Form1(args);
	//if (args->Length == 1) form->OpenFile(args[0]);
	//Application::Idle += gcnew EventHandler(form,&Form1::Application_Idle);

	// Create the main window and run it
	Application::Run(form);
	return 0;
}
