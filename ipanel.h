#ifndef _PANEL_H_
#define _PANEL_H_
#include <wx/wx.h>
#include <wx/notebook.h>
#include <stdio.h>

class inputPanel : public wxPanel
{
public:
	inputPanel(wxPanel *parent);
//Values
	std::string name;
	wxPanel *m_parent;
	wxRadioButton *radio1;

	wxTextCtrl *nameTctl;
	wxTextCtrl *dateATctl;
	wxTextCtrl *timeTctl;
	wxTextCtrl *lngDeg;
	wxTextCtrl *lngMin;
	wxTextCtrl *lngSec;
	wxTextCtrl *latDeg;
	wxTextCtrl *latMin;
	wxTextCtrl *latSec;

	wxStaticText *CountryLabel;
	wxStaticText *CityLabel;
	wxStaticText *CustomLabel;

	wxCheckBox *dlsCheck;
	wxComboBox *tzoneCombo;
	wxComboBox *CntryCombo;
	wxComboBox *CityCombo;
	wxComboBox *CustomCombo;

	wxCheckBox *enableCheck;
//Events
	void OnCalculate(wxCommandEvent& event);
	void DateCheck(wxCommandEvent& event);
	void CntySelect(wxCommandEvent& event);
	void OnCustom(wxCommandEvent& event);
	void Populate(void);
	DECLARE_EVENT_TABLE()
};

enum
{
	cBttnId = wxID_HIGHEST + 1,
	dInptId = wxID_HIGHEST + 20
};
#endif
