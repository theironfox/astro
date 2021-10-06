#include "ipanel.h"
#include <wx/msgdlg.h>
#include "strdef.h"
#include <stdio.h>
#include <iomanip>
#include "main.h"
#include <string.h>
#include "swephexp.h"

extern simple* Simple;
extern std::string tmp_string;

//SWEPH
extern int day, month, year;
extern double minute, hour, second;
extern double Julian_Date;
//

//spacing information: text and control = 25
//		       text and combobox = 30

inputPanel::inputPanel(wxPanel *parent)
	: wxPanel(parent, wxID_ANY, wxPoint(4,4), wxSize(300,800), wxDOUBLE_BORDER)
{
	
	m_parent = parent; //This stores it's own parent control for later reference..

//name control and label	
	wxStaticText *nameLabel = new wxStaticText(this, wxID_ANY, wxT("Name:"), wxPoint(5,10), wxDefaultSize);
	nameTctl = new wxTextCtrl(this, wxID_ANY, wxT(""), wxPoint(5,35), wxSize(150,20));

//date control and label
	wxStaticText *dateLabel = new wxStaticText(this, wxID_ANY, wxT("Date:"), wxPoint(5,65), wxDefaultSize);
	wxStaticText *dateLabelHint = new wxStaticText(this, wxID_ANY, wxT("(dd/mm/yyyy)"), wxPoint(50,65), wxDefaultSize);
	dateLabelHint->SetForegroundColour(wxColor(*wxLIGHT_GREY));
	dateATctl = new wxTextCtrl(this, wxID_ANY, wxT(""), wxPoint(5,90), wxSize(150,20));
	dateATctl->Bind(wxEVT_TEXT, &inputPanel::DateCheck, this);

//panel contains time settings including radio buttons
	wxPanel *TimePanel = new wxPanel(this, wxID_ANY, wxPoint(1,120), wxSize(280,60));
	wxStaticText *timeLabel = new wxStaticText(TimePanel, wxID_ANY, wxT("Time:"), wxPoint(5,1), wxDefaultSize);
	timeTctl = new wxTextCtrl(TimePanel, wxID_ANY, wxT(""), wxPoint(5,25), wxSize(150,20));
	radio1 = new wxRadioButton(TimePanel, wxID_ANY, wxT("AM"), wxPoint(160,25), wxDefaultSize);
	wxRadioButton *radio2 = new wxRadioButton(TimePanel, wxID_ANY, wxT("PM"), wxPoint(210,25), wxDefaultSize);

//Timezone
	wxStaticText *tzLabel = new wxStaticText(this, wxID_ANY, wxT("TimeZone:"), wxPoint(5,175), wxDefaultSize);
	tzoneCombo = new wxComboBox(this, wxID_ANY, wxEmptyString, wxPoint(5,199), wxDefaultSize);
	wxCheckBox *lmtCheck = new wxCheckBox(this, wxID_ANY, wxT("LMT (Local Mean Time)"), wxPoint(5,230), wxDefaultSize);
	dlsCheck = new wxCheckBox(this, wxID_ANY, wxT("Daylight saving time"), wxPoint(5,250), wxDefaultSize);

//Location data
	wxStaticText *CountryLabel = new wxStaticText(this, wxID_ANY, wxT("Country:"), wxPoint(5,295), wxDefaultSize);
	wxComboBox *CntryCombo = new wxComboBox(this, wxID_ANY, wxEmptyString, wxPoint(5,320), wxSize(150,30));
	wxStaticText *CityLabel = new wxStaticText(this, wxID_ANY, wxT("City:"), wxPoint(5,350), wxDefaultSize);
	wxComboBox *CityCombo = new wxComboBox(this, wxID_ANY, wxEmptyString, wxPoint(5,375), wxSize(150,30));
	wxStaticText *CustomLabel = new wxStaticText(this, wxID_ANY, wxT("Custom:"), wxPoint(5,405), wxDefaultSize);
	CustomLabel->SetForegroundColour(wxColor(*wxLIGHT_GREY));
	wxComboBox *CustomCombo = new wxComboBox(this, wxID_ANY, wxEmptyString, wxPoint(5,430), wxSize(150,30));
	wxCheckBox *enableCheck = new wxCheckBox(this, wxID_ANY, wxT("Enable"), wxPoint(170, 430), wxDefaultSize);

//Buttons
	wxButton *calcBttn = new wxButton(this, cBttnId, wxT("OK"), wxPoint(5, 750), wxDefaultSize);
//Save data
	Populate();
	
}

void inputPanel::Populate(void)
{
	hour = 0;
	for(int index = 0; index < 38; index++)
	{
		tzoneCombo->Append(timeZones[index]);
	}
	radio1->SetValue(true); //Windows requirement.
}

