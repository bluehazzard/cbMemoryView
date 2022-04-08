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
	//EVT_COMMAND_SCROLL(XRCID("ID_TEXTBYTE"), MemoryPanel::OnScroll)
	EVT_COMMAND_SCROLL(XRCID("ID_SCROLLBAR1"), MemoryPanel::OnScroll)
END_EVENT_TABLE()

MemoryPanel::MemoryPanel(wxWindow* parent)
{
	//(*Initialize(MemoryPanel)
	wxXmlResource::Get()->LoadObject(this,parent,_T("MemoryPanel"),_T("wxPanel"));
	StaticText1 = (wxStaticText*)FindWindow(XRCID("ID_STATICTEXT_ADDRESS"));
	m_Address = (wxTextCtrl*)FindWindow(XRCID("ID_TEXTCTRL_ADDRESS"));
	StaticText2 = (wxStaticText*)FindWindow(XRCID("ID_STATICTEXT_SIZE"));
	m_Size = (wxTextCtrl*)FindWindow(XRCID("ID_TEXTCTRL_SIZE"));
	m_ByteOutput = (wxTextCtrl*)FindWindow(XRCID("ID_TEXTBYTE"));
	m_AchiiOutput = (wxTextCtrl*)FindWindow(XRCID("ID_TEXTASCII"));
	SplitterWindow1 = (wxSplitterWindow*)FindWindow(XRCID("ID_SPLITTERWINDOW1"));
	ScrollBar1 = (wxScrollBar*)FindWindow(XRCID("ID_SCROLLBAR1"));

	Connect(XRCID("ID_TEXTCTRL_ADDRESS"),wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&MemoryPanel::OnTextEnter);
	Connect(XRCID("ID_TEXTCTRL_SIZE"),wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&MemoryPanel::OnTextEnter);
	//*)

	m_ByteOutput->SetFont( wxFont(9, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL) );
	m_AchiiOutput->SetFont( wxFont(9, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL) );

	ScrollBar1->SetRange(0);
	wxSize windowSize = SplitterWindow1->GetSize();
	SplitterWindow1->SetSashPosition(0.65*windowSize.GetWidth());
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

        m_sAddress = m_Address->GetValue();
        if (m_sAddress.IsEmpty())
        {
            m_ByteOutput->SetValue("Address is invalid...");
        }
        else
        {
            wxString sSize = m_Size->GetValue();
            if (sSize.ToULongLong(&m_llSize, 10))
            {
                uint64_t llAddress;
                if (m_sAddress.ToULongLong(&llAddress, 16))
                {
                    m_watch = dbg->AddMemoryRange(llAddress, m_llSize, wxEmptyString, true);
                }
                else
                {
                    m_watch = dbg->AddMemoryRange( 0, m_llSize, m_sAddress, true);
                }
                dbg->UpdateWatch(m_watch);
            }
            else
            {
                m_ByteOutput->SetValue("Size is invalid...");
            }
        }
    }
}

void MemoryPanel::OnScroll(wxScrollEvent& event)
{
    m_ByteOutput->ShowPosition(m_AchiiOutput->XYToPosition(0, event.GetPosition()));
    m_AchiiOutput->ShowPosition(m_AchiiOutput->XYToPosition(0, event.GetPosition()));
}

void MemoryPanel::DebuggerCursorChanged()
{
    if(!m_watch)
        return;

    cbDebuggerPlugin *dbg = Manager::Get()->GetDebuggerManager()->GetActiveDebugger();
    if(dbg->IsRunning())
    {
        dbg->UpdateWatch(m_watch);
    }
}

