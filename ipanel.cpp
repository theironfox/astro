#include "ipanel.h"
//#include <wx/datectrl.h>
#include <stdio.h>
#include <iomanip>
#include "main.h"
#include <string.h>
#include "swephexp.h"
extern simple* Simple;
extern std::string tmp_string;

//SWEPH
extern int minute, hour, day, month, year;
extern double Julian_Date;
//

std::string timeZones[38] = {
			    "-12:00","-11:00", "-10:00", "-9:30", "-9:00", "-8:00", "-7:00", "-6:00", "-5:00",
			    "-4:00", "-3:30", "-3:00", "-2:00", "-1:00", "0:00", "1:00", "2:00", "3:00", "3:30",
			    "4:00", "4:30", "5:00", "5:30", "5:45", "6:00", "6:30", "7:00", "8:00", "8:45", "9:00",
			    "9:30", "10:00", "10:30", "11:00", "12:00", "12:45", "13:00", "14:00"};

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
}

void inputPanel::OnCalculate(wxCommandEvent& event)
{
	bool am = true;
	bool dls = false; //Day light saving
	std::string strAMPM = "AM";
	std::cout << std::fixed;
	std::cout << std::setprecision(6);

	std::string tempString = std::string(nameTctl->GetValue().mb_str());
	std::string tD = std::string(dateATctl->GetValue().mb_str());
	std::string tTM = std::string(timeTctl->GetValue().mb_str());
	/*****************************************
	*	Time input and handling		 *
	*****************************************/
	std::cout << tTM << std::endl;
	if(tTM.length() != 5)
	{
		std::cout << "Incorrect time format" << std::endl;
	}
	if(tTM.length() == 5)
	{
		hour = ((tTM.at(0) - 48) * 10) + (tTM.at(1) - 48);
		minute = ((tTM.at(3) - 48) * 10) + (tTM.at(4) - 48);
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
//	std::cout << tempString.length() << std::endl;
	tempString = tempString + "\nDate: ";
	tempString = tempString + std::to_string(day) + "/" + std::to_string(month) + "/" + std::to_string(year);
	Julian_Date = swe_julday(year, month, day, hour, SE_GREG_CAL);
	
	tempString = tempString + "\nTime: " + std::to_string(hour) + ":" + std::to_string(minute) + " " + strAMPM;

	tempString = tempString + "\nJulian Date: " + std::to_string(Julian_Date);

	Simple->UpdateNP(tempString);

}

void inputPanel::DateCheck(wxCommandEvent& event)
{
}
