#include <iostream>
#include "windows.h"
#include <direct.h>
#include <winreg.h>
#include <string>
#include <msclr/marshal.h> 

#include "..\..\lltool\include\lllogger.h"
#include "..\..\lltool\include\llutils.h"
#include "..\..\lltool\include\llcommands.h"
#include "..\..\lltool\include\lllogfile.h"
#include "..\..\lltool\include\llsetflag.h"

#include "llguicommands.h"

#include "tes4qlod_small.h"
#include "info.h"

#pragma once

namespace mpgui {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::IO;

	HANDLE g_hChildStd_OUT_Rd = NULL;
	HANDLE g_hChildStd_ERR_Rd = NULL;
	HANDLE g_hChildStd_IN_Wr = NULL;

	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(String ^args)
		{
			InitializeComponent();

			myBatchStream = nullptr;
			s_listname = nullptr;
			l_listname = nullptr;
			ws_primary = nullptr;
			searchdir  = gcnew String(Directory::GetCurrentDirectory());
			is_in_update =  0;
			child_pid    = -1;
			plugins_done =  0;
			gamemode     =  0;
			this->tab_plugins->Enabled = false;
			this->tab_ws->Enabled = false;
			cancelProcessToolStripMenuItem->Enabled = false;

			mesg  = _llLogger();
			mesg->NoAutoDump();

			batch = new llCommands();
			batch->NoWorkerPrint();
			llLogFile *LogFile = new llLogFile();
			LogFile->SetFixedIndex(COM_LOGFILE);
			batch->RegisterWorker(LogFile);
			llSetFlag *SetFlag = new llSetFlag();
			SetFlag->SetFixedIndex(COM_SETFLAG);
			batch->RegisterWorker(SetFlag);
			batch->RegisterWorker(new llGameMode());
			batch->RegisterWorker(new llAddGame());
			batch->RegisterWorker(new llSetGamePluginFile());
			batch->RegisterWorker(new llSetGameLoadorderFile());
			batch->RegisterWorker(new llSetGameStdWorldspace());
			batch->RegisterWorker(new llSetGameSearchPattern());
			batch->RegisterWorker(new llGUIConsoleEcho());
			batch->RegisterWorker(new llGUITab());
			batch->RegisterWorker(new llGUITextBox());
			batch->RegisterWorker(new llGUICheckBox());
			batch->RegisterWorker(new llGUIDropDown());
			batch->RegisterWorker(new llGUIDropDownItem());
			batch->RegisterWorker(new llGUIButton());
			batch->RegisterWorker(new llGUIExec());
			batch->RegisterWorker(new llGUIEnable());
			batch->RegisterWorker(new llGUIDisable());
			batch->RegisterWorker(new llGUIMessageBox());
			batch->RegisterWorker(new llGUIRequestVersion());
			batch->RegisterWorker(new llSetPath());
			batch->RegisterWorker(new llSetOption());

			textBox1->AppendText("This is Gruftikus' Multi Purpose GUI. Hello!" + Environment::NewLine );

			checkedListBox1->CheckOnClick = true;

			tab_app = gcnew cli::array<System::Windows::Forms::TabPage^>(0);

			backgroundWorker1 = gcnew System::ComponentModel::BackgroundWorker;
			backgroundWorker1->DoWork += gcnew DoWorkEventHandler( this, &Form1::backgroundWorker1_DoWork );
			backgroundWorker1->RunWorkerCompleted += gcnew RunWorkerCompletedEventHandler( this, &Form1::backgroundWorker1_RunWorkerCompleted );
			backgroundWorker1->ProgressChanged += gcnew ProgressChangedEventHandler( this, &Form1::backgroundWorker1_ProgressChanged );
			backgroundWorker1->WorkerReportsProgress = true;
			backgroundWorker1->WorkerSupportsCancellation = true;

			backgroundWorker1a = gcnew System::ComponentModel::BackgroundWorker;
			backgroundWorker1a->DoWork += gcnew DoWorkEventHandler( this, &Form1::backgroundWorker1a_DoWork );
			backgroundWorker1a->RunWorkerCompleted += gcnew RunWorkerCompletedEventHandler( this, &Form1::backgroundWorker1a_RunWorkerCompleted );
			backgroundWorker1a->ProgressChanged += gcnew ProgressChangedEventHandler( this, &Form1::backgroundWorker1a_ProgressChanged );
			backgroundWorker1a->WorkerReportsProgress = true;
			backgroundWorker1a->WorkerSupportsCancellation = true;

			backgroundWorker2 = gcnew System::ComponentModel::BackgroundWorker;
			backgroundWorker2->DoWork += gcnew DoWorkEventHandler( this, &Form1::backgroundWorker2_DoWork );
			backgroundWorker2->RunWorkerCompleted += gcnew RunWorkerCompletedEventHandler( this, &Form1::backgroundWorker2_RunWorkerCompleted );
			backgroundWorker2->WorkerReportsProgress = false;
			backgroundWorker2->WorkerSupportsCancellation = true;

			backgroundWorker3 = gcnew System::ComponentModel::BackgroundWorker;
			backgroundWorker3->DoWork += gcnew DoWorkEventHandler( this, &Form1::backgroundWorker3_DoWork );
			backgroundWorker3->RunWorkerCompleted += gcnew RunWorkerCompletedEventHandler( this, &Form1::backgroundWorker3_RunWorkerCompleted );
			backgroundWorker3->ProgressChanged += gcnew ProgressChangedEventHandler( this, &Form1::backgroundWorker3_ProgressChanged );
			backgroundWorker3->WorkerReportsProgress = true;
			backgroundWorker3->WorkerSupportsCancellation = true;

			folderBrowserDialog1 = gcnew FolderBrowserDialog;

			this->gameModeToolStripMenuItem->Enabled = false;
			this->loadOrderToolStripMenuItem->Enabled = false;

			using namespace System;
			using namespace Microsoft::Win32;

			RegistryKey ^rk = nullptr;
			rk = Registry::CurrentUser->OpenSubKey("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders");
			if (rk == nullptr) {
				MessageBox::Show("Fatal Error", "Fatal Error: Registry key not found", 
					MessageBoxButtons::AbortRetryIgnore, MessageBoxIcon::Stop );
			} else {
				String ^rgValue = gcnew String(rk->GetValue("Local Appdata")->ToString());
				msclr::interop::marshal_context cxt; 
				char *tmp2 = _llUtils()->NewString(cxt.marshal_as<const char*>(rgValue));
				_llUtils()->SetValue("_appdata", tmp2);
				_llUtils()->SetHidden("_appdata");
			}
			msclr::interop::marshal_context cxt; 
			char *tmp2 =  _llUtils()->NewString(cxt.marshal_as<const char*>(Directory::GetCurrentDirectory()));
			_llUtils()->SetValue("_workdir", tmp2);
			_llUtils()->SetHidden("_workdir");
			//searchdir = gcnew String(Directory::GetCurrentDirectory());
			textBox1->AppendText("Working directory: " + Directory::GetCurrentDirectory() + Environment::NewLine );

			RegistryKey ^rk2 = nullptr;
			installdir = nullptr;
			rk2 = Registry::CurrentUser->OpenSubKey("SOFTWARE\\MPGUI");
			String ^auto4 = nullptr;
			if (rk2 == nullptr) {
				textBox1->AppendText("Registry key of MPGUI not found, autoload batch file can not be loaded from this place");
				this->referenceManualToolStripMenuItem->Enabled = false;
			} else {
				if (rk2->GetValue("Path")) {
					installdir = gcnew String(rk2->GetValue("Path")->ToString());
					auto4 = gcnew String(installdir + "mpgui_autoload.mpb");
				}
			}

			//Version
			_llUtils()->SetValue("_mpgui_version", "1.99");
			_llUtils()->SetHidden("_mpgui_version");

			//looking for the autostart
			String ^auto1 = gcnew String("mpgui_autoload.mpb");
			String ^auto2 = gcnew String("ini\\mpgui_autoload.mpb");
			String ^auto3 = gcnew String("ini\\mpgui\\mpgui_autoload.mpb");

			if (System::IO::File::Exists(auto1)) {
				if (batch->Open("mpgui_autoload.mpb", "[_autoload]")) {
					batch->ReadCache(1);
					batch->CompileScript();
					textBox1->AppendText("Configuration loaded from working directory" + Environment::NewLine );
					update_all_tabs(-1, "[_autoload]");
				}
			} else if (System::IO::File::Exists(auto2)) {
				if (batch->Open("ini\\mpgui_autoload.mpb", "[_autoload]")) {
					batch->ReadCache(1);
					batch->CompileScript();
					textBox1->AppendText("Configuration loaded from \\INI directory" + Environment::NewLine );
					update_all_tabs(-1, "[_autoload]");
				}
			} else if (System::IO::File::Exists(auto3)) {
				if (batch->Open("ini\\mpgui\\mpgui_autoload.mpb", "[_autoload]")) {
					batch->ReadCache(1);
					batch->CompileScript();
					textBox1->AppendText("Configuration loaded from \\INI\\MPGUI directory" + Environment::NewLine );
					update_all_tabs(-1, "[_autoload]");
				}
			} else if (auto4 &&  System::IO::File::Exists(auto4)) {
				msclr::interop::marshal_context cxt; 
				char *tmp2 = new char[strlen(cxt.marshal_as<const char*>(auto4))+1];
				strcpy_s(tmp2,strlen(cxt.marshal_as<const char*>(auto4))+1,cxt.marshal_as<const char*>(auto4));
				if (batch->Open(tmp2, "[_autoload]")) {
					batch->ReadCache(1);
					batch->CompileScript();
					textBox1->AppendText("Configuration loaded from install directory" + Environment::NewLine );
					update_all_tabs(-1, "[_autoload]");
				}
			} else  {
				textBox1->AppendText("Configuration file not found" + Environment::NewLine);
			} 
			mesg->WriteNextLine(LOG_INFO, "This is MPGUI version %s", _llUtils()->GetValue("_mpgui_version"));
			Dump();
			if (args) OpenFile(gcnew String(args));
		}

