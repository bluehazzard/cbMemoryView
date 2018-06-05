#include "cbmvwindow.h"


BEGIN_EVENT_TABLE(cbMVWindow,wxAuiNotebook)

END_EVENT_TABLE()

cbMVWindow::cbMVWindow(wxWindow* parent) : wxAuiNotebook(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_WINDOWLIST_BUTTON)
{
    //ctor
}

cbMVWindow::~cbMVWindow()
{
    //dtor
}
