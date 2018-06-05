/***************************************************************
 * Name:      cbMemoryView
 * Purpose:   Code::Blocks plugin
 * Author:    Bluehazzard (bluehazzard2@gmail.com)
 * Created:   2018-06-05
 * Copyright: Bluehazzard
 * License:   GPL
 **************************************************************/

#ifndef CBMEMORYVIEW_H_INCLUDED
#define CBMEMORYVIEW_H_INCLUDED

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>
#include "MemoryPanel.h"
#include "include/cbmvwindow.h"

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <cbplugin.h> // for "class cbPlugin"
#include "debuggermanager.h"
#include "cbdebugger_interfaces.h"
#include <deque>

class MemoryViewMenuItem : public cbDebuggerWindowMenuItem
{
    public:
    MemoryViewMenuItem(wxWindow* window ) : m_window(window) {};
    virtual ~MemoryViewMenuItem() {};

    virtual void OnClick(bool enable)
    {
        CodeBlocksDockEvent evt2(enable ? cbEVT_SHOW_DOCK_WINDOW : cbEVT_HIDE_DOCK_WINDOW);
        evt2.pWindow = m_window;
        Manager::Get()->ProcessEvent(evt2);
    };

    virtual bool IsEnabled()
    {
        return true;
    };

    virtual bool IsChecked()
    {
        return IsWindowReallyShown(m_window);
    };

private:
    wxWindow* m_window;
};

class cbMemoryView : public cbPlugin
{
    public:
        /** Constructor. */
        cbMemoryView();
        /** Destructor. */
        virtual ~cbMemoryView();

        /** This method is called by Code::Blocks and is used by the plugin
          * to add any menu items it needs on Code::Blocks's menu bar.\n
          * It is a pure virtual method that needs to be implemented by all
          * plugins. If the plugin does not need to add items on the menu,
          * just do nothing ;)
          * @param menuBar the wxMenuBar to create items in
          */
        virtual void BuildMenu(wxMenuBar* menuBar);

        /** This method is called by Code::Blocks core modules (EditorManager,
          * ProjectManager etc) and is used by the plugin to add any menu
          * items it needs in the module's popup menu. For example, when
          * the user right-clicks on a project file in the project tree,
          * ProjectManager prepares a popup menu to display with context
          * sensitive options for that file. Before it displays this popup
          * menu, it asks all attached plugins (by asking PluginManager to call
          * this method), if they need to add any entries
          * in that menu. This method is called.\n
          * If the plugin does not need to add items in the menu,
          * just do nothing ;)
          * @param type the module that's preparing a popup menu
          * @param menu pointer to the popup menu
          * @param data pointer to FileTreeData object (to access/modify the file tree)
          */
        virtual void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0);

        /** This method is called by Code::Blocks and is used by the plugin
          * to add any toolbar items it needs on Code::Blocks's toolbar.\n
          * It is a pure virtual method that needs to be implemented by all
          * plugins. If the plugin does not need to add items on the toolbar,
          * just do nothing ;)
          * @param toolBar the wxToolBar to create items on
          * @return The plugin should return true if it needed the toolbar, false if not
          */
        virtual bool BuildToolBar(wxToolBar* toolBar);
    protected:
        /** Any descendent plugin should override this virtual method and
          * perform any necessary initialization. This method is called by
          * Code::Blocks (PluginManager actually) when the plugin has been
          * loaded and should attach in Code::Blocks. When Code::Blocks
          * starts up, it finds and <em>loads</em> all plugins but <em>does
          * not</em> activate (attaches) them. It then activates all plugins
          * that the user has selected to be activated on start-up.\n
          * This means that a plugin might be loaded but <b>not</b> activated...\n
          * Think of this method as the actual constructor...
          */
        virtual void OnAttach();

        /** Any descendent plugin should override this virtual method and
          * perform any necessary de-initialization. This method is called by
          * Code::Blocks (PluginManager actually) when the plugin has been
          * loaded, attached and should de-attach from Code::Blocks.\n
          * Think of this method as the actual destructor...
          * @param appShutDown If true, the application is shutting down. In this
          *         case *don't* use Manager::Get()->Get...() functions or the
          *         behaviour is undefined...
          */
        virtual void OnRelease(bool appShutDown);

        virtual void OnRClick(wxAuiNotebookEvent& event);
        virtual void OnAddPage(wxCommandEvent& event);
        virtual void OnRenamePage(wxCommandEvent& event);

        virtual void OnDebuggerStateEvt(CodeBlocksDebuggerEvent& evt);
        virtual void OnDebuggerWinEvt(CodeBlocksDebuggerEvent& evt);

    private:
        typedef std::deque<MemoryPanel*> PanelList;

        PanelList   m_panelList;
        wxAuiNotebook* m_window;
        MemoryViewMenuItem* m_MenuItem;

        DECLARE_EVENT_TABLE();
};

#endif // CBMEMORYVIEW_H_INCLUDED
