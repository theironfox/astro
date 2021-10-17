#include "ipanel.h"
#include <wx/msgdlg.h>
#include "strdef.h"
#include <stdio.h>
#include <iomanip>
#include "main.h"
#include <string.h>
#include "swephexp.h"
#include <fstream>

extern simple* Simple;
extern std::string tmp_string;

char serr[256];
char pName[24];
//SWEPH
extern int iflag;
extern int day, month, year;
extern double minute, hour, second;
extern double Julian_Date, Te_Date;
extern double lgtData, latData;
extern double pdat[6];
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
	timeTctl = new wxTextCtrl(TimePanel, wxID_ANY, wxT(""), wxPoint(5,25), wxSize(40,20));
	minTctl = new wxTextCtrl(TimePanel, wxID_ANY, wxT(""), wxPoint(45,25), wxSize(40,20));
	secTctl = new wxTextCtrl(TimePanel, wxID_ANY, wxT(""), wxPoint(85,25), wxSize(40,20));
	radio1 = new wxRadioButton(TimePanel, wxID_ANY, wxT("AM"), wxPoint(160,25), wxDefaultSize);
	wxRadioButton *radio2 = new wxRadioButton(TimePanel, wxID_ANY, wxT("PM"), wxPoint(210,25), wxDefaultSize);

//Timezone
	wxStaticText *tzLabel = new wxStaticText(this, wxID_ANY, wxT("TimeZone:"), wxPoint(5,175), wxDefaultSize);
	tzoneCombo = new wxComboBox(this, wxID_ANY, wxEmptyString, wxPoint(5,199), wxDefaultSize);
	wxCheckBox *lmtCheck = new wxCheckBox(this, wxID_ANY, wxT("LMT (Local Mean Time)"), wxPoint(5,230), wxDefaultSize);
	dlsCheck = new wxCheckBox(this, wxID_ANY, wxT("Daylight saving time"), wxPoint(5,250), wxDefaultSize);

//Location data
	CountryLabel = new wxStaticText(this, wxID_ANY, wxT("Country:"), wxPoint(5,295), wxDefaultSize);
	CntryCombo = new wxComboBox(this, wxID_ANY, wxEmptyString, wxPoint(5,320), wxSize(150,30));
	CntryCombo->Bind(wxEVT_COMBOBOX, &inputPanel::CntySelect, this);
	
	CityLabel = new wxStaticText(this, wxID_ANY, wxT("City:"), wxPoint(5,350), wxDefaultSize);
	CityCombo = new wxComboBox(this, wxID_ANY, wxEmptyString, wxPoint(5,375), wxSize(150,30));
	
	CustomLabel = new wxStaticText(this, wxID_ANY, wxT("Custom Data:"), wxPoint(5,405), wxDefaultSize);
	CustomLabel->SetForegroundColour(wxColor(*wxLIGHT_GREY));
	CustomCombo = new wxComboBox(this, wxID_ANY, wxEmptyString, wxPoint(5,430), wxSize(150,30));
	CustomCombo->Disable();
	
	enableCheck = new wxCheckBox(this, wxID_ANY, wxT("Custom Data"), wxPoint(170, 430), wxDefaultSize);
	enableCheck->Bind(wxEVT_CHECKBOX, &inputPanel::OnCustom, this);
	
	wxPanel *cdPanel = new wxPanel(this, wxID_ANY, wxPoint(0,480), wxSize(250,100));
	wxStaticText *lngLabel = new wxStaticText(cdPanel, wxID_ANY, wxT("Longitude:"), wxPoint(5, 1), wxDefaultSize);
	lngDeg = new wxTextCtrl(cdPanel, wxID_ANY, wxT(""), wxPoint(5,25), wxSize(40,30));
	lngMin = new wxTextCtrl(cdPanel, wxID_ANY, wxT(""), wxPoint(50,25), wxSize(40,30));
	lngSec = new wxTextCtrl(cdPanel, wxID_ANY, wxT(""), wxPoint(95, 25), wxSize(40,30));
	radioLE = new wxRadioButton(cdPanel, wxID_ANY, wxT("E"), wxPoint(150, 25), wxDefaultSize);
	wxRadioButton *radioLW = new wxRadioButton(cdPanel, wxID_ANY, wxT("W"), wxPoint(190, 25), wxDefaultSize);

	wxPanel *latPanel = new wxPanel(this, wxID_ANY, wxPoint(0,540), wxSize(250,100));
	wxStaticText *latLabel = new wxStaticText(latPanel, wxID_ANY, wxT("Latitude:"), wxPoint(5, 1), wxDefaultSize);
	latDeg = new wxTextCtrl(latPanel, wxID_ANY, wxT(""), wxPoint(5,25), wxSize(40,30));
	latMin = new wxTextCtrl(latPanel, wxID_ANY, wxT(""), wxPoint(50,25), wxSize(40,30));
	latSec = new wxTextCtrl(latPanel, wxID_ANY, wxT(""), wxPoint(95, 25), wxSize(40,30));
	radioLN = new wxRadioButton(latPanel, wxID_ANY, wxT("N"), wxPoint(150,25), wxDefaultSize);
	wxRadioButton *radioLS = new wxRadioButton(latPanel, wxID_ANY, wxT("S"), wxPoint(190,25), wxDefaultSize);
