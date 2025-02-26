#include <EditorFramework/EditorFrameworkPCH.h>

#include <EditorFramework/Assets/AssetBrowserDlg.moc.h>
#include <EditorFramework/Dialogs/DashboardDlg.moc.h>
#include <EditorFramework/EditorApp/EditorApp.moc.h>
#include <EditorFramework/Settings/SettingsTab.moc.h>
#include <GuiFoundation/ActionViews/MenuBarActionMapView.moc.h>
#include <QDesktopServices>

EZ_IMPLEMENT_SINGLETON(ezQtSettingsTab);

ezString ezQtSettingsTab::GetWindowIcon() const
{
  return ""; //:/GuiFoundation/EZ-logo.svg";
}

ezString ezQtSettingsTab::GetDisplayNameShort() const
{
  return "";
}

void ezQtEditorApp::ShowSettingsDocument()
{
  ezQtSettingsTab* pSettingsTab = ezQtSettingsTab::GetSingleton();

  if (pSettingsTab == nullptr)
  {
    pSettingsTab = new ezQtSettingsTab();
  }
}

void ezQtEditorApp::CloseSettingsDocument()
{
  ezQtSettingsTab* pSettingsTab = ezQtSettingsTab::GetSingleton();

  if (pSettingsTab != nullptr)
  {
    pSettingsTab->CloseDocumentWindow();
  }
}

ezQtSettingsTab::ezQtSettingsTab()
  : ezQtDocumentWindow("Settings")
  , m_SingletonRegistrar(this)
{
  ezQtMenuBarActionMapView* pMenuBar = static_cast<ezQtMenuBarActionMapView*>(menuBar());
  ezActionContext context;
  context.m_sMapping = "SettingsTabMenuBar";
  context.m_pDocument = nullptr;
  pMenuBar->SetActionContext(context);

  FinishWindowCreation();
}

ezQtSettingsTab::~ezQtSettingsTab() = default;

bool ezQtSettingsTab::InternalCanCloseWindow()
{
  // if this is the last window, prevent closing it
  return ezQtDocumentWindow::GetAllDocumentWindows().GetCount() > 1;
}

void ezQtSettingsTab::InternalCloseDocumentWindow()
{
  // make sure this instance isn't used anymore
  UnregisterSingleton();
}