void inputPanel::OnCalculate(wxCommandEvent& event)
{
	bool am = true;
	bool dls = false; //Day light saving
	std::string strAMPM = "AM";
	std::cout << std::fixed;
	std::cout << std::setprecision(6);

	std::string tString = std::string(nameTctl->GetValue().mb_str());
	std::string tD = std::string(dateATctl->GetValue().mb_str());
	std::string tTM = std::string(timeTctl->GetValue().mb_str());
	/*****************************************
	*	Time input and handling		 *
	*****************************************/
	std::string tempint = timeZones[tzoneCombo->GetSelection()];
	std::stringstream tSstream;
	tSstream << tempint;
	int number;
	tSstream >> number;
	std::cout << number + 1;


	switch(tTM.length()) //This works well but is clumsy, clean up when possible
	{
		case 0:
			wxMessageBox(wxT("Missing information: Time"), wxT("Time input error"), wxICON_ERROR);
		break;
		case 1:
			hour = (tTM.at(0) - 48);
		break;
		case 2:
			hour = ((tTM.at(0) - 48) * 10) + (tTM.at(1) - 48);
		break;
		case 4:
			hour = (tTM.at(0) - 48);
			minute = ((tTM.at(2) - 48) * 10) + (tTM.at(3) - 48);
			hour = hour + (minute / 60);
		break;
		case 5:
			hour = ((tTM.at(0) - 48) * 10) + (tTM.at(1) - 48);
			minute = ((tTM.at(3) - 48) * 10) + (tTM.at(4) - 48);
			hour = hour + (minute / 60);
		break;
		case 7:
			hour = ((tTM.at(0) - 48) * 10) + (tTM.at(1) - 48);
			minute = ((tTM.at(3) - 48) * 10) + (tTM.at(4) - 48);
			second = (tTM.at(6) - 48);
			hour = hour + (minute /60) + (second / 3600);
		break;
		case 8:
			hour = ((tTM.at(0) - 48) * 10) + (tTM.at(1) - 48);
			minute = ((tTM.at(3) - 48) * 10) + (tTM.at(4) - 48);
			second = ((tTM.at(6) - 48) * 10) + (tTM.at(7) - 48);
			hour = hour + (minute /60) + (second / 3600);
		break;
		case 9:
			hour = ((tTM.at(0) - 48) * 10) + (tTM.at(1) - 48);
			minute = ((tTM.at(3) - 48) * 10) + (tTM.at(4) - 48);
			second = ((tTM.at(6) - 48) * 100) + ((tTM.at(7) - 48) * 10) + (tTM.at(8) - 48);
			hour = hour + (minute /60) + (second / 3600);
		default:
			wxMessageBox(wxT("Check time is entered correctly"), wxT("Time input error"), wxICON_ERROR);
			std::cout << "Unrecognised time input" << std::endl;
		break;
	}
	if(!radio1->GetValue())
	{
		am = false;
		strAMPM = "PM";
	}
	if(dlsCheck->GetValue())
	{
		dls = true;
		strAMPM = strAMPM + " (Day light savings time)";
		hour = hour + 1;
	}
	/*****************************************
	*	Date input and handling		 *
	*****************************************/
	if((tD.length() > 10) || (tD.length() < 8))	//TODO MUST ACCOMODATE OR || 
	{
		std::cout << "Incorrect date format" << std::endl;
	}
	if(tD.length() == 10)
	{
		day = ((tD.at(0) - 48) * 10) + (tD.at(1) - 48);
		month = ((tD.at(3) - 48) * 10) + (tD.at(4) - 48);
		year = ((tD.at(6) - 48) * 1000) + ((tD.at(7) - 48) * 100) + ((tD.at(8) - 48) * 10) + (tD.at(9) - 48);
	}
	if(tD.length() == 9)
	{
		day = ((tD.at(0) - 48) * 10) + (tD.at(1) - 48);
		month = (tD.at(3) - 48);
		year = ((tD.at(5) - 48) * 1000) + ((tD.at(6) - 48) * 100) + ((tD.at(7) - 48) * 10) + (tD.at(8) - 48);
	}

//	std::cout << tempString.length() << std::endl;
	tString = tString + "\nDate: ";
	tString = tString + std::to_string(day) + "/" + std::to_string(month) + "/" + std::to_string(year);




	if((int)minute == 0 && (int)second == 0)
	{
		tString = tString + "\nTime: " + std::to_string((int)hour) + " " + strAMPM;
	}
	else
	{
		tString = tString + "\nTime: " + std::to_string((int)hour) + ":" + std::to_string((int)minute);
		tString = tString + ":" + std::to_string(second) +  strAMPM;
	}
	
	if(!am)
	{
		hour = hour + 12;
	}
	hour = hour - tZoneD[tzoneCombo->GetSelection()];
	Julian_Date = swe_julday(year, month, day, hour, SE_GREG_CAL);
		
	tString = tString + "\nJulian Date: " + std::to_string(Julian_Date);

	Simple->UpdateNP(tString);

}

void inputPanel::DateCheck(wxCommandEvent& event)
{
}
