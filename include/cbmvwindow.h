#ifndef CBMVWINDOW_H
#define CBMVWINDOW_H

#include <wx/panel.h>
#include <wx/aui/aui.h>


class cbMVWindow : public wxAuiNotebook
{
    public:
        cbMVWindow(wxWindow* parent);
        virtual ~cbMVWindow();

    protected:

    private:

        DECLARE_EVENT_TABLE();
};

#endif // CBMVWINDOW_H
