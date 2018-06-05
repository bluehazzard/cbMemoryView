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

	m_ByteOutput->SetFont( wxFont(9, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL) );
	m_AchiiOutput->SetFont( wxFont(9, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL) );
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

        m_addr = addr;

        m_watch = dbg->AddMemoryRange( m_addr , size, wxEmptyString );
    }
}

void MemoryPanel::UpdatePanel()
{
    wxString memory;
    wxString ascii;

    wxString val;

    if(!m_watch)
        return; // WTF is going on?

    m_watch->GetValue(val);

    wxCharBuffer buff = val.To8BitData();
    memory << wxT("           ");
    ascii  << wxT("           ");

    for(size_t i = 0; i < 32; ++i)
    {
        memory << wxString::Format(wxT("%02x "),(unsigned int)(0xFF&i));
        ascii  << wxString::Format(wxT("%02x "),(unsigned int)(0xFF&i));
    }

    memory << wxT("\n");
    memory << wxString::Format(wxT("0x%08lx "),(m_addr));
    ascii << wxT("\n");
    ascii  << wxString::Format(wxT("0x%08lx "),(m_addr));

    int line = 1;

    for(size_t i = 0; i < val.size(); ++i)
    {
        char tmp = buff[i];
        memory << wxString::Format(wxT("%02x "),(unsigned int)(0xFF&tmp));
        if(buff[i] > 31 && buff[i] < 126 && buff[i] != '\n')
            ascii << wxString::Format(wxT("%2c "), buff[i]);
        else if (buff[i] == '\n')
            ascii << wxT("\\n ");
        else
            ascii << wxString::FromUTF8(u8"·· ");


        if((i+1) % 32 == 0)
        {
            memory << wxT("\n");
            ascii  << wxT("\n");
            memory << wxString::Format(wxT("0x%08lx "),(m_addr + line * 32));
            ascii  << wxString::Format(wxT("0x%08lx "),(m_addr + line * 32));
            line++;
        }
    }

    m_ByteOutput->SetValue(memory);
    m_AchiiOutput->SetValue(ascii);
}