		void Dump(void) {
			//textBox1->BeginUpdate();
			const char *x = mesg->ReadNextLine();
			while (x) {
				textBox1->AppendText(gcnew String(x) + Environment::NewLine);
				x = mesg->ReadNextLine();
			}
			//textBox1->EndUpdate();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}

		Stream ^myBatchStream;
		String ^scratch;
		int plugins_done, child_pid, is_in_update;
		llCommands *batch;
		llLogger *mesg;
		String ^s_listname;
		String ^l_listname;
		String ^ws_primary;
		String ^searchdir;
		String ^installdir;


	private: System::Windows::Forms::MenuStrip ^menuStrip1;


	protected: 

	private: System::ComponentModel::BackgroundWorker^ backgroundWorker1;
	private: System::ComponentModel::BackgroundWorker^ backgroundWorker1a;
	private: System::ComponentModel::BackgroundWorker^ backgroundWorker2;
	private: System::ComponentModel::BackgroundWorker^ backgroundWorker3;

	private: System::Windows::Forms::ToolStripMenuItem^  fileToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  openBatchToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  exitToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  helpToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  aboutToolStripMenuItem;
	private: System::Windows::Forms::TabControl^  tab;
	private: System::Windows::Forms::TabPage^  tab_splash;
	private: System::Windows::Forms::TabPage^  tab_plugins;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::Button^  button_std;
	private: System::Windows::Forms::CheckedListBox^  checkedListBox1;
	private: System::Windows::Forms::Button^  button_all;
	private: System::Windows::Forms::Button^  button_none;
	private: System::Windows::Forms::TabPage^  tab_ws;
	private: System::Windows::Forms::ComboBox^  comboBox1;
	private: System::Windows::Forms::Button^  button_readws;
	private: System::Windows::Forms::ProgressBar^  progressBar1;

	private: array<System::Windows::Forms::TabPage^>^tab_app;

	private: System::Windows::Forms::ToolTip^  toolTip1;
	private: System::Windows::Forms::ToolStripMenuItem^  cancelProcessToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  saveBatchToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  referenceManualToolStripMenuItem;
	private: System::Windows::Forms::TextBox^  textBox2;
	private: System::Windows::Forms::ToolStripMenuItem^  userReadmeToolStripMenuItem;
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::ToolStripMenuItem^  optionsToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  gameModeToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  loadOrderToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  loadOrderTimeToolStripMenuItem;
    private: System::Windows::Forms::ToolStripMenuItem^  loadOrderTxtToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  gameDirToolStripMenuItem;
	
	private: FolderBrowserDialog^ folderBrowserDialog1;

	private: System::ComponentModel::IContainer^  components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->openBatchToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->saveBatchToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->cancelProcessToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->optionsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->gameModeToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->loadOrderToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->loadOrderTimeToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->loadOrderTxtToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->gameDirToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->helpToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->aboutToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->userReadmeToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->referenceManualToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->tab = (gcnew System::Windows::Forms::TabControl());
			this->tab_splash = (gcnew System::Windows::Forms::TabPage());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->tab_plugins = (gcnew System::Windows::Forms::TabPage());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->button_none = (gcnew System::Windows::Forms::Button());
			this->button_all = (gcnew System::Windows::Forms::Button());
			this->button_std = (gcnew System::Windows::Forms::Button());
			this->checkedListBox1 = (gcnew System::Windows::Forms::CheckedListBox());
			this->tab_ws = (gcnew System::Windows::Forms::TabPage());
			this->textBox2 = (gcnew System::Windows::Forms::TextBox());
			this->progressBar1 = (gcnew System::Windows::Forms::ProgressBar());
			this->comboBox1 = (gcnew System::Windows::Forms::ComboBox());
			this->button_readws = (gcnew System::Windows::Forms::Button());
			this->toolTip1 = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->menuStrip1->SuspendLayout();
			this->tab->SuspendLayout();
			this->tab_splash->SuspendLayout();
			this->tab_plugins->SuspendLayout();
			this->tab_ws->SuspendLayout();
			this->SuspendLayout();
			// 
			// menuStrip1
			// 
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->fileToolStripMenuItem, 
				this->optionsToolStripMenuItem, this->helpToolStripMenuItem});
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(1031, 24);
			this->menuStrip1->TabIndex = 0;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// fileToolStripMenuItem
			// 
			this->fileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->openBatchToolStripMenuItem, 
				this->saveBatchToolStripMenuItem, this->cancelProcessToolStripMenuItem, this->exitToolStripMenuItem});
			this->fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
			this->fileToolStripMenuItem->Size = System::Drawing::Size(37, 20);
			this->fileToolStripMenuItem->Text = L"&File";
			// 
			// openBatchToolStripMenuItem
			// 
			this->openBatchToolStripMenuItem->Name = L"openBatchToolStripMenuItem";
			this->openBatchToolStripMenuItem->Size = System::Drawing::Size(153, 22);
			this->openBatchToolStripMenuItem->Text = L"&Open batch...";
			this->openBatchToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::openBatchToolStripMenuItem_Click);
			// 
			// saveBatchToolStripMenuItem
			// 
			this->saveBatchToolStripMenuItem->Name = L"saveBatchToolStripMenuItem";
			this->saveBatchToolStripMenuItem->Size = System::Drawing::Size(153, 22);
			this->saveBatchToolStripMenuItem->Text = L"Save batch...";
			this->saveBatchToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::saveBatchToolStripMenuItem_Click);
			// 
			// cancelProcessToolStripMenuItem
			// 
			this->cancelProcessToolStripMenuItem->Name = L"cancelProcessToolStripMenuItem";
			this->cancelProcessToolStripMenuItem->Size = System::Drawing::Size(153, 22);
			this->cancelProcessToolStripMenuItem->Text = L"Cancel process";
			this->cancelProcessToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::cancelProcessToolStripMenuItem_Click);
			// 
			// exitToolStripMenuItem
			// 
			this->exitToolStripMenuItem->Name = L"exitToolStripMenuItem";
			this->exitToolStripMenuItem->Size = System::Drawing::Size(153, 22);
			this->exitToolStripMenuItem->Text = L"&Exit";
			this->exitToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::exitToolStripMenuItem_Click);
			// 
			// optionsToolStripMenuItem
			// 
			this->optionsToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->gameModeToolStripMenuItem,
				this->loadOrderToolStripMenuItem, this->gameDirToolStripMenuItem});
			this->optionsToolStripMenuItem->Name = L"optionsToolStripMenuItem";
			this->optionsToolStripMenuItem->Size = System::Drawing::Size(61, 20);
			this->optionsToolStripMenuItem->Text = L"&Options";
			// 
			// gameModeToolStripMenuItem
			// 
			this->gameModeToolStripMenuItem->Name = L"gameModeToolStripMenuItem";
			this->gameModeToolStripMenuItem->Size = System::Drawing::Size(152, 22);
			this->gameModeToolStripMenuItem->Text = L"&Game mode";
			// 
			// loadOrderToolStripMenuItem
			// 
			this->loadOrderToolStripMenuItem->Name = L"loadOrderToolStripMenuItem";
			this->loadOrderToolStripMenuItem->Size = System::Drawing::Size(152, 22);
			this->loadOrderToolStripMenuItem->Text = L"&Load order";
			//
			// Select custom gamedir
			//
			this->gameDirToolStripMenuItem->Name = L"gameDirToolStripMenuItem";
			this->gameDirToolStripMenuItem->Size = System::Drawing::Size(152, 22);
			this->gameDirToolStripMenuItem->Text = L"&Custom game directory";
			this->gameDirToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::gameDirToolStripMenuItem_Click);

			//submenue
			this->loadOrderToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->loadOrderTimeToolStripMenuItem,
				this->loadOrderTxtToolStripMenuItem});
			this->loadOrderTimeToolStripMenuItem->Name = L"loadOrderTimeToolStripMenuItem";
			this->loadOrderTimeToolStripMenuItem->Size = System::Drawing::Size(152, 22);
			this->loadOrderTimeToolStripMenuItem->Text = L"&By time stamp";
			this->loadOrderTimeToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::loadOrderTime_Click);
			this->loadOrderTxtToolStripMenuItem->Name = L"loadOrderTxtToolStripMenuItem";
			this->loadOrderTxtToolStripMenuItem->Size = System::Drawing::Size(152, 22);
			this->loadOrderTxtToolStripMenuItem->Text = L"&By plugins/loadorder file";
			this->loadOrderTxtToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::loadOrderFile_Click);


			// 
			// helpToolStripMenuItem
			// 
			this->helpToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->aboutToolStripMenuItem, 
				this->userReadmeToolStripMenuItem, this->referenceManualToolStripMenuItem});
			this->helpToolStripMenuItem->Name = L"helpToolStripMenuItem";
			this->helpToolStripMenuItem->Size = System::Drawing::Size(44, 20);
			this->helpToolStripMenuItem->Text = L"&Help";
			// 
			// aboutToolStripMenuItem
			// 
			this->aboutToolStripMenuItem->Name = L"aboutToolStripMenuItem";
			this->aboutToolStripMenuItem->Size = System::Drawing::Size(169, 22);
			this->aboutToolStripMenuItem->Text = L"&About...";
			this->aboutToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::aboutToolStripMenuItem_Click);
			// 
			// userReadmeToolStripMenuItem
			// 
			this->userReadmeToolStripMenuItem->Name = L"userReadmeToolStripMenuItem";
			this->userReadmeToolStripMenuItem->Size = System::Drawing::Size(169, 22);
			this->userReadmeToolStripMenuItem->Text = L"User readme";
			this->userReadmeToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::userReadmeToolStripMenuItem_Click);
			// 
			// referenceManualToolStripMenuItem
			// 
			this->referenceManualToolStripMenuItem->Name = L"referenceManualToolStripMenuItem";
			this->referenceManualToolStripMenuItem->Size = System::Drawing::Size(169, 22);
			this->referenceManualToolStripMenuItem->Text = L"Reference manual";
			this->referenceManualToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::referenceManualToolStripMenuItem_Click);
			// 
			// tab
			// 
			this->tab->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->tab->Controls->Add(this->tab_splash);
			this->tab->Controls->Add(this->tab_plugins);
			this->tab->Controls->Add(this->tab_ws);
			this->tab->Location = System::Drawing::Point(12, 27);
			this->tab->Name = L"tab";
			this->tab->SelectedIndex = 0;
			this->tab->ShowToolTips = true;
			this->tab->Size = System::Drawing::Size(1007, 519);
			this->tab->TabIndex = 1;
			// 
			// tab_splash
			// 
			this->tab_splash->Controls->Add(this->textBox1);
			this->tab_splash->Location = System::Drawing::Point(4, 22);
			this->tab_splash->Name = L"tab_splash";
			this->tab_splash->Padding = System::Windows::Forms::Padding(3);
			this->tab_splash->Size = System::Drawing::Size(999, 493);
			this->tab_splash->TabIndex = 0;
			this->tab_splash->Text = L"Console";
			this->tab_splash->ToolTipText = L"Console for messages";
			this->tab_splash->UseVisualStyleBackColor = true;
			// 
			// textBox1
			// 
			this->textBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox1->BackColor = System::Drawing::SystemColors::Window;
			this->textBox1->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->textBox1->Location = System::Drawing::Point(6, 6);
			this->textBox1->Multiline = true;
			this->textBox1->Name = L"textBox1";
			this->textBox1->ReadOnly = true;
			this->textBox1->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->textBox1->Size = System::Drawing::Size(993, 487);
			this->textBox1->TabIndex = 0;
			// 
			// tab_plugins
			// 
			this->tab_plugins->Controls->Add(this->button1);
			this->tab_plugins->Controls->Add(this->button_none);
			this->tab_plugins->Controls->Add(this->button_all);
			this->tab_plugins->Controls->Add(this->button_std);
			this->tab_plugins->Controls->Add(this->checkedListBox1);
			this->tab_plugins->Location = System::Drawing::Point(4, 22);
			this->tab_plugins->Name = L"tab_plugins";
			this->tab_plugins->Padding = System::Windows::Forms::Padding(3);
			this->tab_plugins->Size = System::Drawing::Size(999, 493);
			this->tab_plugins->TabIndex = 1;
			this->tab_plugins->Text = L"Plugins";
			this->tab_plugins->ToolTipText = L"Select the plugins here";
			this->tab_plugins->UseVisualStyleBackColor = true;
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(250, 6);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(75, 23);
			this->button1->TabIndex = 4;
			this->button1->Text = L"Rescan";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &Form1::button1_Click);
			// 
			// button_none
			// 
			this->button_none->Location = System::Drawing::Point(168, 6);
			this->button_none->Name = L"button_none";
			this->button_none->Size = System::Drawing::Size(75, 23);
			this->button_none->TabIndex = 3;
			this->button_none->Text = L"None";
			this->button_none->UseVisualStyleBackColor = true;
			this->button_none->Click += gcnew System::EventHandler(this, &Form1::button_none_Click);
			// 
			// button_all
			// 
			this->button_all->Location = System::Drawing::Point(87, 6);
			this->button_all->Name = L"button_all";
			this->button_all->Size = System::Drawing::Size(75, 23);
			this->button_all->TabIndex = 2;
			this->button_all->Text = L"All";
			this->button_all->UseVisualStyleBackColor = true;
			this->button_all->Click += gcnew System::EventHandler(this, &Form1::button_all_Click);
			// 
			// button_std
			// 
			this->button_std->Location = System::Drawing::Point(6, 6);
			this->button_std->Name = L"button_std";
			this->button_std->Size = System::Drawing::Size(75, 23);
			this->button_std->TabIndex = 1;
			this->button_std->Text = L"Plugins.txt";
			this->button_std->UseVisualStyleBackColor = true;
			this->button_std->Click += gcnew System::EventHandler(this, &Form1::button_std_Click);
			// 
			// checkedListBox1
			// 
			this->checkedListBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->checkedListBox1->FormattingEnabled = true;
			this->checkedListBox1->Location = System::Drawing::Point(6, 33);
			this->checkedListBox1->Name = L"checkedListBox1";
			this->checkedListBox1->Size = System::Drawing::Size(993, 454);
			this->checkedListBox1->TabIndex = 0;
			this->checkedListBox1->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::checkedListBox1_SelectedIndexChanged);
			// 
			// tab_ws
			// 
			this->tab_ws->Controls->Add(this->textBox2);
			this->tab_ws->Controls->Add(this->progressBar1);
			this->tab_ws->Controls->Add(this->comboBox1);
			this->tab_ws->Controls->Add(this->button_readws);
			this->tab_ws->Location = System::Drawing::Point(4, 22);
			this->tab_ws->Name = L"tab_ws";
			this->tab_ws->Size = System::Drawing::Size(999, 493);
			this->tab_ws->TabIndex = 2;
			this->tab_ws->Text = L"Worldspace";
			this->tab_ws->ToolTipText = L"Worldspace selection";
			this->tab_ws->UseVisualStyleBackColor = true;
			// 
			// textBox2
			// 
			this->textBox2->BackColor = System::Drawing::SystemColors::ControlLightLight;
			this->textBox2->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->textBox2->Location = System::Drawing::Point(4, 62);
			this->textBox2->Multiline = true;
			this->textBox2->Name = L"textBox2";
			this->textBox2->ReadOnly = true;
			this->textBox2->Size = System::Drawing::Size(991, 428);
			this->textBox2->TabIndex = 5;
			// 
			// progressBar1
			// 
			this->progressBar1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->progressBar1->Location = System::Drawing::Point(4, 32);
			this->progressBar1->Name = L"progressBar1";
			this->progressBar1->Size = System::Drawing::Size(992, 23);
			this->progressBar1->Step = 2;
			this->progressBar1->TabIndex = 4;
			// 
			// comboBox1
			// 
			this->comboBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->comboBox1->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBox1->FormattingEnabled = true;
			this->comboBox1->Location = System::Drawing::Point(116, 5);
			this->comboBox1->Name = L"comboBox1";
			this->comboBox1->Size = System::Drawing::Size(879, 21);
			this->comboBox1->TabIndex = 3;
			this->comboBox1->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::comboBox1_SelectedIndexChanged);
			// 
			// button_readws
			// 
			this->button_readws->Location = System::Drawing::Point(4, 3);
			this->button_readws->Name = L"button_readws";
			this->button_readws->Size = System::Drawing::Size(106, 23);
			this->button_readws->TabIndex = 2;
			this->button_readws->Text = L"Read Worldspaces";
			this->button_readws->UseVisualStyleBackColor = true;
			this->button_readws->Click += gcnew System::EventHandler(this, &Form1::button_readws_Click);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::ControlLightLight;
			this->ClientSize = System::Drawing::Size(1031, 558);
			this->Controls->Add(this->tab);
			this->Controls->Add(this->menuStrip1);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->MainMenuStrip = this->menuStrip1;
			this->Name = L"Form1";
			this->Text = L"MPGUI";
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			this->tab->ResumeLayout(false);
			this->tab_splash->ResumeLayout(false);
			this->tab_splash->PerformLayout();
			this->tab_plugins->ResumeLayout(false);
			this->tab_ws->ResumeLayout(false);
			this->tab_ws->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion


		//********************************************** Help Menu
	private: System::Void aboutToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {

				 myinfo^ info = gcnew myinfo(); 
				 info->SetText(L"This is Gruftikus' Multi Purpose GUI (" + gcnew String(_llUtils()->GetValue("_mpgui_version")) + ")."  + Environment::NewLine
					 + "MPGUI is a graphical interface for various console-based applications." + Environment::NewLine
					 + "Redistribution of MPGUI as a free modder ressource is allowed."  + Environment::NewLine
					 + "Any commercial use, however, is prohibited."  + Environment::NewLine 
					 + "So if you had to pay somebody in order to use MPGUI, something went wrong..."  + Environment::NewLine 
					 + Environment::NewLine 
					 + "The esm/esp parser source code (based on TES4qLOD) has been provided by Lightwave"  + Environment::NewLine);
				 info->ShowDialog();
				 delete info;
			 }

			 //********************************************** Exit menu
	private: System::Void exitToolStripMenuItem_Click(System::Object ^sender, System::EventArgs ^e) {
				 _llLogger()->CloseLogfile();
				 Application::Exit();
			 }

			 //********************************************* open batch
	private: System::Void openBatchToolStripMenuItem_Click(System::Object ^sender, System::EventArgs ^e) {

				 OpenFileDialog ^openFileDialog1 = gcnew OpenFileDialog;

				 openFileDialog1->InitialDirectory = searchdir;
				 openFileDialog1->Filter = "Batch Files|*.mpb";
				 openFileDialog1->Title = "Select a Batch File";
				 openFileDialog1->RestoreDirectory = true;

				 // Show the Dialog.
				 if (openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
					 // Insert code to read the stream here.
					 OpenFile(openFileDialog1->FileName);
				 }
			 }

	public: System::Void OpenFile(String ^file) {

				using namespace System;
				using namespace Microsoft::Win32;

				if (!file) {
					mesg->WriteNextLine(-LOG_FATAL, "File object empty");
					Dump();
					return;
				}
				//MessageBox::Show(gcnew String(file),gcnew String("bla"));
				msclr::interop::marshal_context cxt; 
				batch->Close();
				const char *bla = cxt.marshal_as<char const*>(gcnew String(file));
				batch->Open(bla, "[_gamemode]"); 
				//batch->Open( (char*)(void*)Marshal::StringToHGlobalAnsi(file), "[_gamemode]");
				mesg->WriteNextLine(LOG_INFO, "Open '%s'", cxt.marshal_as<char const*>(file)); //geht alleine
				batch->ReadCache(1);
				batch->OpenSection("[_gamemode]");
				batch->CompileScript(0);
				update_all_tabs(-1, "[_gamemode]");
				if (_llUtils()->GetValue("_gamemode")) {
					mesg->WriteNextLine(LOG_INFO, "Gamemode '%s'", _llUtils()->GetValue("_gamemode")); 
					for (int i=0; i<MAX_GAMES; i++) {
						if (game[i] && _stricmp(game[i], _llUtils()->GetValue("_gamemode"))==0)
							gamemode = i;
					}
				}

				Dump();
				if (!gamemode) {
					mesg->WriteNextLine(LOG_WARNING, "Game mode not set, try to use the search pattern....");
					for (int i=0; i<MAX_GAMES; i++) {
						if (pattern[i]) {
							String ^s1 = gcnew String(pattern[i]);
							int sl = searchdir->Length;
							int pl = strlen(pattern[i]);
							for (int j=0; j<=(sl-pl); j++) {
								if (String::Compare(searchdir, j, s1 , 0, strlen(pattern[i]), true)==0) {
									mesg->WriteNextLine(LOG_INFO, "...found %s", game[i]);
									gamemode = i;
									if (std_ws[gamemode])
										ws_primary = gcnew String(std_ws[gamemode]);
									_llUtils()->SetValue("_gamemode", game[gamemode]);
									//_llUtils()->SetHidden("_gamemode");
								}
							}
						}
					}
					if (!gamemode) {
						mesg->WriteNextLine(LOG_WARNING, "...failed");
					}
					Dump();
				} else {
					if (std_ws[gamemode])
						ws_primary = gcnew String(std_ws[gamemode]);
				}

				update_game_path();

				update_all_tabs(-1, "[MPGUI]");
				
				//Fill Plugin list (only ONCE!)
				update_loadorder();

				if (!plugins_done) {
					plugins_done = 1;
					FillPlugins();
				}

				update_all_tabs(-1, "[_saved]");
				update_game_menu();
			}			


    //********************************************** Game dir
	private: System::Void gameDirToolStripMenuItem_Click(System::Object ^sender, System::EventArgs ^e) {
				 this->folderBrowserDialog1->RootFolder = Environment::SpecialFolder::Desktop;
				 if (_llUtils()->GetValue("_gamedir"))
					 this->folderBrowserDialog1->SelectedPath = gcnew String(_llUtils()->GetValue("_gamedir"));
				 else
					 this->folderBrowserDialog1->SelectedPath = Directory::GetCurrentDirectory();
				 System::Windows::Forms::DialogResult result = folderBrowserDialog1->ShowDialog();
				 if (result == System::Windows::Forms::DialogResult::OK) {
					 msclr::interop::marshal_context cxt; 
					 char *tmp2 = _llUtils()->NewString(cxt.marshal_as<const char*>(folderBrowserDialog1->SelectedPath));
					 _llUtils()->SetValue("_gamedir", tmp2);	
					 mesg->WriteNextLine(LOG_INFO, 
						"Using '%s' as the game directory", _llUtils()->GetValue("_gamedir"));
					 update_game_path();
					 FillPlugins();				 
				 }
			 }

	public: System::Void update_game_path(System::Void) {
				using namespace Microsoft::Win32;
				msclr::interop::marshal_context cxt; 
#if 1
				try {
					//set current directory to game path
					if (_llUtils()->GetValue("_gamedir")) {
						Directory::SetCurrentDirectory(gcnew String(_llUtils()->GetValue("_gamedir")));
					} else if (gamemode) {
						RegistryKey ^rk = nullptr;
						rk = Registry::LocalMachine->OpenSubKey(L"SOFTWARE\\Bethesda Softworks\\" + gcnew String(game[gamemode]));
						if (rk == nullptr) {
							rk = Registry::LocalMachine->OpenSubKey(L"SOFTWARE\\Wow6432Node\\Bethesda Softworks\\" + gcnew String(game[gamemode]));
						}
						if (rk == nullptr) {
							mesg->WriteNextLine(LOG_ERROR, "No registry entry for selected game, chdir not possible");
						} else {
							String ^rgValue = gcnew String(rk->GetValue("Installed Path")->ToString());
							if (rgValue) {
								if (gamemode == 1)
									Directory::SetCurrentDirectory(rgValue + "\\Data Files\\");
								else
									Directory::SetCurrentDirectory(rgValue + "\\Data\\");
								char *tmp2 = new char[strlen(cxt.marshal_as<const char*>(Directory::GetCurrentDirectory()))+1];
								strcpy_s(tmp2, strlen(cxt.marshal_as<const char*>(Directory::GetCurrentDirectory()))+1, cxt.marshal_as<const char*>(Directory::GetCurrentDirectory()));
								_llUtils()->SetValue("_gamedir", tmp2);
								_llUtils()->SetHidden("_gamedir");
							} else  {
								mesg->WriteNextLine(LOG_ERROR, 
									"Registry entry 'Installed Path' for selected game not found, chdir not possible");
							} 
						}
					} 
				}
				catch ( DirectoryNotFoundException^ e ) 
				{
					// Let the user know that the directory did not exist.
					mesg->WriteNextLine(LOG_ERROR, 
						"Path '%s' for selected game not found, chdir not possible", _llUtils()->GetValue("_gamedir"));
				}
#endif
				char *tmp2 = _llUtils()->NewString(cxt.marshal_as<const char*>(Directory::GetCurrentDirectory()));
				_llUtils()->SetValue("_gamedir", tmp2);			

			}

	public: System::Void update_game_menu(System::Void) {
				for (int i=0; i<MAX_GAMES; i++) {
					if (game[i]) {
						array<ToolStripItem^> ^tmp = this->gameModeToolStripMenuItem->DropDownItems->Find(gcnew String(game[i]), false);
						if (tmp->Length == 1) {
							//tmp[0]->CanSelect = true;
							((System::Windows::Forms::ToolStripMenuItem^)tmp[0])->Checked = false;
						}
						if (tmp->Length == 0) {
							System::Windows::Forms::ToolStripMenuItem ^ dummy = gcnew System::Windows::Forms::ToolStripMenuItem;
							dummy->Name = gcnew String(game[i]);
							dummy->Size = System::Drawing::Size(152, 22);
							dummy->Text = gcnew String(game[i]);
							dummy->Click += gcnew System::EventHandler(this, &Form1::gameModeMenu_Click);
							this->gameModeToolStripMenuItem->DropDownItems->Add(dummy);
							this->gameModeToolStripMenuItem->Enabled = true;
							this->loadOrderToolStripMenuItem->Enabled = true;
						}
					}
					if (gamemode) {
						array<ToolStripItem^> ^tmp = 
							this->gameModeToolStripMenuItem->DropDownItems->Find(gcnew String(game[gamemode]), false);
						if (tmp->Length == 1) {
							//tmp[0]->CanSelect = true;
							((System::Windows::Forms::ToolStripMenuItem ^)tmp[0])->Checked = true;
						}
					}
				}
			}

	private: System::Void gameModeMenu_Click(System::Object ^sender, System::EventArgs ^e) {
				 //read game mode name
				 System::Windows::Forms::ToolStripMenuItem ^dummy = ((System::Windows::Forms::ToolStripMenuItem^) sender);
				 msclr::interop::marshal_context cxt; 
				 const char *name = cxt.marshal_as<char const*>(dummy->Name);
				 for (int i=0; i<MAX_GAMES; i++) {
					 if (game[i] && (strcmp(game[i],name)==0)) {
						 gamemode = i;
						 if (std_ws[gamemode])
							 ws_primary = gcnew String(std_ws[gamemode]);
						 _llUtils()->SetValue("_gamemode", game[gamemode]);
						 _llUtils()->SetHidden("_gamemode");
					 }
				 }
				 update_loadorder();
				 update_game_menu();
				 update_game_path();
				 FillPlugins();
			 }

    private: System::Void loadOrderTime_Click(System::Object ^sender, System::EventArgs ^e) {
				 use_loadorder[gamemode] = 0;
				 update_loadorder();
				 FillPlugins();
			 }

    private: System::Void loadOrderFile_Click(System::Object ^sender, System::EventArgs ^e) {
				 use_loadorder[gamemode] = 1;
				 update_loadorder();
				 FillPlugins();
			 }

    public: System::Void update_loadorder(System::Void) {
				 this->loadOrderTxtToolStripMenuItem->Checked = false;
				 this->loadOrderTimeToolStripMenuItem->Checked = false;
				 if (use_loadorder[gamemode])
					 this->loadOrderTxtToolStripMenuItem->Checked = true;
				 else
					 this->loadOrderTimeToolStripMenuItem->Checked = true;
			}

	public: System::Void FillPlugins(System::Void) {
				checkedListBox1->Items->Clear();
				button_std->Enabled = false;

				if (!plugin[gamemode]) {
					mesg->WriteNextLine(LOG_WARNING, "Plugins.txt not set, Plugins button disabled");
					button_std->Enabled = false;
				} else {
					mesg->WriteNextLine(LOG_INFO, "Plugins.txt path is: '%s'", plugin[gamemode]);
					s_listname = gcnew String(plugin[gamemode]);
					button_std->Enabled = true;
					if (!System::IO::File::Exists(s_listname)) {
						mesg->WriteNextLine(LOG_WARNING, "Plugins.txt file not existing, Plugins button disabled");
						button_std->Enabled = false;
					}
					Dump();
				}

				if (use_loadorder[gamemode]) {
					mesg->WriteNextLine(LOG_INFO, "Using Loadorder.txt");
					if (!loadorder[gamemode]) {
						mesg->WriteNextLine(LOG_WARNING, "Loadorder.txt not set");
						use_loadorder[gamemode] = 0;
					} else {
						l_listname = gcnew String(loadorder[gamemode]);
						if (!System::IO::File::Exists(l_listname)) {
							if (s_listname) {
								mesg->WriteNextLine(LOG_WARNING, "Loadorder.txt file not existing, using Plugins.txt as a fallback");
								l_listname = s_listname;
							} else {
								mesg->WriteNextLine(LOG_WARNING, "Plugins.txt file not existing, getting loadorder not possible");
								use_loadorder[gamemode] = 0;
							}
						}
						Dump();
					}
				}

				msclr::interop::marshal_context cxt; 
				mesg->WriteNextLine(LOG_INFO, "Reading esp/esm files from '%s'",
					cxt.marshal_as<char const*>(Directory::GetCurrentDirectory()));
				Dump();

				//Read plugins from directory
				array<String^>^dirs1 = Directory::GetFiles(Directory::GetCurrentDirectory(), "*.esp" );
				array<String^>^dirs2 = Directory::GetFiles(Directory::GetCurrentDirectory(), "*.esm" );

				int len = dirs1->Length;
				Array::Resize(dirs1, len + dirs2->Length);
				for (int i=0; i<dirs2->Length; i++) dirs1[len+i] = dirs2[i];

				//FILETIME *time_list_sorted=new FILETIME[dirs1->Length];
				array<FileInfo^> ^file1;
				Array::Resize(file1, dirs1->Length);
				for (int i=0; i<dirs1->Length; i++) {
					file1[i] = gcnew FileInfo(dirs1[i]);
					if (!file1[i]) {
						mesg->WriteNextLine(LOG_FATAL, "The esp '%s' was not found",  cxt.marshal_as<char const*>(dirs1[i]));
						Dump();
					}
				}

				//Sort to time
				for (int i=0; i<dirs1->Length; i++) {  //quicksort
					for (int j=i; j>0; j--) {  //quicksort
						if (file1[j-1]->LastWriteTime.CompareTo(file1[j]->LastWriteTime) > 0) {
							FileInfo ^ttmp = file1[j-1];
							String   ^tmp  = dirs1[j-1];
							file1[j-1] = file1[j];
							dirs1[j-1] = dirs1[j];
							file1[j]   = ttmp;
							dirs1[j]   = tmp;
						}
					}
				}

				if (use_loadorder[gamemode]) {
					array<Int32> ^index = gcnew array<Int32>(dirs1->Length);
					for (int i=0; i<dirs1->Length; i++) {
						index[i] = 9999;
					}
					//fill index numbers with position from loadorder
					StreamReader ^din = File::OpenText(l_listname);
					String ^str;
					int count = 0;
					while ((str=din->ReadLine()) != nullptr)  {
						//mesg->WriteNextLine(LOG_INFO, "Open '%s'", cxt.marshal_as<char const*>(str));
						for (int i=0; i<dirs1->Length; i++) {
							if (str->Equals(System::IO::Path::GetFileName(dirs1[i]), System::StringComparison::CurrentCultureIgnoreCase)) {
								index[i] = count;
								//mesg->WriteNextLine(LOG_INFO, "Setting '%s' to %i", cxt.marshal_as<char const*>(dirs1[i]), count);
							}
						}
						count++;
					}

					//Sort to index number
					for (int i=0; i<dirs1->Length; i++) {  //quicksort
						for (int j=i; j>0; j--) {  //quicksort
							if (index[j-1] > index[j]) {
								FileInfo ^ttmp = file1[j-1];
								String   ^tmp  = dirs1[j-1];
								Int32     itmp = index[j-1];
								file1[j-1] = file1[j];
								dirs1[j-1] = dirs1[j];
								index[j-1] = index[j];
								file1[j]   = ttmp;
								dirs1[j]   = tmp;
								index[j]   = itmp;
							}
						}
					}
					Dump();
				}

				for (int i=0; i<dirs1->Length; i++) {
					String ^tmp = System::IO::Path::GetFileName(dirs1[i]);
					String ^s2 = ",";
					//if (!tmp->Contains(s2))
					checkedListBox1->Items->Add(System::IO::Path::GetFileName(dirs1[i]));
					//else {
					//String ^s1 = " <do not use>"; //BUGBUG: must be solved with \,
					//tmp += s1;
					//checkedListBox1->Items->Add(tmp);
					//}
				}

				this->tab_plugins->Enabled = true;
			}

	private: void get_object(int *tab, int *obj, char *name) {
				 *tab = *obj = -1;
				 if (!name) 
					 return;
				 String ^n = gcnew String(name);
				 for (int ii=0; ii<tab_app->Length; ii++) {
					 if (tab_app[ii]->Name == n) 
						 *tab = ii;
					 for (int ij=0; ij<tab_app[ii]->Controls->Count; ij++) {
						 if (tab_app[ii]->Controls[ij]->Name == n) {
							 *obj = ij;
							 *tab = ii;
						 }
					 }
				 }
			 }


	private: void update_all_tabs(int start, const char *sec) {
				 if (is_in_update) 
					 return;
				 is_in_update = 1;
				 this->SuspendLayout();
				 msclr::interop::marshal_context cxt; 
				 //mesg->WriteNextLine(LOG_INFO, "**** sec %s", sec);
				 Dump();

				 //compile section, if not yet done
				 if (sec) {
					 batch->OpenSection(sec);
					 batch->CompileScript(0);
				 }

				 for (int j=start; j<tab_app->Length; j++) {
					 //mesg->WriteNextLine(LOG_INFO, "j %i", j);
					 char tmp[1000];
					 if (j < 0) {
						 if (!sec)
							 sprintf(tmp, "%s", "[mpgui]");
						 else
							 sprintf(tmp, "%s", sec);
					 } else 
						 sprintf(tmp, "[%s]", cxt.marshal_as<char const*>(tab_app[j]->Name));
					 //mesg->WriteNextLine(LOG_INFO, "update tab %s", tmp);
					 batch->OpenSection(tmp);
					 batch->CompileScript(0);
					 Dump();

					 int com;

					 while ((com = batch->GetCommand())>-2) {
						 //mesg->WriteNextLine(LOG_INFO, "got %i", com);
						 Dump();
						 //before I continue I have to make sure that object is not on list already
						 int tab=-1, obj=-1;

						 if (com == COM_SETPATH) {
							 searchdir = gcnew String(flagtext);
						 }

						 if (com == COM_GUISPLASHECHO) {
							 mesg->WriteNextLine(LOG_ECHO, guitext);
							 if (myswitch) this->tab->SelectedIndex=0;
							 Dump();
						 }
						

						 if ((com == COM_GUITEXTBOX || com == COM_GUICHECKBOX || com == COM_GUIENABLE || com == COM_GUIDISABLE ||
							 com == COM_GUIDROPDOWN || com == COM_GUIDROPDOWNITEM || com == COM_GUIBUTTON) && j>=0) {
								 get_object(&tab, &obj, guiname);
								 //mesg->WriteNextLine(LOG_INFO, "got %i %i for %s", tab, obj, guiname);
								 //Dump();
						 }

						 if (com == COM_GUIAPPLICATION) {
							 int found = 0;
							 String ^name  = gcnew String(guiname);
							 for (int jj=0; jj<tab_app->Length; jj++) {
								 if (tab_app[jj]->Name == name) found=1;
							 }

							 if (!found) {
								 mesg->WriteNextLine(LOG_INFO, "Create tab for '%s'", guitext);
								 Dump();

								 /* Add new TAB ******************************/
								 Array::Resize(tab_app, tab_app->Length+1);
								 tab_app[tab_app->Length-1] = gcnew TabPage;
								 tab_app[tab_app->Length-1]->SuspendLayout();
								 tab_app[tab_app->Length-1]->Name = name;
								 tab_app[tab_app->Length-1]->Size = System::Drawing::Size(999, 322);
								 tab_app[tab_app->Length-1]->TabIndex = tab_app->Length+3;
								 tab_app[tab_app->Length-1]->Text = gcnew String(guitext);
								 tab_app[tab_app->Length-1]->ToolTipText = gcnew String(guihelp);
								 tab_app[tab_app->Length-1]->UseVisualStyleBackColor = true;
								 this->tab->Controls->Add(tab_app[tab_app->Length-1]);
								 tab_app[tab_app->Length-1]->Click += gcnew System::EventHandler(this, &Form1::tab_app_Click);
								 tab_app[tab_app->Length-1]->ResumeLayout(false);
							 } else {
								 mesg->WriteNextLine(LOG_WARNING, "Tab '%s' already created", guiname);
								 Dump();
							 }
						 }

						 if (com == COM_GUITEXTBOX && tab<0 && j>=0) {
							 int pos = 4 + vdist;
							 if (tab_app[j]->Controls->Count) {
								 pos = tab_app[j]->Controls[tab_app[j]->Controls->Count-1]->Location.Y + 
									 tab_app[j]->Controls[tab_app[j]->Controls->Count-1]->Height + vdist;
								 if (sameline) 
									 pos = tab_app[j]->Controls[tab_app[j]->Controls->Count-1]->Location.Y + vdist;
							 } //sameline
							 TextBox ^n = (gcnew System::Windows::Forms::TextBox());
							 n->Anchor      = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
								 | System::Windows::Forms::AnchorStyles::Right));
							 n->BorderStyle = System::Windows::Forms::BorderStyle::None;
							 n->Location    = System::Drawing::Point(4, pos);
							 if (sameline && tab_app[j]->Controls->Count) {
								 n->Location = System::Drawing::Point(8+tab_app[j]->Controls[tab_app[j]->Controls->Count-1]->Width+
									 tab_app[j]->Controls[tab_app[j]->Controls->Count-1]->Location.X, pos); //sameline
							 }
							 n->Name = gcnew String(guiname);
							 toolTip1->SetToolTip(n, gcnew String(guihelp));
							 _llUtils()->AddFlag(guiname);		
							 if (textinput || fileinput) {
								 if (textinput) {
									 n->ReadOnly = false;
								 }
								 if (fileinput) {
									 n->Click += gcnew System::EventHandler(this, &Form1::my_TextBoxClicked);
									 //n->BackColor = Color::AntiqueWhite;
								 }
								 n->Leave += gcnew System::EventHandler(this, &Form1::my_TextChanged);
								 n->TextChanged += gcnew System::EventHandler(this, &Form1::my_TextChanged);
								 n->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
								 _llUtils()->SetValue(guiname, guitext);	
							 } else {
								 n->ReadOnly = true;
								 //n->Top=5;
							 }
							 n->Multiline = true;
							 n->Size = System::Drawing::Size(tab_app[j]->Size.Width*guiwidth-8, guiheight); //sameline
							 if (tab_app[j]->Size.Width - (n->Size.Width + n->Location.X) > 10) n->Anchor = 
								 static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | 
								 System::Windows::Forms::AnchorStyles::Left)));  //sameline
							 n->TabIndex = 5;
							 n->Text = gcnew String(guitext);
							 tab_app[j]->Controls->Add(n);
						 }

						 if (com == COM_GUICHECKBOX && tab<0 && j>=0) {
							 int pos = 4 + vdist;
							 if (tab_app[j]->Controls->Count) {
								 pos = tab_app[j]->Controls[tab_app[j]->Controls->Count-1]->Location.Y + 
									 tab_app[j]->Controls[tab_app[j]->Controls->Count-1]->Height + vdist;
								 if (sameline) 
									 pos = tab_app[j]->Controls[tab_app[j]->Controls->Count-1]->Location.Y + vdist;
							 } //sameline
							 CheckBox ^n = (gcnew System::Windows::Forms::CheckBox());						
							 n->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
								 | System::Windows::Forms::AnchorStyles::Right));
							 n->Location = System::Drawing::Point(4, pos);
							 if (sameline && tab_app[j]->Controls->Count) {
								 n->Location = System::Drawing::Point(8+tab_app[j]->Controls[tab_app[j]->Controls->Count-1]->Width+
									 tab_app[j]->Controls[tab_app[j]->Controls->Count-1]->Location.X, pos); //sameline
							 }
							 n->Name = gcnew String(guiname);
							 n->Size = System::Drawing::Size(tab_app[j]->Size.Width*guiwidth-8, guiheight); //sameline
							 if (tab_app[j]->Size.Width - (n->Size.Width + n->Location.X) > 10) n->Anchor = 
								 static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | 
								 System::Windows::Forms::AnchorStyles::Left)));  //sameline
							 n->TabIndex = 5;
							 n->Text = gcnew String(guitext);
							 toolTip1->SetToolTip(n, gcnew String(guihelp));
							 _llUtils()->AddFlag(guiname);			 
							 if (guienabled) {
								 n->Checked = true;
								 _llUtils()->EnableFlag(guiname);
							 } else {
								 _llUtils()->DisableFlag(guiname);
							 }
							 n->CheckedChanged += gcnew System::EventHandler(this, &Form1::my_CheckedChanged);
							 tab_app[j]->Controls->Add(n);
						 }

						 if (com == COM_GUIDROPDOWN && tab<0 && j>=0) {
							 int pos = 4 + vdist;
							 if (tab_app[j]->Controls->Count) {
								 pos = tab_app[j]->Controls[tab_app[j]->Controls->Count-1]->Location.Y + 
									 tab_app[j]->Controls[tab_app[j]->Controls->Count-1]->Height + vdist;
								 if (sameline) 
									 pos = tab_app[j]->Controls[tab_app[j]->Controls->Count-1]->Location.Y + vdist;
							 } //sameline
							 ComboBox ^n = (gcnew System::Windows::Forms::ComboBox());
							 n->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
								 | System::Windows::Forms::AnchorStyles::Right));

							 n->Location = System::Drawing::Point(4, pos);
							 if (sameline && tab_app[j]->Controls->Count) {
								 n->Location = System::Drawing::Point(8+tab_app[j]->Controls[tab_app[j]->Controls->Count-1]->Width+
									 tab_app[j]->Controls[tab_app[j]->Controls->Count-1]->Location.X, pos); //sameline
							 }
							 n->Name = gcnew String(guiname);
							 n->Size = System::Drawing::Size(tab_app[j]->Size.Width*guiwidth-8, guiheight); //sameline
							 if (tab_app[j]->Size.Width - (n->Size.Width + n->Location.X) > 10) n->Anchor = 
								 static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | 
								 System::Windows::Forms::AnchorStyles::Left)));  //sameline
							 n->TabIndex = 5;
							 n->Text = gcnew String(guitext);
							 toolTip1->SetToolTip(n, gcnew String(guihelp));
							 _llUtils()->AddFlag(guiname);			 
							 //batch->DisableFlag(batch->guiname);
							 n->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
							 n->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::my_SelectedIndexChanged);
							 tab_app[j]->Controls->Add(n);
						 }

						 if (com == COM_GUIDROPDOWNITEM && tab<0 && j>=0) {
							 int pos = 4;
							 //first check for parent
							 get_object(&tab, &obj, guiparent);
							 if (tab>=0 && obj>=0) {
								 //check for item itself
								 int index = ((ComboBox^)tab_app[tab]->Controls[obj])->FindString(gcnew String(guitext));
								 if (index < 0) {
									 ((ComboBox^)tab_app[tab]->Controls[obj])->Items->Add(gcnew String(guitext));
									 _llUtils()->AddFlag(guiname);
									 _llUtils()->SetDescription(guiname, guitext);
									 if (guienabled) {
										 int num = (((ComboBox^)tab_app[tab]->Controls[obj])->Items->Count);
										 ((ComboBox^)tab_app[tab]->Controls[obj])->SelectedIndex = num -1;
										 _llUtils()->EnableFlag(guiname);
									 } else {
										 _llUtils()->DisableFlag(guiname);
									 }
								 }
							 } else {
								 mesg->WriteNextLine(LOG_ERROR, "Parent [%s] not found in [GUIDropDownItem]", guiparent);
							 }
						 }

						 if (com == COM_GUIBUTTON && tab<0 && j>=0) {
							 int pos = 4 + vdist;
							 if (tab_app[j]->Controls->Count) {
								 pos = tab_app[j]->Controls[tab_app[j]->Controls->Count-1]->Location.Y + 
									 tab_app[j]->Controls[tab_app[j]->Controls->Count-1]->Height + vdist;
								 if (sameline) 
									 pos = tab_app[j]->Controls[tab_app[j]->Controls->Count-1]->Location.Y + vdist;
							 } //sameline
							 Button ^n = (gcnew System::Windows::Forms::Button());
							 tab_app[j]->Controls->Add(n);
							 n->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
								 | System::Windows::Forms::AnchorStyles::Right));

							 n->Location = System::Drawing::Point(4, pos);
							 if (sameline && tab_app[j]->Controls->Count) {
								 n->Location = System::Drawing::Point(8+tab_app[j]->Controls[tab_app[j]->Controls->Count-1]->Width+
									 tab_app[j]->Controls[tab_app[j]->Controls->Count-1]->Location.X, pos); //sameline
							 }
							 n->Name = gcnew String(guiname);

							 n->Size = System::Drawing::Size(tab_app[j]->Size.Width*guiwidth-8, guiheight); //sameline
							 if (tab_app[j]->Size.Width - (n->Size.Width + n->Location.X) > 10) n->Anchor = 
								 static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | 
								 System::Windows::Forms::AnchorStyles::Left)));  //sameline
							 n->TabIndex = 5;
							 n->Text = gcnew String(guitext);
							 toolTip1->SetToolTip(n, gcnew String(guihelp));
							 _llUtils()->AddFlag(guiname);			 
							 _llUtils()->DisableFlag(guiname);
							 n->Click += gcnew System::EventHandler(this, &Form1::my_ButtonPressed);
							 n->Tag = gcnew String("Button");
						 }

						 if (com == COM_GUIEXEC) {
							 mesg->WriteNextLine(LOG_INFO, "Executing: '%s'", guitext);
							 Dump();
							 SECURITY_ATTRIBUTES saAttr; 
							 saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
							 saAttr.bInheritHandle = TRUE; 
							 saAttr.lpSecurityDescriptor = NULL; 
							 HANDLE g_hChildStd_IN_Rd = NULL;
							 //HANDLE g_hChildStd_IN_Wr = NULL;
							 //HANDLE g_hChildStd_OUT_Rd = NULL;
							 HANDLE g_hChildStd_OUT_Wr = NULL;
							 HANDLE g_hChildStd_ERR_Wr = NULL;
							 // Create a pipe for the child process's STDOUT.  
							 if ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0) ) 
								 mesg->WriteNextLine(LOG_ERROR, "StdoutRd CreatePipe"); 
							 if ( ! CreatePipe(&g_hChildStd_ERR_Rd, &g_hChildStd_ERR_Wr, &saAttr, 0) ) 
								 mesg->WriteNextLine(LOG_ERROR, "StderrRd CreatePipe"); 
							 // Ensure the read handle to the pipe for STDOUT is not inherited.
							 if ( ! SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) )
								 mesg->WriteNextLine(LOG_ERROR, "Stdout SetHandleInformation"); 
							 if ( ! SetHandleInformation(g_hChildStd_ERR_Rd, HANDLE_FLAG_INHERIT, 0) )
								 mesg->WriteNextLine(LOG_ERROR, "Stdout SetHandleInformation"); 
							 // Create a pipe for the child process's STDIN. 
							 if (! CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0)) 
								 mesg->WriteNextLine(LOG_ERROR, "Stdin CreatePipe"); 
							 // Ensure the write handle to the pipe for STDIN is not inherited. 
							 if ( ! SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0) )
								 mesg->WriteNextLine(LOG_ERROR, "Stdin SetHandleInformation"); 
							 Dump();
							 //TCHAR szCmdline[]=TEXT(batch->guitext);
							 PROCESS_INFORMATION piProcInfo; 
							 STARTUPINFO siStartInfo;
							 BOOL bSuccess = FALSE; 
							 // Set up members of the PROCESS_INFORMATION structure. 
							 ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );
							 // Set up members of the STARTUPINFO structure. 
							 // This structure specifies the STDIN and STDOUT handles for redirection.
							 ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
							 siStartInfo.cb = sizeof(STARTUPINFO); 
							 //siStartInfo.hStdError = g_hChildStd_OUT_Wr;
							 siStartInfo.hStdError  = g_hChildStd_ERR_Wr;
							 siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
							 //siStartInfo.hStdOutput = NULL;
							 siStartInfo.hStdInput = g_hChildStd_IN_Rd;
							 siStartInfo.dwXSize = 0;
							 siStartInfo.dwYSize = 0;
							 siStartInfo.dwX = 99999;
							 siStartInfo.dwY = 99999;
							 siStartInfo.dwFlags |= (STARTF_USESTDHANDLES | STARTF_USESIZE | STARTF_USEPOSITION);
							 // Create the child process.   
							 int len = strlen(guitext)+1;
							 wchar_t *wText = new wchar_t[len];
							 memset(wText, 0, len);
							 ::MultiByteToWideChar(CP_ACP, NULL, guitext, -1, wText, len);
							 bSuccess = CreateProcess(NULL, 
								 wText,     // command line 
								 NULL,          // process security attributes 
								 NULL,          // primary thread security attributes 
								 TRUE,          // handles are inherited 
								 0,             // creation flags 
								 NULL,          // use parent's environment 
								 NULL,          // use parent's current directory 
								 &siStartInfo,  // STARTUPINFO pointer 
								 &piProcInfo);  // receives PROCESS_INFORMATION 
							 // If an error occurs, exit the application. 
							 this->tab->SelectedIndex = 0;
							 if ( ! bSuccess ) 
								 mesg->WriteNextLine(LOG_ERROR, "Create Process failed (wrong command, or executable not existing?)"); 
							 else {
								 child_pid = piProcInfo.dwProcessId;
								 // Close handles to the child process and its primary thread.
								 // Some applications might keep these handles to monitor the status
								 // of the child process, for example. 
								 CloseHandle(piProcInfo.hProcess);
								 CloseHandle(piProcInfo.hThread);

								 DWORD dwRead, dwWritten; 
								 CHAR chBuf[100]; 

#if 1
								 if (!CloseHandle(g_hChildStd_OUT_Wr)) 
									 mesg->WriteNextLine(LOG_ERROR, "StdOutWr CloseHandle"); 
								 if (backgroundWorker1->IsBusy) {
									 this->backgroundWorker1->CancelAsync();
									 MessageBox::Show(L"Background reader didn't finished - break and go for coffee....", L"Info");
								 }
								 while (backgroundWorker1->IsBusy);
								 backgroundWorker1->RunWorkerAsync( 0 );
#endif
#if 1
								 if (!CloseHandle(g_hChildStd_ERR_Wr)) 
									 mesg->WriteNextLine(LOG_ERROR, "StdOutWr CloseHandle"); 
								 if (backgroundWorker1a->IsBusy) {
									 this->backgroundWorker1a->CancelAsync();
									 MessageBox::Show(L"Background error reader didn't finished - break and go for coffee....", L"Info");
								 }
								 while (backgroundWorker1a->IsBusy);
								 backgroundWorker1a->RunWorkerAsync( 0 );
#endif

								 if (!CloseHandle(g_hChildStd_IN_Rd)) 
									 mesg->WriteNextLine(LOG_ERROR, "StdInRd CloseHandle"); 

								 if (dumpbatch) {
									 if (backgroundWorker2->IsBusy) {
										 this->backgroundWorker2->CancelAsync();
										 MessageBox::Show(L"Background writer didn't finished - break and go for coffee....", L"Info");
									 }
									 while (backgroundWorker2->IsBusy);
									 backgroundWorker2->RunWorkerAsync( 0 );
								 } else {
									 CloseHandle(g_hChildStd_IN_Wr); 
									 g_hChildStd_IN_Wr = NULL;
								 }
								 cancelProcessToolStripMenuItem->Enabled = true;
								 tab_ws->Enabled = false;
								 tab_plugins->Enabled = false;
								 for (int w=0; w<tab_app->Length; w++) {
									 tab_app[w]->Enabled = false;
								 }							
							 }
						 } //guiexec

						 if (com == COM_GUIENABLE) {
							 if (tab>=0 && obj>=0) {
								 tab_app[tab]->Controls[obj]->Enabled = 1;
								 if (unselect || guienabled) {
									 CheckBox ^tmp = dynamic_cast<CheckBox^>(tab_app[tab]->Controls[obj]);
									 if (tmp) {
										 if (unselect) {
											 tmp->Checked = false;
											 _llUtils()->DisableFlag(guiname);
										 } else {
											 tmp->Checked = true;
											 _llUtils()->EnableFlag(guiname);
										 }
									 } else {
										 mesg->WriteNextLine(LOG_ERROR, "Object [%s] is not a CheckBox, '-unselect' ignored", guiname);
									 }
								 }
							 } else if (tab >= 0) {
								 tab_app[tab]->Enabled = 1;
							 } else if (_stricmp(guiname, "tab_ws") == 0) {
								 tab_ws->Enabled = 1;
							 } else {
								 mesg->WriteNextLine(LOG_ERROR, "Object [%s] not found in [GUIEnable]", guiname);
							 }
						 }

						 if (com == COM_GUIDISABLE) {
							 if (tab>=0 && obj>=0) {
								 tab_app[tab]->Controls[obj]->Enabled = 0;
								 if (unselect || guienabled) {
									 CheckBox ^tmp = dynamic_cast<CheckBox^>(tab_app[tab]->Controls[obj]);
									 if (tmp) {
										 if (unselect) {
											 tmp->Checked = false;
											 _llUtils()->DisableFlag(guiname);
										 } else {
											 tmp->Checked = true;
											 _llUtils()->EnableFlag(guiname);
										 }
									 } else {
										 mesg->WriteNextLine(LOG_ERROR, "Object [%s] is not a CheckBox, '-unselect' ignored", guiname);
									 }
								 }
							 } else if (tab >= 0) {
								 tab_app[tab]->Enabled = 0;							 
							 } else if (_stricmp(guiname, "tab_ws") == 0) {
								 tab_ws->Enabled = 0;
							 } else {
								 mesg->WriteNextLine(LOG_ERROR, "Object [%s] not found in [GUIDisable]", guiname);
							 }
						 }

						 if (com == COM_GUIMESSAGEBOX) {
							 MessageBox::Show(gcnew String(guitext),gcnew String(guiname));
						 }

						 if (com == COM_GUIWINDOWSIZE) {
							 this->Size = System::Drawing::Size(guiwidth, guiheight);
						 }

						 if (com == COM_GUIREQUESTVERSION) {
							 float v = *(_llUtils()->GetValueF("_mpgui_version"));
							 if (value > v) 
								 MessageBox::Show(L"This batch file needs MPGUI version " + value + L" or newer","Version Error");

						 }

					 } //while

