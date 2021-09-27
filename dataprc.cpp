#include <iostream>
//#include <unistd.h>
#include <iomanip>
#include <fstream>
#include "swephexp.h"



std::ifstream datafile;		//File from GUI
std::ofstream infile;		//File for GUI

struct DData			//Date and time data
{
	int hour;		
	double minute;		
	double second;
	int day;
	int month;
	int year;
};

std::ostringstream CBodyDL[10];		//Hold longitude values to send to GUI
std::ostringstream CBodyDLA[10];	//Same as above for latitude values
double CLong[11];
double CLat[11];
DData UTD;

double Sidereal_Time;
double Julian_Date;
double Te_Date;			//Terrestrial or Ephemperis time
double TEcliptic;		//True obliquity of the ecliptic
double DeltaT;			//Hold deltaT for period

double longGUI[3];		//hold the 3 values that hold the longitude from GUI
double latGUI[3];		//Same as above for latitude

int longIn;			//Hold longitude from GUI
int latIn;			//Hold latitude from GUI

void readfile(std::string fileHandle);
void writefile(std::string fileHandle);

char serr[256];
char PName[20];			//character array to store current planet name

double ASCMC[10];
double cusps[13];


int main(void)
{
	std::cout << std::fixed;		//set fixed precision
//		datafile >> longIn;
//		datafile >> longIn;
	std::cout << std::setprecision(6);	//set precision to 6 decimal places
	double pdat[6];			//planet data
//	int iflag = SEFLG_SWIEPH;	//default flag for swe_calc()
int iflag = SEFLG_SWIEPH | SEFLG_SPEED | SEFLG_EQUATORIAL;
//	int ipl; 


	swe_set_ephe_path((char*)"./");		//Set ephemeris file path to local directory
	readfile("testfile");			//read file from GUI

	std::cout << "longIn: " << longIn << "\t" << latIn << std::endl;
	
	Julian_Date = swe_julday(UTD.year, UTD.month, UTD.day, UTD.minute, SE_GREG_CAL);
	std::cout << Julian_Date << std::endl;
	Te_Date = Julian_Date - swe_deltat_ex(Julian_Date, SEFLG_SWIEPH, serr);
	Sidereal_Time = swe_sidtime(Julian_Date);
	DeltaT = swe_deltat(Julian_Date) * 86400.0;

	//Split obliquity of the ecliptic into degrees
	int a,b,c,d;
	double f;
	swe_split_deg(Te_Date, 0, &a, &b, &c, &f, &d);
	f = (double)c + f;
//	std::cout << "TE Split: " << a << "°" << b << "'" << f << d << std::endl;


	for(int i = SE_SUN; i < SE_PLUTO+1; i++)
	{
		swe_calc(Julian_Date, i, iflag, pdat, serr);
		int a,b,c,d;
		double f;
		swe_get_planet_name(i, PName);
		for(int ii = 0; ii < 2; ii++)
		{
			swe_split_deg(pdat[ii], 0, &a, &b, &c, &f, &d);
			f = (double)c + f;
		}
		//std::cout << PName <<  pdat[0] << std::endl;
		//std::cout << pdat[1] << std::endl;
		swe_split_deg(pdat[0], 0, &a, &b, &c, &f, &d);
                f = (double)c + f;

		CBodyDL[i] << a << "°" << b << "'" << f << d;
		
		swe_split_deg(pdat[1], 0, &a, &b, &c, &f, &d);
		f = (double)c + f;


		CBodyDLA[i] << a << "°" << b << "'" << f << d;
	
		//Now save long and lat to a more permanent array
		CLong[i] = pdat[0];
		CLat[i] = pdat[1];

	}


	swe_calc(Te_Date, SE_ECL_NUT, 0, pdat, serr);
	TEcliptic = pdat[0];

//	std::cout << "TE" << TEcliptic << std::endl;

	//Testing output
	for(int i = 0; i < SE_PLUTO+1; i++)
	{
	}

	//STARTING HOUSE DATA
	//double cusps[13];
	double xpin[2];
	xpin[0] = 260.076474;
	xpin[1] = -0.003022;
	swe_houses(Julian_Date, latIn, longIn, 'P', cusps, ASCMC);	//calculate house 

	std::cout << "\n\n\n\n" << std::endl;

	for(int i = 0; i < 10; i++)
	{
	swe_get_planet_name(i, PName);
	xpin[0] = CLong[i];
	xpin[1] = CLat[i];
	std::cout << PName << " " << swe_house_pos(ASCMC[2], 0, 23.436395, 'P', xpin, serr) << std::endl;
	}

	for(int i = 0; i < 12; i ++)
	{
		std::cout << cusps[i] << std::endl;
	}

	for(int i = 1; i < 13; i++)
	{
//	std::cout << "Cusp " << i << " " << cusps[i] << std::endl;
	}
	writefile("tmp.in");

	std::cout << "Local SR " << ASCMC[2] / 15 << std::endl;
	std::cout << "OLD SR " << Sidereal_Time << std::endl;
	
	return 0;
}

