#include "MemoryPanel.h"
#include "cbplugin.h"

//(*InternalHeaders(MemoryPanel)
#include <wx/xrc/xmlres.h>
//*)

//(*IdInit(MemoryPanel)
//*)

BEGIN_EVENT_TABLE(MemoryPanel,wxPanel)
	//(*EventTable(MemoryPanel)
	//*)
END_EVENT_TABLE()

MemoryPanel::MemoryPanel(wxWindow* parent)
{
	//(*Initialize(MemoryPanel)
	wxXmlResource::Get()->LoadObject(this,parent,_T("MemoryPanel"),_T("wxPanel"));
	StaticText1 = (wxStaticText*)FindWindow(XRCID("ID_STATICTEXT1"));
	m_Address = (wxTextCtrl*)FindWindow(XRCID("ID_TEXTCTRL1"));
	StaticText2 = (wxStaticText*)FindWindow(XRCID("ID_STATICTEXT2"));
	m_Size = (wxTextCtrl*)FindWindow(XRCID("ID_TEXTCTRL2"));
	m_ByteOutput = (wxTextCtrl*)FindWindow(XRCID("ID_TEXTCTRL3"));
	m_AchiiOutput = (wxTextCtrl*)FindWindow(XRCID("ID_TEXTCTRL4"));

	Connect(XRCID("ID_TEXTCTRL1"),wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&MemoryPanel::OnTextEnter);
	Connect(XRCID("ID_TEXTCTRL2"),wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&MemoryPanel::OnTextEnter);
	//*)

	m_ByteOutput->SetFont( wxFont(12, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL) );
	m_AchiiOutput->SetFont( wxFont(12, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL) );
}

MemoryPanel::~MemoryPanel()
{
	//(*Destroy(MemoryPanel)
	//*)
}

void MemoryPanel::OnTextEnter(wxCommandEvent& event)
{
    cbDebuggerPlugin *dbg = Manager::Get()->GetDebuggerManager()->GetActiveDebugger();
    if(dbg->IsRunning())
    {
        if(m_watch)
        {
            // Remove old watch
            dbg->DeleteWatch(m_watch);
        }

        uint64_t addr = 0;
        uint64_t size = 0;
        try
        {
            addr = std::stoull(std::string(m_Address->GetValue().ToUTF8()), 0, 0);
            size = std::stoull(std::string(m_Size->GetValue().ToUTF8()), 0, 0);
        } catch (...)
        {
            m_ByteOutput->SetValue("Size or Address are invalid...");
        }

        m_watch = dbg->AddMemoryRange( addr , size, wxEmptyString );
    }
}

void MemoryPanel::UpdatePanel()
{
    wxString memory;
    wxString ascii;
    uint8_t dot[] = {0xC2, 0xB7, 0};

    wxString val;
    m_watch->GetValue(val);

    wxCharBuffer buff = val.To8BitData();

    for(size_t i = 0; i < val.size(); ++i)
    {
        memory << wxString::Format(wxT("%2x "), buff[i]);
        if(buff[i] < 128)
            ascii << wxString::Format(wxT("%c "), buff[i]);
        else
            ascii << wxString::FromUTF8((char*) dot);


        if(i % 32 == 0)
        {
            memory << wxT("\n");
            ascii  << wxT("\n");
        }
    }

    m_ByteOutput->SetValue(memory);
    m_AchiiOutput->SetValue(ascii);
}