void MemoryPanel::UpdatePanel()
{
    if(!m_watch)
    {
        m_ByteOutput->SetValue(_("Cannot find m_watch!"));
        m_AchiiOutput->SetValue(_("Cannot find m_watch!"));
        ScrollBar1->SetRange(1);
        return; // WTF is going on?
    }

    wxString val;
    m_watch->GetValue(val);

    if (m_watch->GetIsValueErrorMessage())
    {
        m_ByteOutput->SetValue(val);
        m_AchiiOutput->SetValue(val);
        ScrollBar1->SetRange(1);
        return;
    }

    if(val.IsEmpty())
    {
        m_ByteOutput->SetValue(_("No result data found!"));
        m_AchiiOutput->SetValue(_("No result data found!"));
        ScrollBar1->SetRange(1);
        return; // WTF is going on?
    }

    // ----------------------------------------------------------------------------------------
    //
    // (gdb) x/40xb 0x172e750
    // 0x172e750:      0xff    0xff    0x00    0x00    0x00    0x00    0x00    0x00
    // 0x172e758:      0x00    0x00    0x00    0x00    0x00    0x00    0x00    0x00
    // 0x172e760:      0x00    0x00    0x00    0x00    0x00    0x00    0x00    0x00
    // 0x172e768:      0xc0    0xe0    0x5b    0x01    0x00    0x00    0x00    0x00
    // 0x172e770:      0xd8    0x00    0x00    0x00    0x29    0x00    0x00    0x00
    //
    // ----------------------------------------------------------------------------------------
    //
    // GDB/MI  "-data-read-memory-bytes %s %d", m_address, m_length);
    //      memory=
    //      [
    //          {
    //              begin="0x0000008284fffc40",
    //              offset="0x0000000000000000",
    //              end="0x0000008284fffc60",
    //              contents="000000000154657374204f6e650000000000000000000000000100b501000000"
    //          }
    //      ]
    //
    // ----------------------------------------------------------------------------------------

    wxString memory = wxEmptyString;
    wxString ascii = wxEmptyString;

    uint64_t llAddress = m_watch->GetAddress();
    int line = 1;
#if wxCHECK_VERSION(3, 1, 5)
    if (wxPlatformInfo::Get().GetBitness() == wxBITNESS_64)
#else
    if (wxPlatformInfo::Get().GetArchitecture() == wxARCH_64)
#endif
    {

        memory << wxString::Format("%#018llx ", llAddress); // 18 = 0x + 16 digits
        ascii  << wxString::Format("%#018llx ", llAddress); // 18 = 0x + 16 digits
    }
    else
    {
        memory << wxString::Format("%#10llx ", llAddress); // 10 = 0x + 8 digits
        ascii  << wxString::Format("%#10llx ", llAddress); // 10 = 0x + 8 digits
    }

    wxCharBuffer buff = val.To8BitData();
    wxString hBuff;
    long lBuff;
    size_t valSize = val.size();

    for(size_t i = 0; i < valSize; ++i)
    {
        if (valSize == m_llSize)
        {
            lBuff = buff[i];
            memory << wxString::Format("%02x ",(unsigned int)(0xFF&lBuff));
        }
        else
        {
            hBuff = val.Mid(i*2,2);
            if (!hBuff.ToLong(&lBuff, 16))
            {
                lBuff = 0;
            }
            memory << hBuff << ' ';
        }

        if ((lBuff > 31) && (lBuff < 126) && (lBuff != '\n'))
        {
            ascii << wxString::Format("%2c ", lBuff);
        }
        else if (lBuff == '\n')
        {
            ascii << "\\n ";
        }
        else
        {
            ascii << wxString::Format("·· ");
        }

        if (((i+1) % 32 == 0) && (i < valSize-1))
        {
            uint64_t m_llAddressDisplay = llAddress + line * 32;
#if wxCHECK_VERSION(3, 1, 5)
            if (wxPlatformInfo::Get().GetBitness() == wxBITNESS_64)
#else
            if (wxPlatformInfo::Get().GetArchitecture() == wxARCH_64)
#endif
            {
                memory << wxString::Format("\n%#018llx ", m_llAddressDisplay); // 18 = 0x + 16 digits
                ascii  << wxString::Format("\n%#018llx ", m_llAddressDisplay); // 18 = 0x + 16 digits
            }
            else
            {
                memory << wxString::Format("\n%#10llx ", m_llAddressDisplay); // 10 = 0x + 8 digits
                ascii  << wxString::Format("\n%#10llx ", m_llAddressDisplay); // 10 = 0x + 8 digits
            }

            line++;
        }
    }

    m_ByteOutput->SetValue(memory);
    m_AchiiOutput->SetValue(ascii);

    ScrollBar1->SetRange(line);
}