//Buttons
	wxButton *calcBttn = new wxButton(this, cBttnId, wxT("OK"), wxPoint(5, 750), wxDefaultSize);
//Save data
	Populate();
	
}

void inputPanel::OnCustom(wxCommandEvent& event)
{
	if(enableCheck->GetValue())
	{
		CountryLabel->SetForegroundColour(wxColor(*wxLIGHT_GREY));
		CityLabel->SetForegroundColour(wxColor(*wxLIGHT_GREY));
		CustomLabel->SetForegroundColour(wxColor(*wxBLACK));
		CntryCombo->Disable();
		CityCombo->Disable();
		CustomCombo->Enable();
	}
	else
	{
		CountryLabel->SetForegroundColour(wxColor(*wxBLACK));
		CityLabel->SetForegroundColour(wxColor(*wxBLACK));
		CustomLabel->SetForegroundColour(wxColor(*wxLIGHT_GREY));
		CntryCombo->Enable();
		CityCombo->Enable();
		CustomCombo->Disable();
	}
}

void inputPanel::Populate(void)
{
	hour = 0;
	for(int index = 0; index < 38; index++)
	{
		tzoneCombo->Append(timeZones[index]);
	}
	radio1->SetValue(true); //Windows requirement.
	
	std::ifstream countryFile("./Data/Countries");
	std::string line;
	
	for(int index = 0; index < 31; index++)
	{
		std::getline(countryFile, line);
		CntryCombo->Append(line);
	}
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

	try{
		double deg = std::stod(std::string(lngDeg->GetValue().mb_str()));
		int mins = std::stoi(std::string(lngMin->GetValue().mb_str()));
		double secs = std::stod(std::string(lngSec->GetValue().mb_str()));
		if(deg + mins + secs > 0)
		{
			lgtData = deg + (mins/60) + (secs/3600);
			if(!radioLE->GetValue())
			{
				lgtData = -lgtData;
			}
		}
	}
	catch(...)
	{
		wxMessageBox(wxT("Check Longitude Data"), wxT("Calculation error"), wxICON_ERROR);
	}
	try{
		double deg = std::stod(std::string(latDeg->GetValue().mb_str()));
		int mins = std::stoi(std::string(latMin->GetValue().mb_str()));
		double secs = std::stod(std::string(latSec->GetValue().mb_str()));
		if(deg + mins + secs > 0)
		{
			latData = deg + (mins/60) + (secs/3600);
			if(!radioLN->GetValue())
			{
				latData = -latData;
			}
		}
	}
	catch(...)
	{
		wxMessageBox(wxT("Check Latitude Data"), wxT("Calculation error"), wxICON_ERROR);
	}

	try{
		int tmpHour = std::stoi(std::string(timeTctl->GetValue().mb_str()));;
		int tmpMinute = std::stoi(std::string(minTctl->GetValue().mb_str()));
		double tmpSecond = std::stod(std::string(secTctl->GetValue().mb_str()));
		minute = tmpMinute;
		second = tmpSecond;
		hour = tmpHour + (tmpMinute/60) + (tmpSecond/3600);
	}
	catch(...)
	{
		wxMessageBox(wxT("Check Time Data"), wxT("Calculation error"), wxICON_ERROR);
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
		//std::cout << "Incorrect date format" << std::endl;                                   ////////DEL COMMENT
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
		tString = tString + "." + std::to_string(second) +  strAMPM;
	}
	
	if(!am)
	{
		hour = hour + 12;
	}
	hour = hour - tZoneD[tzoneCombo->GetSelection()];
	Julian_Date = swe_julday(year, month, day, hour, SE_GREG_CAL);
	Te_Date = Julian_Date - swe_deltat_ex(Julian_Date, SEFLG_SWIEPH, serr);
		
	tString = tString + "\nJulian Date: " + std::to_string(Julian_Date);
	//tString = tString + "\nTe Date: " + std::to_string(Te_Date);
	{
		double deltaT = swe_deltat(Julian_Date) * 86400.0;;
		tString = tString + "\nDelta T: " + std::to_string(deltaT);
	}
	
	for(int i = SE_SUN; i < SE_PLUTO; i++)
	{
		std::cout << i << std::endl;
		swe_calc(Julian_Date, i, iflag, pdat, serr);
		swe_get_planet_name(i, pName);
		std::cout << pName << "  ";
		for(int ii = 0; ii < 6; ii++)
		{
			std::cout << pdat[i];
		}
		std::cout << std::endl;
	}
	
	Simple->UpdateNP(tString);

}

void inputPanel::DateCheck(wxCommandEvent& event)
{
}

void inputPanel::CntySelect(wxCommandEvent& event)
{
	CityCombo->Clear();
	std::stringstream tmpStream;
	std::string line;
	tmpStream << CntryCombo->GetSelection();
	std::string filePath = "./Data/" + tmpStream.str() + "/index";
	std::cout << filePath << std::endl;
	std::ifstream cityFile(filePath);
	int counter = 0;
	while(std::getline(cityFile, line))
	{
		CityCombo->Append(line);
		counter++;
	}

}



