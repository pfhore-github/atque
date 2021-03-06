// -*- C++ -*- generated by wxGlade 0.6.3 on Sat Sep  6 18:07:53 2008

#include "atque.h"
#include "split.h"
#include "merge.h"
#include <iostream>
#include <sstream>

#include <wx/config.h>
#include <wx/filename.h>

// begin wxGlade: ::extracode
// end wxGlade

AtqueWindow::AtqueWindow(wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
	wxFrame(parent, id, title, pos, size, wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU
#if defined(__WIN32__) || (defined(__APPLE__) && defined(__MACH__))
		| wxSIMPLE_BORDER
#endif
		)
{
    // begin wxGlade: AtqueWindow::AtqueWindow
    panel_1 = new wxPanel(this, wxID_ANY);
    menuBar = new wxMenuBar();
    wxMenu* fileMenu = new wxMenu();
    fileMenu->Append(wxID_ABOUT, wxT("Atqueについて"), wxEmptyString, wxITEM_NORMAL);
    fileMenu->AppendSeparator();
    fileMenu->Append(MENU_Split, wxT("分割"), wxEmptyString, wxITEM_NORMAL);
    fileMenu->Append(MENU_Merge, wxT("マージ"), wxEmptyString, wxITEM_NORMAL);
    fileMenu->Append(wxID_EXIT, wxT("終了(&Q)"), wxEmptyString, wxITEM_NORMAL);
    menuBar->Append(fileMenu, wxT("ファイル(&F)"));
    SetMenuBar(menuBar);
    instructions = new wxStaticText(panel_1, wxID_ANY, wxT("ファイルをドラッグ＆ドロップすると分割。フォルダをドラッグ＆ドロップするとマージ。"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);

    set_properties();
    do_layout();
    // end wxGlade

    instructions->SetDropTarget(new AtqueDnD(this));
}


BEGIN_EVENT_TABLE(AtqueWindow, wxFrame)
    // begin wxGlade: AtqueWindow::event_table
    EVT_MENU(wxID_ABOUT, AtqueWindow::OnAbout)
    EVT_MENU(MENU_Split, AtqueWindow::OnSplit)
    EVT_MENU(MENU_Merge, AtqueWindow::OnMerge)
    EVT_MENU(wxID_EXIT, AtqueWindow::OnExit)
    // end wxGlade
END_EVENT_TABLE();


void AtqueWindow::OnAbout(wxCommandEvent &event)
{
    wxMessageBox(wxT("Atque 1.1.1 (C) 2008-2011 Gregory Smith\n\nAtqueはGNU GPLでライセンスされています。詳細はCOPYING.txtにて"), wxT("Atqueについて"), wxOK);
}


void AtqueWindow::OnMerge(wxCommandEvent &event)
{
    // find a directory to merge
    wxConfig config;
    wxString Directory;
    config.Read(wxT("Merge/DefaultDirectory/Load"), &Directory, wxT(""));
    wxDirDialog *openDirDialog = new wxDirDialog(this,
						 wxT("マージしたいフォルダを選んでください。"),
						 Directory,
						 wxDD_DIR_MUST_EXIST);
    if (openDirDialog->ShowModal() == wxID_OK)
    {
	Merge(openDirDialog->GetPath());
    }
}


void AtqueWindow::OnSplit(wxCommandEvent &event)
{
    // find the user's file
    wxConfig config;
    wxString Directory;
    config.Read(wxT("Split/DefaultDirectory/Load"), &Directory, wxT(""));
    wxFileDialog *openFileDialog = new wxFileDialog(this,
						    wxT("分割したいファイルを選んでください。"),
						    Directory,
						    wxT(""),
						    wxT("シナリオファイル|*.sceA;*.imgA|全てのファイル|*.*"),
						    wxFD_OPEN);
    if (openFileDialog->ShowModal() == wxID_OK)
    {
	Split(openFileDialog->GetPath());
    }
}


void AtqueWindow::OnExit(wxCommandEvent &event)
{
	Close(TRUE);
}


// wxGlade: add AtqueWindow event handlers

void AtqueWindow::Split(const wxString& file)
{
    // find the split destination
    wxConfig config;
    wxString Directory;
    config.Read(wxT("Split/DefaultDirectory/Save"), &Directory, wxT(""));
    wxFileDialog *saveFileDialog = new wxFileDialog(this,
						    wxT("分割したファイルの保存先のフォルダ"),
						    Directory,
						    wxT("Split Map Folder"),
						    wxT(""),
						    wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog->ShowModal() == wxID_OK)
    {
	wxBusyCursor busy;
	wxString filename = saveFileDialog->GetPath();
	config.Write(wxT("Split/DefaultDirectory/Save"), filename.BeforeLast('/'));
	config.Write(wxT("Split/DefaultDirectory/Load"), file.BeforeLast('/'));
	if (wxFileName::FileExists(filename))
	{
	    wxRemoveFile(filename);
	}
	std::stringstream log;

	wxLogWindow* logWindow = new wxLogWindow(this,
						 wxT("分割ログ（") + file + wxT("）"),
						 true,
						 false);
	try
	{
		atque::split(std::string(file.mb_str(wxConvUTF8)), std::string(filename.mb_str(wxConvUTF8)), log);
		
		log.seekg(0);
		std::string line;
		while (getline(log, line))
		{
			wxLogMessage(wxString(line.c_str(), wxConvUTF8));
		}
		wxLogMessage(wxT("分割に成功しました。"));
	}
	catch (const atque::split_error& e)
	{
		wxLogMessage(wxT("分割に失敗：" + wxString(e.what(), wxConvUTF8)));
	}
    }
}

void AtqueWindow::Merge(const wxString& folder)
{
    // find the merge destination
    wxConfig config;
    wxString Directory;
    config.Read(wxT("Merge/DefaultDirectory/Save"), &Directory, wxT(""));
    wxFileDialog *saveFileDialog = new wxFileDialog(this,
						    wxT("マージしたファイルの保存先："),
						    wxT(""),
						    wxT("マージ済みマップ.sceA"),
						    wxT(""),
						    wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog->ShowModal() == wxID_OK)
    {
	wxBusyCursor busy;
	wxString filename = saveFileDialog->GetPath();

	config.Write(wxT("Merge/DefaultDirectory/Save"), filename.BeforeLast('/'));
	config.Write(wxT("Merge/DefaultDirectory/Load"), folder.BeforeLast('/'));

	std::stringstream log;
	wxLogWindow* logWindow = new wxLogWindow(this,
						 wxT("マージログ（") + filename + wxT("）"),
						     true,
						     false);
	try
	{
		atque::merge(std::string(folder.mb_str(wxConvUTF8)), std::string(filename.mb_str(wxConvUTF8)), log);
		
		log.seekg(0);
		std::string line;
		while (getline(log, line))
		{
			wxLogMessage(wxString(line.c_str(), wxConvUTF8));
		}
		wxLogMessage(wxT("マージは成功しました。"));
	}
	catch (const atque::merge_error& e)
	{
		wxLogMessage(wxT("マージ失敗：" + wxString(e.what(), wxConvUTF8)));
	}
    }
}

void AtqueWindow::set_properties()
{
    // begin wxGlade: AtqueWindow::set_properties
    SetTitle(wxT("Atque"));
    // end wxGlade
}


void AtqueWindow::do_layout()
{
    // begin wxGlade: AtqueWindow::do_layout
    wxBoxSizer* sizer_1 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* sizer_2 = new wxBoxSizer(wxVERTICAL);
    sizer_2->Add(instructions, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5);
    panel_1->SetSizer(sizer_2);
    sizer_1->Add(panel_1, 1, wxEXPAND, 0);
    SetSizer(sizer_1);
    sizer_1->Fit(this);
    Layout();
    // end wxGlade
}

bool AtqueDnD::OnDropFiles(wxCoord, wxCoord, const wxArrayString& filenames)
{
	if (filenames.GetCount() == 1)
	{
		if (wxFileName::DirExists(filenames[0]))
		{
			window_->Merge(filenames[0]);
			return true;
		}
		else if (wxFileName::FileExists(filenames[0]))
		{
			window_->Split(filenames[0]);
			return true;
		}
	}

	return false;
}

class Atque: public wxApp {
public:
    bool OnInit();
#ifdef __WXMAC__
	void MacOpenFile(const wxString& filename);
#endif
private:
	AtqueWindow* Frame;
};

IMPLEMENT_APP(Atque)

bool Atque::OnInit()
{
    wxInitAllImageHandlers();
    Frame = new AtqueWindow(NULL, wxID_ANY, wxEmptyString);
    SetTopWindow(Frame);
    Frame->Show();
    return true;
}

#ifdef __WXMAC__
void Atque::MacOpenFile(const wxString& filename)
{
	if (wxFileName::DirExists(filename))
	{
		Frame->Merge(filename);
	}
	else if (wxFileName::FileExists(filename))
	{
		Frame->Split(filename);
	}
}
#endif

