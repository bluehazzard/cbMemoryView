#ifndef MEMORYPANEL_H
#define MEMORYPANEL_H

//(*Headers(MemoryPanel)
#include <wx/panel.h>
#include <wx/scrolbar.h>
#include <wx/splitter.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

#include <memory>
#include "debuggermanager.h"

class MemoryPanel: public wxPanel
{
	public:

		MemoryPanel(wxWindow* parent);
		virtual ~MemoryPanel();

		//(*Declarations(MemoryPanel)
		wxScrollBar* ScrollBar1;
		wxSplitterWindow* SplitterWindow1;
		wxStaticText* StaticText1;
		wxStaticText* StaticText2;
		wxTextCtrl* m_AchiiOutput;
		wxTextCtrl* m_Address;
		wxTextCtrl* m_ByteOutput;
		wxTextCtrl* m_Size;
		//*)

		void UpdatePanel();
		void OnScroll(wxScrollEvent& event);
		void DebuggerCursorChanged();

	protected:

		//(*Identifiers(MemoryPanel)
		//*)

	private:

		//(*Handlers(MemoryPanel)
		void OnTextEnter(wxCommandEvent& event);
		//*)

        wxString m_sAddress;
		std::shared_ptr<cbWatch> m_watch;

		DECLARE_EVENT_TABLE()
};

#endif