void readfile(std::string fileHandle)
{
	std::string tmpSec, tmpMin, tmpHour, tmpDay, tmpMonth, tmpYear;
	datafile.open(fileHandle);
	datafile >> tmpDay;
	datafile >> tmpMonth;
	datafile >> tmpYear;
	datafile >> tmpHour;
	datafile >> tmpMin;
	datafile >> tmpSec;

	std::cout << "tmpSec: " << tmpSec << std::endl;

	UTD.second = std::stoi(tmpSec);
	UTD.hour = std::stoi(tmpHour);
	UTD.minute = std::stod(tmpMin);
	UTD.day = std::stoi(tmpDay);
	UTD.month = std::stoi(tmpMonth);
	UTD.year = std::stoi(tmpYear);
//	std::cout << std::setprecision(4);
//	std::cout << UTD.hour << std::endl;
//	std::cout << UTD.minute << std::endl;
//	std::cout << UTD.minute / 60 << std::endl;
	UTD.minute = UTD.hour + (UTD.minute / 60);
	UTD.minute = UTD.minute + (UTD.second / 3600);

	//long/lat
	for(int i = 0; i < 3; i++)
	{
		datafile >> longGUI[i];
	}
	for(int i = 0; i < 3; i++)
	{
		datafile >> latGUI[i];
	}

	std::cout << std::defaultfloat;
	std::cout << "Longitude: " << longGUI[0] << "°" << longGUI[1] << "'" << longGUI[2] << "''" << std::endl;
	std::cout << "Latitude: " << latGUI[0] << "°" << latGUI[1] << "'" << latGUI[2] << "''" << std::endl;
	std::cout << std::fixed;


	//datafile >> longIn;
	//datafile >> latIn;
}

void writefile(std::string fileHandle)
{
	infile << std::fixed;
	infile.open(fileHandle);//file:///home/phil/astr/testfile

	infile << Julian_Date << std::endl;		//Output Julian Date
	infile << Te_Date << std::endl;			//Terestrial Date
	infile << Sidereal_Time << std::endl;		//Sidereal Time
	infile << TEcliptic << std::endl;		//True obliquity of the eliptic
//	infile << DeltaT << std::endl;
//	infile << swe_deltat(Julian_Date) * 86400.0 << std::endl;

	for(int iterator = 0; iterator < SE_PLUTO+1; iterator++)	//Send CBodyDL array to GUI//
	{
		infile << CBodyDL[iterator].str() << std::endl;	
	}
	for(int iterator = 0; iterator < SE_PLUTO+1; iterator++)	//Send CBodyDLA array to GUI/
	{
		infile << CBodyDLA[iterator].str() << std::endl;
	}								/////////////////////////////
/*Write house data to output
	double xpin[2];
	for(int i = 0; i < 10; i++)
	{
		xpin[0] = CLong[i];
		xpin[1] = CLat[i];
		infile << (int)swe_house_pos(ASCMC[2],0, 23.436395, 'P', xpin, serr) << std::endl;
	}*/

	for(int iterator = 1; iterator < 13; iterator++)
	{
		infile << cusps[iterator] << std::endl;
	}

	infile.close();
}

