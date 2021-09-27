#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/filedlg.h>
#include "ipanel.h"
#include "splash.h"
#include "swephexp.h"
#include <stdio.h>
#include "main.h"

		/*************************
		*Swiss ephemeris globals *
		*************************/
int iflag = SEFLG_SWIEPH | SEFLG_SPEED | SEFLG_EQUATORIAL;
double Julian_Date;
int day, month, year;
double minute, hour, second;

/**********************************************************/
std::string tmp_string;

bool update = false;
dataHolder outData;

int ID_OPEN = 2001;
int ID_SAVE = 2002;
simple* Simple;


class mainApp : public wxApp
{
public:
	virtual bool OnInit();
};



simple::simple(const wxString& title)
	: wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(1210,860))
{
	wxMenu *mainMenu = new wxMenu;
	mainMenu->Append(wxID_ANY, "New", "item1");
	mainMenu->Append(ID_OPEN, "Open", "item2");
	mainMenu->Append(ID_SAVE,"Save", "item3");
	mainMenu->AppendSeparator();
	mainMenu->Append(wxID_ANY, "Export PDF", "item4");
	mainMenu->AppendSeparator();
	mainMenu->Append(wxID_EXIT,"Quit", "item4");

	wxMenu *locMenu = new wxMenu;
	locMenu->Append(wxID_ANY, "Custom Data", "item1");

	wxMenuBar *mainMBar = new wxMenuBar;
	mainMBar->Append(mainMenu, "&File");
	mainMBar->Append(locMenu, "&Settings");
	SetMenuBar(mainMBar);

	Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(simple::OnQuit));
	Connect(ID_OPEN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(simple::OnOpen));
	Connect(ID_SAVE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(simple::OnSave));

	wxPanel *panel = new wxPanel(this, wxID_ANY);
	ipanel = new inputPanel(panel);
	wxNotebook *notebook = new wxNotebook(panel, wxID_ANY, wxPoint(305,5), wxSize(850,798));
	textCtrl1 = new wxTextCtrl(notebook, wxID_ANY, L"Tab 1 Contents", wxPoint(1,1), wxSize(50,29), wxTE_MULTILINE|wxTE_READONLY);
    notebook->AddPage(textCtrl1, L"Data");
    wxPanel *dummyPanel = new wxPanel(notebook, wxID_ANY);
    notebook->AddPage(dummyPanel, L"Chart");

//
	textCtrl1->ChangeValue("Name:");
//
	
	Centre();
}

void simple::UpdateNP(std::string str)
{
	textCtrl1->ChangeValue("Name: " + str);
}

void simple::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close(true);
}

void simple::OnSave(wxCommandEvent& WXUNUSED(event))
{
    wxFileDialog
        saveFileDialog(this, _("Save DSB file"), "", "",
                       "DSB files (*.dsb)|*.dsb", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    if (saveFileDialog.ShowModal() == wxID_CANCEL)
        return;     // the user changed idea...
	std::cout << "on save" << std::endl;
}

void simple::OnOpen(wxCommandEvent& WXUNUSED(event))
{
	    wxFileDialog 
        openFileDialog(this, _("Open DSB file"), "", "",
                       "DSB files (*.dsb)|*.dsb", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;     // the user changed idea...
}

  BEGIN_EVENT_TABLE(inputPanel, wxPanel)
  EVT_BUTTON (cBttnId, inputPanel::OnCalculate)
  END_EVENT_TABLE()

//IMPLEMENT_APP(mainApp)
IMPLEMENT_APP_NO_MAIN(mainApp) 

int main(int argc, char **argv) 
{ 

	wxEntry(argc, argv); 
	return 0;
}

bool mainApp::OnInit()
{

/***********************************
*initialize swiss ephemeris library*
***********************************/

	swe_set_ephe_path((char*)"./");


/**********************************/
	 Simple = new simple(wxT("Simple"));


//	splash *Splash = new splash(wxT("Splash screen"));
//	Splash->Show(true);
	Simple->Show(true);
	return true;
}

