#ifndef MAIN_H
#define MAIN_H
#include "ipanel.h"
#include <wx/wx.h>
#include <string.h>

extern bool update;

struct dataHolder
{
   public:
	char* name;
	double date;
};

class simple : public wxFrame
{
public:
	simple(const wxString& title);
	inputPanel* ipanel;
	wxTextCtrl* textCtrl1;
	void OnQuit(wxCommandEvent& event);
	void OnOpen(wxCommandEvent& event);
	void OnSave(wxCommandEvent& event);
	void OnCData(wxCommandEvent& event);
	void OnPrint(wxCommandEvent& event);
	void UpdateNP(std::string str);
};

extern dataHolder outData;

#endif