#if 0
						 

						 if (com == COM_SETFLAG) {
							 get_object(&tab, &obj, batch->myflagname);
							 //mesg->WriteNextLine(MH_WARNING,"flag %s", batch->myflagname);
							 if (strcmp(batch->myflagname,"_modlist") == 0) {
								 for ( int i = 0; i < checkedListBox1->Items->Count; i++ )
								 {
									 checkedListBox1->SetItemChecked(i,false);
								 }
								 //mesg->WriteNextLine(MH_WARNING,"Mod cruch");
								 batch->CrunchStart(batch->myflagvalue);
								 while (batch->CrunchNext()) {
									 String^ str = gcnew String(batch->CrunchCurrent());
									 //mesg->WriteNextLine(MH_WARNING,"Mod '%s'",batch->CrunchCurrent());
									 int j = checkedListBox1->FindStringExact(str);
									 if (j != ListBox::NoMatches) 
										 checkedListBox1->SetItemChecked(j,true);
									 else
										 mesg->WriteNextLine(MH_WARNING,"Mod '%s' not longer available",batch->CrunchCurrent());
								 }
								 if (checkedListBox1->CheckedItems->Count != 0) this->tab_ws->Enabled = true;
								 else this->tab_ws->Enabled = false;
							 }

							 if (strcmp(batch->myflagname,"_worldspace") == 0) {
								 tab_ws->Enabled = false;
								 this->tab->SelectedIndex=2;
								 ws_primary = gcnew String(batch->myflagvalue);
								 if (backgroundWorker3->IsBusy) {
									 this->backgroundWorker3->CancelAsync();
									 MessageBox::Show(L"Background worldspace reader didn't finished - break and go for coffee....", L"Info");
								 }
								 while (backgroundWorker3->IsBusy);
								 backgroundWorker3->RunWorkerAsync( 0 );
								 //while (!tab_ws->Enabled);
								 //this->tab->SelectedIndex=0;

							 }

							 if ( tab>-1 && obj>-1 && tab_app[tab]->Controls[obj]) {
								 CheckBox ^tmp = dynamic_cast<CheckBox^>(tab_app[tab]->Controls[obj]);
								 if (tmp) {
									 // mesg->WriteNextLine(MH_WARNING,"bipong1");
									 if (batch->unselect)
										 tmp->Checked = false;
									 else
										 tmp->Checked = true;
									 // mesg->WriteNextLine(MH_WARNING,"bipong2");
								 }
								 TextBox ^tmp2 = dynamic_cast<TextBox^>(tab_app[tab]->Controls[obj]);
								 if (tmp2) {
									 if (tmp2->BorderStyle == System::Windows::Forms::BorderStyle::FixedSingle)
										 tmp2->Text = gcnew String(batch->myflagvalue);
								 }
							 } else	 {
								 for (int ii=0;ii<tab_app->Length;ii++) {
									 for (int ij=0;ij<tab_app[ii]->Controls->Count;ij++) {
										 ComboBox ^dropdown = dynamic_cast<ComboBox^>(tab_app[ii]->Controls[ij]);
										 if (dropdown && batch->GetDescription(batch->myflagname)) {
											 //mesg->WriteNextLine(MH_WARNING,"%s",batch->myflagname);
											 int index = dropdown->FindStringExact(gcnew String(batch->GetDescription(batch->myflagname)));
											 if (index>=0 && !batch->unselect) {
												 dropdown->SelectedIndex = index;
											 }
										 }
									 }
								 }
							 }
						 }





						 


						 }
					 }
