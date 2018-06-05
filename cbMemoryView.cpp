#include <sdk.h> // Code::Blocks SDK
#include <configurationpanel.h>
#include "cbMemoryView.h"
#include "logmanager.h"

// Register the plugin with Code::Blocks.
// We are using an anonymous namespace so we don't litter the global one.
namespace
{
    PluginRegistrant<cbMemoryView> reg(_T("cbMemoryView"));
}


int ID_MEMORY_VIEW_WINDOW       =wxNewId();
int ID_MEMORY_VIEW_ADD_PANEL    =wxNewId();
int ID_MEMORY_VIEW_RENAME_PANEL    =wxNewId();


// events handling
BEGIN_EVENT_TABLE(cbMemoryView, cbPlugin)
    // add any events you want to handle here

    EVT_AUINOTEBOOK_TAB_RIGHT_DOWN(ID_MEMORY_VIEW_WINDOW, cbMemoryView::OnRClick)

    EVT_MENU(ID_MEMORY_VIEW_ADD_PANEL, cbMemoryView::OnAddPage)
    EVT_MENU(ID_MEMORY_VIEW_RENAME_PANEL, cbMemoryView::OnRenamePage)

END_EVENT_TABLE()

// constructor
cbMemoryView::cbMemoryView()
{
    // Make sure our resources are available.
    // In the generated boilerplate code we have no resources but when
    // we add some, it will be nice that this code is in place already ;)
    if(!Manager::LoadResource(_T("cbMemoryView.zip")))
    {
        NotifyMissingFile(_T("cbMemoryView.zip"));
    }
}

// destructor
cbMemoryView::~cbMemoryView()
{
}

void cbMemoryView::OnRClick(wxAuiNotebookEvent& event)
{
    wxMenu ctx;
    ctx.Append(ID_MEMORY_VIEW_ADD_PANEL, wxT("Add page"));

    ctx.Append(ID_MEMORY_VIEW_RENAME_PANEL, wxT("Rename page"));
    m_window->PopupMenu(&ctx);
}

void cbMemoryView::OnAddPage(wxCommandEvent& event)
{
    m_window->AddPage(new MemoryPanel(m_window), wxT("Memory View") , false);
}

void cbMemoryView::OnRenamePage(wxCommandEvent& event)
{
    int cur_page = m_window->GetSelection();
    wxTextEntryDialog nameDlg(m_window, wxT("Name of the page:"), wxT("Memory view name"), m_window->GetPageText(cur_page));
    if(nameDlg.ShowModal() == wxID_OK)
    {
        m_window->SetPageText(cur_page, nameDlg.GetValue());
    }

}

void cbMemoryView::OnAttach()
{
    // do whatever initialization you need for your plugin
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be TRUE...
    // You should check for it in other functions, because if it
    // is FALSE, it means that the application did *not* "load"
    // (see: does not need) this plugin...
    m_window = new wxAuiNotebook(Manager::Get()->GetAppWindow(),
                                 ID_MEMORY_VIEW_WINDOW,
                                 wxDefaultPosition,
                                 wxDefaultSize,
                                 wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_WINDOWLIST_BUTTON | wxAUI_NB_CLOSE_ON_ALL_TABS);

    CodeBlocksDockEvent evt(cbEVT_ADD_DOCK_WINDOW);
    evt.name = _T("SystemView");
    evt.title = _("System View Register window");
    evt.pWindow = m_window;
    evt.dockSide = CodeBlocksDockEvent::dsFloating;
    evt.desiredSize.Set(400, 300);
    evt.floatingSize.Set(400, 300);
    evt.minimumSize.Set(200, 150);
    Manager::Get()->ProcessEvent(evt);

    CodeBlocksDockEvent evt2(cbEVT_SHOW_DOCK_WINDOW);
    evt2.pWindow = m_window;
    Manager::Get()->ProcessEvent(evt2);

    m_MenuItem = new MemoryViewMenuItem(m_window);
    Manager::Get()->GetDebuggerManager()->GetMenuHandler()->RegisterWindowMenu(wxT("Memory view"), wxT("Memory view"), m_MenuItem);


    Manager::Get()->RegisterEventSink(cbEVT_DEBUGGER_UPDATE_UI,  new cbEventFunctor<cbMemoryView, CodeBlocksDebuggerEvent>(this, &cbMemoryView::OnDebuggerWinEvt));

    m_window->AddPage(new MemoryPanel(m_window), wxT("Memory View") , false);
    m_window->AddPage(new MemoryPanel(m_window), wxT("Memory View2") , false);
    m_window->AddPage(new MemoryPanel(m_window), wxT("Memory View3") , false);
    m_window->AddPage(new MemoryPanel(m_window), wxT("Memory View4") , false);


}

void cbMemoryView::OnRelease(bool appShutDown)
{
    // do de-initialization for your plugin
    // if appShutDown is true, the plugin is unloaded because Code::Blocks is being shut down,
    // which means you must not use any of the SDK Managers
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be FALSE...
}

void cbMemoryView::BuildMenu(wxMenuBar* menuBar)
{
    //The application is offering its menubar for your plugin,
    //to add any menu items you want...
    //Append any items you need in the menu...
    //NOTE: Be careful in here... The application's menubar is at your disposal.
    NotImplemented(_T("cbMemoryView::BuildMenu()"));
}

void cbMemoryView::BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data)
{
    //Some library module is ready to display a pop-up menu.
    //Check the parameter \"type\" and see which module it is
    //and append any items you need in the menu...
    //TIP: for consistency, add a separator as the first item...
    NotImplemented(_T("cbMemoryView::BuildModuleMenu()"));
}

bool cbMemoryView::BuildToolBar(wxToolBar* toolBar)
{
    //The application is offering its toolbar for your plugin,
    //to add any toolbar items you want...
    //Append any items you need on the toolbar...
    NotImplemented(_T("cbMemoryView::BuildToolBar()"));

    // return true if you add toolbar items
    return false;
}


void cbMemoryView::OnDebuggerStateEvt(CodeBlocksDebuggerEvent& evt)
{

}

void cbMemoryView::OnDebuggerWinEvt(CodeBlocksDebuggerEvent& evt)
{
    cbDebuggerWindow win = evt.GetWindow();
    Manager::Get()->GetLogManager()->Log(wxT("::OnDebuggerWinEvt"));
    if(win == DEBUGGER_WINDOW_MEMORY)
    {
        for(size_t i = 0; i < m_window->GetPageCount(); ++i)
        {
            MemoryPanel* panel = dynamic_cast<MemoryPanel*>(m_window->GetPage(i));
            if(panel)
            {
                panel->UpdatePanel();
            }
        }
    }
}
