#include "splash.h"

splash::splash(const wxString& title)
	: wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(400,400))
{
	wxPanel *panel = new wxPanel(this, wxID_ANY);
	wxButton *OK_Button = new wxButton(panel, wxID_ANY, wxT("Ok"), wxPoint(175,340), wxSize(50,50));
}