#endif
					 Dump();
					 //before leaving the tab, disable all buttons variables
					 if (j >= 0) {
						 for (int ij=0; ij<tab_app[j]->Controls->Count; ij++) {
							 msclr::interop::marshal_context cxt; 
							 if (tab_app[j]->Controls[ij]->Tag) {
								 const char *flagname = cxt.marshal_as<char const*>((tab_app[j]->Controls[ij])->Name);
								 _llUtils()->DisableFlag(flagname);
							 }
						 }
					 }
				 }
				 this->ResumeLayout(false);
				 is_in_update = 0;
			 }

	private: System::Void tab_app_Click(System::Object ^sender, System::EventArgs ^e) {
				 update_all_tabs(0, NULL);
			 }

	private: System::Void button_all_Click(System::Object ^sender, System::EventArgs ^e) {
				 for (int i=0; i<checkedListBox1->Items->Count; i++) {
					 checkedListBox1->SetItemChecked(i, true);
				 }
				 set_enables();
			 }

	private: System::Void button_none_Click(System::Object ^sender, System::EventArgs ^e) {
				 for (int i=0; i<checkedListBox1->Items->Count; i++) {
					 checkedListBox1->SetItemChecked(i, false);
				 }
				 set_enables();
			 }

	private: System::Void button_std_Click(System::Object ^sender, System::EventArgs ^e) {

				 for (int i=0; i<checkedListBox1->Items->Count; i++) {
					 checkedListBox1->SetItemChecked(i,false);
				 }

				 StreamReader ^din = File::OpenText(s_listname);

				 String ^str;
				 int count = 0;
				 if (s_listname->EndsWith("ini")) {
					 mesg->WriteNextLine(LOG_INFO, "Read ini file");
					 while ((str = din->ReadLine()) != nullptr) {
						 array<String^> ^spl = str->Split('=');
						 if (spl->Length == 2) {
							 //textBox1->AppendText(spl[1]);
							 int j = checkedListBox1->FindStringExact(spl[1]);
							 if (j != ListBox::NoMatches) checkedListBox1->SetItemChecked(j,true);
						 }
					 }
				 } else {
					 while ((str=din->ReadLine()) != nullptr)  {
						 int j = checkedListBox1->FindStringExact(str);
						 if (j != ListBox::NoMatches) checkedListBox1->SetItemChecked(j,true);
					 }
				 }
				 set_enables();
			 }


	private: System::Void button_readws_Click(System::Object ^sender, System::EventArgs ^e) {
				 while (backgroundWorker3->IsBusy);
				 tab_ws->Enabled = false;
				 //Fill the mod list
				 input_count = checkedListBox1->CheckedItems->Count;
				 msclr::interop::marshal_context cxt; 
				 for (int i=0; i<checkedListBox1->CheckedItems->Count; i++) {
					 input_filename[i] = 
						 _llUtils()->NewString(cxt.marshal_as<char const*>(checkedListBox1->GetItemText(checkedListBox1->CheckedItems[i])));
				 }
				 tes4qlod_small();
				 backgroundWorker3->RunWorkerAsync(0);
			 }

	public: void backgroundWorker3_DoWork(Object ^sender, DoWorkEventArgs ^e) {
				BackgroundWorker ^worker = dynamic_cast<BackgroundWorker^>(sender);
				worker->ReportProgress(0);
				for (int i=0; i<input_count; i++) {
					worker->ReportProgress( ((i+1)*100)/input_count, gcnew String(L"Open ") + gcnew String(input_filename[i]));
					ExportTES4LandT4QLOD(input_filename[i], mesg);
					//worker->ReportProgress(i+1);
				}	
				worker->ReportProgress(100);
			}

	private: void backgroundWorker3_RunWorkerCompleted(Object ^/*sender*/, RunWorkerCompletedEventArgs ^e) {
				 if (e->Cancelled) {
					 comboBox1->Items->Clear();
					 return;
				 }

				 mesg->WriteNextLine(LOG_INFO, "Found %i worldspaces ", wrld_count);
				 Dump();
				 int myidx = -1;

				 //Read the wordspaces
				 comboBox1->Items->Clear();

				 comboBox1->BeginUpdate();
				 for (int i=0; i<wrld_count; i++) {				 
					 scratch = gcnew String(wrld_name[i]);
					 if (strlen(wrld_description[i]) > 2) {
						 scratch += gcnew String(" \"");
						 scratch += gcnew String(wrld_description[i]);
						 scratch += gcnew String("\"");
					 }
					 scratch += gcnew String(" [");
					 scratch += gcnew String(wrld_formid[i]);
					 scratch += gcnew String("]");
					 if (wrld_child[i]) scratch += gcnew String(" <Child>");
					 scratch += gcnew String(" from '");
					 scratch += gcnew String(wrld_mod[i]);
					 scratch += gcnew String("'");
					 comboBox1->Items->Add(scratch);
					 scratch = gcnew String(wrld_name[i]);
					 if (ws_primary && String::Compare(ws_primary,scratch)==0) {
						 myidx=i;
					 }
				 }
				 comboBox1->EndUpdate();

				 if (myidx>=0) {
					 comboBox1->SelectedIndex = myidx;
				 }

				 update_ws_values(comboBox1);
				 textBox2->AppendText(Environment::NewLine);

				 tab_ws->Enabled = true;
			 }			 

	private: void backgroundWorker3_ProgressChanged(Object ^ /*sender*/, ProgressChangedEventArgs ^e ) {
				 //this->progressBar1->Value = ((e->ProgressPercentage)*100)/input_count;
				 //if (e->ProgressPercentage) {
				//	 int i = e->ProgressPercentage-1;
				//	 textBox2->AppendText((String^)gcnew String(L"Open ") + gcnew String(input_filename[i]) + Environment::NewLine);
				//	 ExportTES4LandT4QLOD(input_filename[i], mesg);
				 //}
				 this->progressBar1->Value = e->ProgressPercentage;
				 textBox2->AppendText((String^)e->UserState + Environment::NewLine );
			 }

	private: void set_enables(void) {
				 if (checkedListBox1->CheckedItems->Count != 0) 
					 this->tab_ws->Enabled = true;
				 else 
					 this->tab_ws->Enabled = false;
				 /********************/
				 char modlist[50000];
				 sprintf_s(modlist, 50000, "\0");
				 msclr::interop::marshal_context cxt; 

				 for (int i=0; i<(checkedListBox1->CheckedItems->Count); i++) {
					 //update _modlist
					 const char *flagname = cxt.marshal_as<char const*>(checkedListBox1->GetItemText(checkedListBox1->CheckedItems[ i ]));;
					 int g = strlen(modlist);
					 Dump();
					 if (i>0) //BUGBUG: check komma
						 sprintf_s(modlist, 50000-g, "%s,%s", modlist, flagname); 
					 else 
						 sprintf_s(modlist, 50000, "%s", flagname); 
				 }
				 char *modlist2 = new char[strlen(modlist)+1];
				 strcpy_s(modlist2, strlen(modlist)+1, modlist);
				 _llUtils()->SetValue("_modlist", modlist2);
				 Dump();
				 comboBox1->Items->Clear();
				 update_all_tabs(0, NULL);
			 }

	private: System::Void checkedListBox1_SelectedIndexChanged(System::Object ^sender, System::EventArgs ^e) {
				 set_enables();
			 }

	private: System::Void my_ButtonPressed(System::Object ^sender, System::EventArgs ^e) {
				 msclr::interop::marshal_context cxt; 
				 const char *flagname = cxt.marshal_as<char const*>(((Button^)sender)->Name);
				 _llUtils()->EnableFlag(flagname);
				 update_all_tabs(0, NULL);
			 }

	private: System::Void my_TextChanged(System::Object ^sender, System::EventArgs ^e) {
				 if (is_in_update) return;
				 String ^text = ((TextBox^)sender)->Text;
				 msclr::interop::marshal_context cxt; 
				 char *tmp2 = _llUtils()->NewString(cxt.marshal_as<const char*>(text));
				 //char *tmp2 = new char[strlen(cxt.marshal_as<const char*>(text))+1];
				 //strcpy_s(tmp2, strlen(cxt.marshal_as<const char*>(text))+1, cxt.marshal_as<const char*>(text));
				 const char *flagname = cxt.marshal_as<char const*>(((TextBox^)sender)->Name);
				 _llUtils()->SetValue(flagname, tmp2);
				 update_all_tabs(0, NULL);
			 }

	private: System::Void my_TextBoxClicked(System::Object ^sender, System::EventArgs ^e) {
				 TextBox ^textbox = (TextBox^)sender;

				 OpenFileDialog ^openFileDialog1 = gcnew OpenFileDialog;
				 openFileDialog1->InitialDirectory = searchdir;
				 openFileDialog1->Filter = "All Files|*.*";
				 String ^help = toolTip1->GetToolTip(textbox);
				 openFileDialog1->Title = "Select file";
				 if (help) openFileDialog1->Title = help;
				 openFileDialog1->RestoreDirectory = true;

				 // Show the Dialog.
				 if (openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
					 textbox->Text = gcnew String(openFileDialog1->FileName);
				 }
			 }

	private: System::Void my_CheckedChanged(System::Object ^sender, System::EventArgs ^e) {
				 if (is_in_update) return;
				 int checked = ((CheckBox^)sender)->Checked;
				 msclr::interop::marshal_context cxt; 
				 const char *flagname = cxt.marshal_as<char const*>(((CheckBox^)sender)->Name);
				 if (checked) _llUtils()->EnableFlag(flagname);
				 else _llUtils()->DisableFlag(flagname);
				 update_all_tabs(0, NULL);
			 }

	private: System::Void my_SelectedIndexChanged(System::Object ^sender, System::EventArgs ^e) {
				 if (is_in_update) return;
				 msclr::interop::marshal_context cxt; 
				 int selectedIndex = ((ComboBox^)sender)->SelectedIndex;

				 for (int i=0; i<(((ComboBox^)sender)->Items->Count); i++) {
					 const char *flagdes = 
						 cxt.marshal_as<char const*>(((ComboBox^)sender)->GetItemText(((ComboBox^)sender)->Items[ i ]));
					 //get flag from description
					 char *flagname = _llUtils()->GetFlagViaDescription(flagdes);
					 if (flagname) {
						 if (selectedIndex != i)
							 _llUtils()->DisableFlag(flagname);
						 else
							 _llUtils()->EnableFlag(flagname);
					 }
				 }
				 update_all_tabs(0, NULL);
			 }

	private: System::Void checkBox1_CheckedChanged(System::Object ^sender, System::EventArgs ^e) {
			 }

	private: System::Void update_ws_values(System::Object ^sender) {
				 int idx = comboBox1->SelectedIndex;
				 char *worldspace      = "<Not selected>";
				 char *worldspaceid    = "<Not selected>";
				 char *worldspaceidhex = "<Not selected>";
				 _llUtils()->AddFlag("_worldspace");
				 _llUtils()->AddFlag("_worldspaceid");
				 _llUtils()->AddFlag("_worldspaceidhex");
				 _llUtils()->DisableFlag("_worldspace");
				 _llUtils()->DisableFlag("_worldspaceid");
				 _llUtils()->DisableFlag("_worldspaceidhex");
	 
				 if (idx>=0) {
					 msclr::interop::marshal_context cxt; 
					 const char *name = cxt.marshal_as<char const*>(((ComboBox^)sender)->SelectedItem->ToString()); 
					 mesg->WriteNextLine(LOG_DEBUG, "Selected [%s] from WS", name);
					 Dump();

					 char *tmp = _llUtils()->NewString(name);
					 for (unsigned int i=0; i<strlen(tmp); i++) {
						 if (tmp[i] == ' ') {
							 tmp[i] ='\0';
							 worldspace = _llUtils()->NewString(tmp);
							 _llUtils()->EnableFlag("_worldspace");
							 _llUtils()->SetValue("_worldspace", worldspace);

							 for (unsigned int j=0; j<strlen(tmp+i+1); j++) {
								 if (tmp[j+i+1] == '[') {
									 for (unsigned int k=j+1; k<strlen(tmp+i+1); k++) {
										 if (tmp[k+i+1] == ']') {
											 tmp[k+i+1] ='\0';
											 tmp[j+i+1] ='\0';
											 worldspaceidhex = tmp+i+j+2;
											 unsigned int tmp2;
											 sscanf(worldspaceidhex, "%X", &tmp2);
											 worldspaceid = new char[12];
											 sprintf(worldspaceid, "%u", tmp2);
											 _llUtils()->EnableFlag("_worldspaceid");
											 _llUtils()->EnableFlag("_worldspaceidhex");
											 _llUtils()->SetValue("_worldspaceid", worldspaceid);
											 _llUtils()->SetValue("_worldspaceidhex", worldspaceidhex);
										 }
									 }
								 }
							 }
						 }
					 }
				 }
				 update_all_tabs(0, NULL);
			 }

	private: System::Void comboBox1_SelectedIndexChanged(System::Object ^sender, System::EventArgs ^e) {
				 update_ws_values(sender);
			 }

	public: void backgroundWorker2_DoWork(Object ^sender, DoWorkEventArgs ^e) {
				DWORD dwWritten; 
				BOOL bSuccess = FALSE; 
				batch->GetNextLine(-1);
				for (;;) { 
					char *line = batch->GetNextLine(0);
					if ( !line ) break; 
					//mesg->WriteNextLine(MH_ERROR,"%s",line); 
					bSuccess = WriteFile(g_hChildStd_IN_Wr, line, strlen(line)+1, &dwWritten, NULL);
					if ( ! bSuccess ) break; 
					bSuccess = WriteFile(g_hChildStd_IN_Wr, "\n", 1, &dwWritten, NULL);
					if ( ! bSuccess ) break; 
				}

			}

	private: void backgroundWorker2_RunWorkerCompleted(Object^ /*sender*/, RunWorkerCompletedEventArgs ^e) {
				 // Close the pipe handle so the child process stops reading. 
				 CloseHandle(g_hChildStd_IN_Wr); 
				 g_hChildStd_IN_Wr=NULL;
			 }

	public: void backgroundWorker1_DoWork(Object ^sender, DoWorkEventArgs ^e) {
				backgroundWorker1->ReportProgress(0, gcnew String("Background reader started" + Environment::NewLine));
				if (!g_hChildStd_OUT_Rd) return;

				BOOL bSuccess = FALSE;
				DWORD dwRead, dwWritten; 
				char chBuf[1024+2];
				int nn=0;

				for (;;) { 
					bSuccess = ReadFile(g_hChildStd_OUT_Rd, chBuf, 1024, &dwRead, NULL);
					if(!bSuccess) {
						CloseHandle(g_hChildStd_OUT_Rd);
						g_hChildStd_OUT_Rd=NULL;
						dwRead = 0;
					}
					String ^cc = gcnew String("");
					for (int l=0; l<dwRead; l++) {
						char c = chBuf[l];
						if (c == '\n') {
							cc += gcnew String(Environment::NewLine);
							backgroundWorker1->ReportProgress(0, cc);
							cc = gcnew String("");
						} else if (c == '\t') {
							cc += gcnew String("    ");
						}
						else if (c<=0x80) {
							if ((_llUtils()->MyIsPrint(c) || c == ' ') && nn==0) {
								char *c1 = new char[3];
								sprintf(c1, "%c\0", c);
								cc += gcnew String(c1);
							}
							else if (_llUtils()->MyIsPrint(c)) nn++;
							else if (c == ' ') nn=0;
							else nn--;
						}
					}
					backgroundWorker1->ReportProgress(0, cc);
					System::Threading::Thread::Sleep(10);
					if (!bSuccess) break; 
				}
				backgroundWorker1->ReportProgress(0, gcnew String("Background reader finished" + Environment::NewLine));
			}

	public: void backgroundWorker1a_DoWork(Object^ sender, DoWorkEventArgs ^e) {
				backgroundWorker1a->ReportProgress(0, gcnew String("Background error reader started" + Environment::NewLine));

				BOOL bSuccess = FALSE;
				DWORD dwRead, dwWritten; 
				char chBuf[1024+2];
				int nn=0, first=1;

				if (g_hChildStd_ERR_Rd) 
					for (;;) { 
						//backgroundWorker1a->ReportProgress(0, "A");
						bSuccess = ReadFile(g_hChildStd_ERR_Rd, chBuf, 1024, &dwRead, NULL);
						if(!bSuccess) {
							CloseHandle(g_hChildStd_ERR_Rd);
							g_hChildStd_ERR_Rd = NULL;
							dwRead = 0;
						}
						String ^cc = gcnew String("");
						//backgroundWorker1a->ReportProgress(0, "B");
						for (int l=0; l<dwRead; l++) {
							char c = chBuf[l];
							if (first) {
								cc += gcnew String("[APPLICATION ERROR] ");
								first=0;
							}
							if (c == '\n') {
								cc += gcnew String(Environment::NewLine);
								backgroundWorker1a->ReportProgress(0, cc);
								cc = gcnew String("");
								first=1;
							} else if (c == '\t') {
								cc += gcnew String("    ");
							}
							else if (c<=0x80) {
								if ((_llUtils()->MyIsPrint(c) || c == ' ') ) {
									char *c1 = new char[3];
									sprintf(c1,"%c\0",c);
									cc += gcnew String(c1);
								}
							}
						}
						backgroundWorker1a->ReportProgress(0, cc);
						System::Threading::Thread::Sleep(10);
						if (!bSuccess) break; 
					}
					backgroundWorker1a->ReportProgress(0, gcnew String("Background error reader finished" + Environment::NewLine));
			}


	private: void backgroundWorker1_RunWorkerCompleted(Object ^ /*sender*/, RunWorkerCompletedEventArgs ^e) {
				 cancelProcessToolStripMenuItem->Enabled = false;
				 tab_ws->Enabled = true;
				 tab_plugins->Enabled = true;
				 //if (g_hChildStd_ERR_Rd) CloseHandle(g_hChildStd_ERR_Rd);
				 //g_hChildStd_ERR_Rd=NULL;
				 for (int w=0; w<tab_app->Length; w++) {
					 tab_app[w]->Enabled = true;
				 }
			 }

	private: void backgroundWorker1_ProgressChanged(Object ^ /*sender*/, ProgressChangedEventArgs ^e) {
				 textBox1->AppendText((String^)e->UserState);
				 msclr::interop::marshal_context cxt; 
				 const char *text = cxt.marshal_as<char const*>((String^)e->UserState);
				 mesg->Log(text);
			 }

	private: void backgroundWorker1a_RunWorkerCompleted(Object ^ /*sender*/, RunWorkerCompletedEventArgs ^e) {
			 }

	private: void backgroundWorker1a_ProgressChanged(Object ^ /*sender*/, ProgressChangedEventArgs ^e) {
				 textBox1->AppendText((String^)e->UserState);
				 msclr::interop::marshal_context cxt; 
				 const char *text = cxt.marshal_as<char const*>((String^)e->UserState);
				 mesg->Log(text);
			 }


	private: System::Void cancelProcessToolStripMenuItem_Click(System::Object ^sender, System::EventArgs ^e) {

				 HANDLE ps = OpenProcess(SYNCHRONIZE|PROCESS_TERMINATE, FALSE, child_pid);
				 TerminateProcess(ps, -1);
				 CloseHandle(ps);

				 this->backgroundWorker1->CancelAsync();
				 this->backgroundWorker1a->CancelAsync();
				 this->backgroundWorker2->CancelAsync();
#if 0
				 if (g_hChildStd_OUT_Rd) CloseHandle(g_hChildStd_OUT_Rd);
				 g_hChildStd_OUT_Rd=NULL;
				 if (g_hChildStd_ERR_Rd) CloseHandle(g_hChildStd_ERR_Rd);
				 g_hChildStd_ERR_Rd=NULL;
				 if (g_hChildStd_IN_Wr) CloseHandle(g_hChildStd_IN_Wr);
				 g_hChildStd_IN_Wr=NULL;
#endif
				 tab_ws->Enabled = true;
				 tab_plugins->Enabled = true;
				 for (int w=0; w<tab_app->Length; w++) {
					 tab_app[w]->Enabled = true;
				 }
				 cancelProcessToolStripMenuItem->Enabled = false;
			 }
	private: System::Void saveBatchToolStripMenuItem_Click(System::Object ^sender, System::EventArgs ^e) {
				 SaveFileDialog ^saveFileDialog1 = gcnew SaveFileDialog;

				 //saveFileDialog1->InitialDirectory = Directory::GetCurrentDirectory();
				 saveFileDialog1->InitialDirectory = searchdir;

				 saveFileDialog1->Filter = "Batch Files|*.mpb";
				 saveFileDialog1->Title = "Select a Batch File";
				 saveFileDialog1->RestoreDirectory = true;

				 msclr::interop::marshal_context cxt; 

				 // Show the Dialog.
				 if (saveFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
					 batch->SaveFile(cxt.marshal_as<char const*>(saveFileDialog1->FileName));
				 }

			 }
	private: System::Void referenceManualToolStripMenuItem_Click(System::Object ^sender, System::EventArgs ^e) {
				 if (System::IO::File::Exists(installdir + L"\\doc\\REFERENCE_MANUAL.pdf"))
					 System::Diagnostics::Process::Start(installdir + L"\\doc\\REFERENCE_MANUAL.pdf");
			 }
	private: System::Void userReadmeToolStripMenuItem_Click(System::Object ^sender, System::EventArgs ^e) {
				 if (System::IO::File::Exists(installdir + L"\\doc\\USER_README.pdf"))
					 System::Diagnostics::Process::Start(installdir + L"\\doc\\USER_README.pdf");
			 }
	private: System::Void button1_Click(System::Object ^sender, System::EventArgs ^e) {

				 FillPlugins();
			 }
	};
}

